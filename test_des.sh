#!/bin/bash

# Test script for ft_ssl des — compares output against openssl des-cbc
# -k behaves like openssl -K (raw hex key, no password derivation)
# -v behaves like openssl -iv
# -a base64 encode/decode of ciphertext
# Password-based tests (-p/-s) use internal roundtrip only (PBKDF2 differs from openssl EVP_BytesToKey)

BIN="./ft_ssl"
PASS=0
FAIL=0

RED='\033[0;31m'
GRN='\033[0;32m'
YLW='\033[0;33m'
RST='\033[0m'

ok()   { echo -e "${GRN}[PASS]${RST} $1"; ((PASS++)); }
fail() { echo -e "${RED}[FAIL]${RST} $1"; ((FAIL++)); }
info() { echo -e "${YLW}[INFO]${RST} $1"; }

# Compare two hex strings (ignore case)
cmp_hex() {
    local got="$1" exp="$2" label="$3"
    if [ "$(echo "$got" | tr '[:upper:]' '[:lower:]')" = "$(echo "$exp" | tr '[:upper:]' '[:lower:]')" ]; then
        ok "$label"
    else
        fail "$label"
        echo "       got: $got"
        echo "       exp: $exp"
    fi
}

# Run ft_ssl des and capture binary output as hex
ft_hex() {
    "$BIN" des "$@" 2>/dev/null | xxd -p | tr -d '\n'
}

# Run openssl des-cbc with raw key flags and capture binary output as hex
# OpenSSL 3.x moved DES to the legacy provider — must pass both providers
ssl_hex() {
    openssl enc -provider legacy -provider default -des-cbc "$@" 2>/dev/null | xxd -p | tr -d '\n'
}

# ─────────────────────────────────────────────────────────────────────────────
echo ""
info "Building..."
make -C "$(dirname "$BIN")" -s 2>/dev/null || { echo "Build failed"; exit 1; }
echo ""

KEY="AABBCCDDAABBCCDD"
IV="0102030405060708"
MSG="Hello, World!   "   # 16 bytes — exact DES block boundary
MSG8="Hi there"           # 8 bytes — exactly one block
LONGMSG="The quick brown fox jumps over the lazy dog."

# ─────────────────────────────────────────────────────────────────────────────
echo "════════════════════════════════════════"
echo " RAW KEY (-k / -K)  +  IV (-v / -iv)"
echo "════════════════════════════════════════"

# --- Encrypt: echo into stdin ---
label="encrypt stdin (16-byte msg)"
ft=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(echo -n "$MSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="encrypt stdin (8-byte msg)"
ft=$(echo -n "$MSG8" | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(echo -n "$MSG8" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="encrypt stdin (multi-block msg)"
ft=$(echo -n "$LONGMSG" | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(echo -n "$LONGMSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

# --- Decrypt ---
label="decrypt stdin (16-byte msg)"
cipher=$(echo -n "$MSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
ft=$(printf "%b" "$(echo "$cipher" | sed 's/../\\x&/g')" | ft_hex -d -k "$KEY" -v "$IV")
exp=$(echo -n "$MSG" | xxd -p | tr -d '\n')
cmp_hex "$ft" "$exp" "$label"

label="decrypt stdin (multi-block msg)"
cipher=$(echo -n "$LONGMSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
ft=$(printf "%b" "$(echo "$cipher" | sed 's/../\\x&/g')" | ft_hex -d -k "$KEY" -v "$IV")
exp=$(echo -n "$LONGMSG" | xxd -p | tr -d '\n')
cmp_hex "$ft" "$exp" "$label"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " BASE64 FLAG (-a)"
echo "════════════════════════════════════════"

label="encrypt + base64 (-a -e)"
ft=$(echo -n "$MSG" | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null)
ssl=$(echo -n "$MSG" | openssl enc -provider legacy -provider default -des-cbc -K "$KEY" -iv "$IV" -nosalt -e -a 2>/dev/null)
if [ "$ft" = "$ssl" ]; then
    ok "$label"
else
    fail "$label"
    echo "       got: $ft"
    echo "       exp: $ssl"
fi

label="decrypt + base64 (-a -d)"
cipher64=$(echo -n "$MSG" | openssl enc -provider legacy -provider default -des-cbc -K "$KEY" -iv "$IV" -nosalt -e -a 2>/dev/null)
ft=$(echo "$cipher64" | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
if [ "$ft" = "$MSG" ]; then
    ok "$label"
else
    fail "$label"
    echo "       got: $(echo -n "$ft" | xxd -p)"
    echo "       exp: $(echo -n "$MSG" | xxd -p)"
fi

label="encrypt+base64 roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
if [ "$roundtrip" = "$LONGMSG" ]; then
    ok "$label"
else
    fail "$label"
    echo "       got: $roundtrip"
    echo "       exp: $LONGMSG"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " FILE I/O (-i / -o)"
echo "════════════════════════════════════════"

TMP_IN=$(mktemp)
TMP_OUT=$(mktemp)
TMP_DEC=$(mktemp)
echo -n "$LONGMSG" > "$TMP_IN"

label="encrypt to file (-i input -o output)"
"$BIN" des -e -k "$KEY" -v "$IV" -i "$TMP_IN" -o "$TMP_OUT" 2>/dev/null
ft=$(xxd -p "$TMP_OUT" | tr -d '\n')
ssl=$(ssl_hex -K "$KEY" -iv "$IV" -nosalt -e < "$TMP_IN")
cmp_hex "$ft" "$ssl" "$label"

label="decrypt from file (-i input -o output)"
"$BIN" des -d -k "$KEY" -v "$IV" -i "$TMP_OUT" -o "$TMP_DEC" 2>/dev/null
ft=$(xxd -p "$TMP_DEC" | tr -d '\n')
exp=$(xxd -p "$TMP_IN" | tr -d '\n')
cmp_hex "$ft" "$exp" "$label"

rm -f "$TMP_IN" "$TMP_OUT" "$TMP_DEC"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " ENCRYPT DEFAULT (no -e flag)"
echo "════════════════════════════════════════"

label="encrypt is default (no -e)"
ft=$(echo -n "$MSG" | ft_hex -k "$KEY" -v "$IV")
ssl=$(echo -n "$MSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DIFFERENT KEYS / IVs"
echo "════════════════════════════════════════"

for KEY2 in "0000000000000000" "FFFFFFFFFFFFFFFF" "133457799BBCDFF1"; do
    for IV2 in "0000000000000000" "AABBCCDDEEFF0011"; do
        label="key=$KEY2 iv=$IV2"
        ft=$(echo -n "$MSG" | ft_hex -e -k "$KEY2" -v "$IV2")
        ssl=$(echo -n "$MSG" | ssl_hex -K "$KEY2" -iv "$IV2" -nosalt -e)
        cmp_hex "$ft" "$ssl" "$label"
    done
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " PASSWORD-BASED KEY DERIVATION (-p / -s)"
echo " (internal roundtrip only — PBKDF2 differs from openssl EVP_BytesToKey)"
echo "════════════════════════════════════════"

for PASSWD in "password" "42isnice" "s3cr3t!"; do
    label="password roundtrip (-p '$PASSWD')"
    roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -p "$PASSWD" 2>/dev/null | "$BIN" des -d -p "$PASSWD" 2>/dev/null)
    if [ "$roundtrip" = "$LONGMSG" ]; then
        ok "$label"
    else
        fail "$label"
        echo "       got: $roundtrip"
        echo "       exp: $LONGMSG"
    fi
done

for SALT2 in "AABBCCDD11223344" "0000000000000000"; do
    label="password+salt roundtrip (salt=$SALT2)"
    roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -p "mypassword" -s "$SALT2" 2>/dev/null | "$BIN" des -d -p "mypassword" -s "$SALT2" 2>/dev/null)
    if [ "$roundtrip" = "$LONGMSG" ]; then
        ok "$label"
    else
        fail "$label"
        echo "       got: $roundtrip"
        echo "       exp: $LONGMSG"
    fi
done

label="password+salt+base64 roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -a -p "hello42" -s "DEADBEEFCAFEBABE" 2>/dev/null | "$BIN" des -d -a -p "hello42" -s "DEADBEEFCAFEBABE" 2>/dev/null)
if [ "$roundtrip" = "$LONGMSG" ]; then
    ok "$label"
else
    fail "$label"
    echo "       got: $roundtrip"
    echo "       exp: $LONGMSG"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " KNOWN VECTORS"
echo "════════════════════════════════════════"

# NIST SP 800-17 Table B.1 — key=0133457799BBCDFF, plaintext=0123456789ABCDEF → ciphertext=85E813540F0AB405
# This is ECB; skip if your implementation is CBC-only.
# Below are CBC vectors verified against openssl.

label="known vector: key=0000000000000000 iv=0000000000000000 msg=0000000000000000"
ZMSG=$(printf '\x00\x00\x00\x00\x00\x00\x00\x00')
ft=$(printf '\x00\x00\x00\x00\x00\x00\x00\x00' | ft_hex -e -k "0000000000000000" -v "0000000000000000")
ssl=$(printf '\x00\x00\x00\x00\x00\x00\x00\x00' | ssl_hex -K "0000000000000000" -iv "0000000000000000" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " BLOCK BOUNDARY SIZES — encrypt vs openssl"
echo " (every size 1–24 bytes; covers all PKCS7 pad amounts)"
echo "════════════════════════════════════════"

for n in $(seq 1 24); do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'X')
    ft=$(printf '%s' "$msg" | ft_hex -e -k "$KEY" -v "$IV")
    ssl=$(printf '%s' "$msg" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
    cmp_hex "$ft" "$ssl" "encrypt ${n}-byte input"
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " BLOCK BOUNDARY SIZES — decrypt roundtrip (via base64)"
echo "════════════════════════════════════════"

for n in $(seq 1 24); do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'X')
    roundtrip=$(printf '%s' "$msg" \
        | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null \
        | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
    if [ "$roundtrip" = "$msg" ]; then
        ok "roundtrip ${n}-byte input"
    else
        fail "roundtrip ${n}-byte input"
        echo "       got: $(printf '%s' "$roundtrip" | xxd -p)"
        echo "       exp: $(printf '%s' "$msg" | xxd -p)"
    fi
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " PKCS7 UNPADDING — decrypt gives back exact original length"
echo "════════════════════════════════════════"

for n in $(seq 1 24); do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'Y')
    actual=$(printf '%s' "$msg" \
        | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null \
        | "$BIN" des -d -k "$KEY" -v "$IV" 2>/dev/null \
        | wc -c)
    if [ "$actual" -eq "$n" ]; then
        ok "PKCS7 unpad: ${n}-byte message recovers ${n} bytes"
    else
        fail "PKCS7 unpad: ${n}-byte message recovers ${n} bytes"
        echo "       got: $actual bytes"
    fi
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " CBC MODE PROPERTIES"
echo "════════════════════════════════════════"

# Two identical 8-byte plaintext blocks must produce different ciphertext blocks
IDENT=$(head -c 16 /dev/zero | tr '\0' 'A')  # "AAAAAAAAAAAAAAAA"
label="identical plaintext blocks → different ciphertext blocks"
cipher=$(printf '%s' "$IDENT" | ft_hex -e -k "$KEY" -v "$IV")
c1="${cipher:0:16}"
c2="${cipher:16:16}"
if [ "$c1" != "$c2" ]; then
    ok "$label"
else
    fail "$label"
    echo "       both blocks: $c1  (should differ in CBC)"
fi

# Changing IV must change the ciphertext
label="different IV → different ciphertext"
c1=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "0000000000000001")
c2=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "0000000000000002")
[ "$c1" != "$c2" ] && ok "$label" || { fail "$label"; echo "       ciphertexts identical despite different IVs"; }

# IV change propagates to ALL ciphertext blocks (not just block 1)
TWOBLOCK=$(head -c 16 /dev/zero | tr '\0' 'B')
label="IV change affects all ciphertext blocks (CBC avalanche)"
c_iv1=$(printf '%s' "$TWOBLOCK" | ft_hex -e -k "$KEY" -v "0000000000000001")
c_iv2=$(printf '%s' "$TWOBLOCK" | ft_hex -e -k "$KEY" -v "0000000000000002")
blk1_same=0; blk2_same=0
[ "${c_iv1:0:16}"  = "${c_iv2:0:16}"  ] && blk1_same=1
[ "${c_iv1:16:16}" = "${c_iv2:16:16}" ] && blk2_same=1
if [ "$blk1_same" -eq 0 ] && [ "$blk2_same" -eq 0 ]; then
    ok "$label"
else
    fail "$label"
    echo "       block1_same=$blk1_same  block2_same=$blk2_same"
fi

# Changing the key must change the ciphertext
label="different key → different ciphertext"
c1=$(echo -n "$MSG" | ft_hex -e -k "0000000000000000" -v "$IV")
c2=$(echo -n "$MSG" | ft_hex -e -k "0000000000000002" -v "$IV")
[ "$c1" != "$c2" ] && ok "$label" || { fail "$label"; echo "       ciphertexts identical despite different keys"; }

# Identical key+IV+plaintext must always produce identical ciphertext (determinism)
label="same key+IV+plaintext → same ciphertext (determinism)"
c1=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "$IV")
c2=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "$IV")
[ "$c1" = "$c2" ] && ok "$label" || { fail "$label"; echo "       non-deterministic output!"; }

# A single-bit difference in the key must change the ciphertext
label="1-bit key difference → different ciphertext"
c1=$(echo -n "$MSG" | ft_hex -e -k "AABBCCDDAABBCCDD" -v "$IV")
c2=$(echo -n "$MSG" | ft_hex -e -k "AABBCCDDAABBCCDF" -v "$IV")  # flip non-parity bit (bit 2 from LSB)
[ "$c1" != "$c2" ] && ok "$label" || { fail "$label"; echo "       1-bit key change had no effect"; }

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " BASE64 PADDING CHARACTERS ('=')"
echo " ciphertext % 3 == 2  →  1 '='"
echo " ciphertext % 3 == 1  →  2 '='"
echo " ciphertext % 3 == 0  →  no '='"
echo "════════════════════════════════════════"

# 8-byte ciphertext  (1-7 byte input)  → 8  % 3 = 2 → 1 '='
# 16-byte ciphertext (8-15 byte input) → 16 % 3 = 1 → 2 '='
# 24-byte ciphertext (16-23 byte input)→ 24 % 3 = 0 → no '='
# 32-byte ciphertext (24-31 byte input)→ 32 % 3 = 2 → 1 '='
# 40-byte ciphertext (32-39 byte input)→ 40 % 3 = 1 → 2 '='
# 48-byte ciphertext (40-47 byte input)→ 48 % 3 = 0 → no '='

for n in 1 7 8 15 16 23 24 31 32 39 40 47; do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'P')
    ft=$(printf '%s' "$msg" \
        | openssl enc -provider legacy -provider default -des-cbc \
            -K "$KEY" -iv "$IV" -nosalt -e -a 2>/dev/null \
        | tr -d '\n')
    got=$(printf '%s' "$msg" | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null | tr -d '\n')
    label="base64 encode ${n}-byte input"
    [ "$got" = "$ft" ] && ok "$label" || {
        fail "$label"
        echo "       got: $got"
        echo "       exp: $ft"
    }

    # Also verify the base64 roundtrip for each of these sizes
    roundtrip=$(printf '%s' "$msg" \
        | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null \
        | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
    label="base64 roundtrip ${n}-byte input"
    [ "$roundtrip" = "$msg" ] && ok "$label" || {
        fail "$label"
        echo "       got: $(printf '%s' "$roundtrip" | xxd -p)"
        echo "       exp: $(printf '%s' "$msg" | xxd -p)"
    }
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " STDIN / FILE EQUIVALENCE"
echo "════════════════════════════════════════"

TMP_EQ_IN=$(mktemp)
TMP_EQ_OUT_A=$(mktemp)
TMP_EQ_OUT_B=$(mktemp)
echo -n "$LONGMSG" > "$TMP_EQ_IN"

label="binary encrypt: stdin == -i file"
ft_stdin=$(echo -n "$LONGMSG" | ft_hex -e -k "$KEY" -v "$IV")
ft_file=$(ft_hex -e -k "$KEY" -v "$IV" -i "$TMP_EQ_IN")
cmp_hex "$ft_stdin" "$ft_file" "$label"

label="base64 encrypt: stdin == -i file"
b64_stdin=$(echo -n "$LONGMSG" | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null)
b64_file=$("$BIN" des -e -a -k "$KEY" -v "$IV" -i "$TMP_EQ_IN" 2>/dev/null)
[ "$b64_stdin" = "$b64_file" ] && ok "$label" || {
    fail "$label"
    echo "       stdin: $b64_stdin"
    echo "       file:  $b64_file"
}

label="-o file output matches stdout output"
echo -n "$LONGMSG" | "$BIN" des -e -k "$KEY" -v "$IV" -o "$TMP_EQ_OUT_A" 2>/dev/null
echo -n "$LONGMSG" | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null > "$TMP_EQ_OUT_B"
if cmp -s "$TMP_EQ_OUT_A" "$TMP_EQ_OUT_B"; then
    ok "$label"
else
    fail "$label"
    echo "       -o file: $(xxd -p "$TMP_EQ_OUT_A" | tr -d '\n')"
    echo "       stdout:  $(xxd -p "$TMP_EQ_OUT_B" | tr -d '\n')"
fi

rm -f "$TMP_EQ_IN" "$TMP_EQ_OUT_A" "$TMP_EQ_OUT_B"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " BINARY INPUT (high bytes 0x80–0xFF, no null)"
echo "════════════════════════════════════════"

label="binary input 8 bytes (0x80–0x87)"
ft=$(printf '\x80\x81\x82\x83\x84\x85\x86\x87' | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\x80\x81\x82\x83\x84\x85\x86\x87' | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="binary input 16 bytes (0xA0–0xAF)"
ft=$(printf '\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf' \
    | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf' \
    | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="binary input 7 bytes (0xF8–0xFE) — max padding 1 byte"
ft=$(printf '\xf8\xf9\xfa\xfb\xfc\xfd\xfe' | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\xf8\xf9\xfa\xfb\xfc\xfd\xfe' | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="binary input roundtrip (via base64)"
roundtrip_hex=$(printf '\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf' \
    | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null \
    | xxd -p | tr -d '\n')
cmp_hex "$roundtrip_hex" "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf" "$label"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " LARGE INPUT"
echo "════════════════════════════════════════"

LARGE100=$(head -c 100 /dev/zero | tr '\0' 'L')
LARGE1000=$(head -c 1000 /dev/zero | tr '\0' 'Z')

label="encrypt 100-byte input vs openssl"
ft=$(printf '%s' "$LARGE100" | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '%s' "$LARGE100" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="decrypt 100-byte roundtrip"
roundtrip=$(printf '%s' "$LARGE100" \
    | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
[ "$roundtrip" = "$LARGE100" ] && ok "$label" || { fail "$label"; }

label="encrypt 1000-byte input vs openssl"
ft=$(printf '%s' "$LARGE1000" | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '%s' "$LARGE1000" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="decrypt 1000-byte roundtrip"
roundtrip=$(printf '%s' "$LARGE1000" \
    | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -a -k "$KEY" -v "$IV" 2>/dev/null)
[ "$roundtrip" = "$LARGE1000" ] && ok "$label" || { fail "$label"; }

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " OUTPUT FORMAT"
echo "════════════════════════════════════════"

# Binary output must have NO trailing newline — byte count must equal ciphertext length
label="binary output: exact byte count, no trailing newline"
byte_count=$(echo -n "$MSG8" | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null | wc -c)
if [ "$byte_count" -eq 16 ]; then  # 8-byte input → 16-byte ciphertext
    ok "$label"
else
    fail "$label"
    echo "       got: $byte_count bytes (expected 16)"
fi

# Base64 output lines must not exceed 64 characters
label="base64 output: lines ≤ 64 chars"
WIDE=$(head -c 100 /dev/zero | tr '\0' 'W')
max_line=0
while IFS= read -r line; do
    len=${#line}
    [ "$len" -gt "$max_line" ] && max_line="$len"
done < <(printf '%s' "$WIDE" | "$BIN" des -e -a -k "$KEY" -v "$IV" 2>/dev/null)
if [ "$max_line" -le 64 ]; then
    ok "$label"
else
    fail "$label"
    echo "       longest line: $max_line chars (expected ≤ 64)"
fi

# Decrypt output byte count equals original plaintext length
label="decrypt output: exact plaintext byte count"
byte_count=$(echo -n "$MSG" | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -k "$KEY" -v "$IV" 2>/dev/null | wc -c)
if [ "$byte_count" -eq ${#MSG} ]; then
    ok "$label"
else
    fail "$label"
    echo "       got: $byte_count bytes (expected ${#MSG})"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " SPECIAL / WEAK DES KEYS"
echo "════════════════════════════════════════"

# The 4 DES weak keys (self-inverse in ECB; still valid in CBC — verify vs openssl)
for WKEY in "0101010101010101" "FEFEFEFEFEFEFEFE" "E0E0E0E0F1F1F1F1" "1F1F1F1F0E0E0E0E"; do
    ft=$(echo -n "$MSG" | ft_hex -e -k "$WKEY" -v "$IV")
    ssl=$(echo -n "$MSG" | ssl_hex -K "$WKEY" -iv "$IV" -nosalt -e)
    cmp_hex "$ft" "$ssl" "weak key $WKEY"
done

# Key with alternating nibbles
ft=$(echo -n "$MSG" | ft_hex -e -k "A5A5A5A5A5A5A5A5" -v "$IV")
ssl=$(echo -n "$MSG" | ssl_hex -K "A5A5A5A5A5A5A5A5" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "alternating-nibble key A5A5..."

# Single-bit keys
for BKEY in "0100000000000000" "0000000000000002" "8000000000000000"; do
    ft=$(echo -n "$MSG" | ft_hex -e -k "$BKEY" -v "$IV")
    ssl=$(echo -n "$MSG" | ssl_hex -K "$BKEY" -iv "$IV" -nosalt -e)
    cmp_hex "$ft" "$ssl" "single-bit key $BKEY"
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " ADDITIONAL KNOWN DES-CBC VECTORS"
echo " (FIPS 81 plaintext, verified vs openssl)"
echo "════════════════════════════════════════"

# FIPS 81 CBC example: "Now is the time for all " (24 bytes)
FIPS_KEY="0123456789ABCDEF"
FIPS_IV="1234567890ABCDEF"
FIPS_MSG="Now is the time for all "

ft=$(echo -n "$FIPS_MSG" | ft_hex -e -k "$FIPS_KEY" -v "$FIPS_IV")
ssl=$(echo -n "$FIPS_MSG" | ssl_hex -K "$FIPS_KEY" -iv "$FIPS_IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "FIPS-81 plaintext: 'Now is the time for all '"

# Decrypt FIPS vector
cipher=$(echo -n "$FIPS_MSG" | ssl_hex -K "$FIPS_KEY" -iv "$FIPS_IV" -nosalt -e)
ft=$(printf "%b" "$(echo "$cipher" | sed 's/../\\x&/g')" | ft_hex -d -k "$FIPS_KEY" -v "$FIPS_IV")
exp=$(echo -n "$FIPS_MSG" | xxd -p | tr -d '\n')
cmp_hex "$ft" "$exp" "FIPS-81 decrypt"

# All-0xFF plaintext
FF8=$(printf '\xff\xff\xff\xff\xff\xff\xff\xff')
ft=$(printf '\xff\xff\xff\xff\xff\xff\xff\xff' | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\xff\xff\xff\xff\xff\xff\xff\xff' | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "all-0xFF 8-byte plaintext"

# Mixed-byte 16-byte plaintext
MIXED=$(printf '\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10')
ft=$(printf '\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10' \
    | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10' \
    | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "mixed-byte 16-byte plaintext"

# Alternating 0x55/0xAA bytes
ALT=$(printf '\x55\xaa\x55\xaa\x55\xaa\x55\xaa')
ft=$(printf '\x55\xaa\x55\xaa\x55\xaa\x55\xaa' | ft_hex -e -k "$KEY" -v "$IV")
ssl=$(printf '\x55\xaa\x55\xaa\x55\xaa\x55\xaa' | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "alternating 0x55/0xAA 8-byte plaintext"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " ROBUSTNESS — wrong key / wrong IV"
echo "════════════════════════════════════════"

# Decrypting with wrong key must not crash (output will be garbage — don't check value)
label="wrong key: no crash"
echo -n "$MSG" | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -k "DEADBEEFDEADBEEF" -v "$IV" 2>/dev/null >/dev/null
[ $? -eq 0 ] && ok "$label" || fail "$label"

# Decrypting with wrong IV: first block is wrong, but binary doesn't crash
label="wrong IV: no crash"
echo -n "$MSG" | "$BIN" des -e -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des -d -k "$KEY" -v "DEADBEEFDEADBEEF" 2>/dev/null >/dev/null
[ $? -eq 0 ] && ok "$label" || fail "$label"

# Wrong IV affects ONLY the first block of decrypt output (CBC property)
label="wrong IV corrupts only block 1 of decrypted output"
TWOBLOCK_PLAIN="1234567812345678"   # two identical 8-byte blocks
cipher_hex=$(echo -n "$TWOBLOCK_PLAIN" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cipher_bin=$(printf "%b" "$(echo "$cipher_hex" | sed 's/../\\x&/g')")
# Decrypt with correct IV
correct=$(printf "%b" "$(echo "$cipher_hex" | sed 's/../\\x&/g')" \
    | ft_hex -d -k "$KEY" -v "$IV")
# Decrypt with wrong IV
wrong=$(printf "%b" "$(echo "$cipher_hex" | sed 's/../\\x&/g')" \
    | ft_hex -d -k "$KEY" -v "DEADBEEFDEADBEEF")
# Block 1 must differ, block 2 must be identical
blk1_correct="${correct:0:16}";  blk1_wrong="${wrong:0:16}"
blk2_correct="${correct:16:16}"; blk2_wrong="${wrong:16:16}"
if [ "$blk1_correct" != "$blk1_wrong" ] && [ "$blk2_correct" = "$blk2_wrong" ]; then
    ok "$label"
else
    fail "$label"
    echo "       block1: $blk1_correct vs $blk1_wrong (should differ)"
    echo "       block2: $blk2_correct vs $blk2_wrong (should match)"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " ADDITIONAL PASSWORD EDGE CASES"
echo "════════════════════════════════════════"

# Single-char password
label="single-char password roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -p "x" 2>/dev/null \
    | "$BIN" des -d -p "x" 2>/dev/null)
[ "$roundtrip" = "$LONGMSG" ] && ok "$label" || { fail "$label"; echo "       got: $roundtrip"; }

# Password with special ASCII characters
for PASSWD in "p@\$\$w0rd!" "hello world" "abc123XYZ"; do
    label="special-char password roundtrip (-p '$PASSWD')"
    roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -p "$PASSWD" 2>/dev/null \
        | "$BIN" des -d -p "$PASSWD" 2>/dev/null)
    [ "$roundtrip" = "$LONGMSG" ] && ok "$label" || {
        fail "$label"
        echo "       got: $roundtrip"
    }
done

# Long password (64 chars)
LONGPW=$(head -c 64 /dev/zero | tr '\0' 'k')
label="64-char password roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des -e -p "$LONGPW" 2>/dev/null \
    | "$BIN" des -d -p "$LONGPW" 2>/dev/null)
[ "$roundtrip" = "$LONGMSG" ] && ok "$label" || { fail "$label"; }

# Same password, different salt → different ciphertext
label="same password, different salts → different ciphertext"
c1=$(echo -n "$MSG" | ft_hex -e -p "samepass" -s "1111111111111111")
c2=$(echo -n "$MSG" | ft_hex -e -p "samepass" -s "2222222222222222")
[ "$c1" != "$c2" ] && ok "$label" || {
    fail "$label"
    echo "       ciphertexts identical despite different salts"
}

# Same salt, different password → different ciphertext
label="same salt, different passwords → different ciphertext"
c1=$(echo -n "$MSG" | ft_hex -e -p "password1" -s "AABBCCDD11223344")
c2=$(echo -n "$MSG" | ft_hex -e -p "password2" -s "AABBCCDD11223344")
[ "$c1" != "$c2" ] && ok "$label" || {
    fail "$label"
    echo "       ciphertexts identical despite different passwords"
}

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DES-EBC (ECB MODE)"
echo " Note: registered as 'des-ebc' in cipher.c"
echo "════════════════════════════════════════"

# Helper: run ft_ssl des-ebc and capture hex
ft_ecb_hex() {
    "$BIN" des-ebc "$@" 2>/dev/null | xxd -p | tr -d '\n'
}

# Helper: run openssl des-ecb and capture hex
ssl_ecb_hex() {
    openssl enc -provider legacy -provider default -des-ecb "$@" 2>/dev/null | xxd -p | tr -d '\n'
}

label="des-ebc: encrypt stdin (8-byte) vs openssl des-ecb"
ft=$(echo -n "$MSG8" | ft_ecb_hex -e -k "$KEY")
ssl=$(echo -n "$MSG8" | ssl_ecb_hex -K "$KEY" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="des-ebc: encrypt stdin (16-byte) vs openssl des-ecb"
ft=$(echo -n "$MSG" | ft_ecb_hex -e -k "$KEY")
ssl=$(echo -n "$MSG" | ssl_ecb_hex -K "$KEY" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="des-ebc: encrypt multi-block vs openssl des-ecb"
ft=$(echo -n "$LONGMSG" | ft_ecb_hex -e -k "$KEY")
ssl=$(echo -n "$LONGMSG" | ssl_ecb_hex -K "$KEY" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="des-ebc: decrypt roundtrip"
cipher=$(echo -n "$LONGMSG" | ssl_ecb_hex -K "$KEY" -nosalt -e)
ft=$(printf "%b" "$(echo "$cipher" | sed 's/../\\x&/g')" | ft_ecb_hex -d -k "$KEY")
exp=$(echo -n "$LONGMSG" | xxd -p | tr -d '\n')
cmp_hex "$ft" "$exp" "$label"

label="des-ebc: base64 roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des-ebc -e -a -k "$KEY" 2>/dev/null \
    | "$BIN" des-ebc -d -a -k "$KEY" 2>/dev/null)
[ "$roundtrip" = "$LONGMSG" ] && ok "$label" || { fail "$label"; echo "       got: $roundtrip"; }

# ECB property: identical plaintext blocks → identical ciphertext blocks (no IV chaining)
label="des-ebc: identical plaintext blocks → identical ciphertext blocks"
IDENT_ECB=$(head -c 16 /dev/zero | tr '\0' 'A')  # two identical 8-byte blocks
cipher=$(printf '%s' "$IDENT_ECB" | ft_ecb_hex -e -k "$KEY")
c1="${cipher:0:16}"
c2="${cipher:16:16}"
if [ "$c1" = "$c2" ]; then
    ok "$label"
else
    fail "$label"
    echo "       block1: $c1  block2: $c2  (should be equal in ECB)"
fi

# ECB property: no IV — passing -v must not change output
label="des-ebc: -v flag has no effect (ECB ignores IV)"
c1=$(echo -n "$MSG" | ft_ecb_hex -e -k "$KEY")
c2=$(echo -n "$MSG" | ft_ecb_hex -e -k "$KEY" -v "AABBCCDDEEFF0011")
if [ "$c1" = "$c2" ]; then
    ok "$label"
else
    fail "$label"
    echo "       without -v: $c1"
    echo "       with    -v: $c2  (should be equal)"
fi

# ECB differs from CBC on same key+plaintext
label="des-ebc output differs from des-cbc output (same key+IV)"
ft_ecb=$(echo -n "$MSG" | ft_ecb_hex -e -k "$KEY")
ft_cbc=$(echo -n "$MSG" | ft_hex -e -k "$KEY" -v "$IV")
[ "$ft_ecb" != "$ft_cbc" ] && ok "$label" || {
    fail "$label"
    echo "       ECB and CBC produced the same output — likely IV not applied in CBC"
}

# Block boundary sizes vs openssl des-ecb
for n in $(seq 1 24); do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'E')
    ft=$(printf '%s' "$msg" | ft_ecb_hex -e -k "$KEY")
    ssl=$(printf '%s' "$msg" | ssl_ecb_hex -K "$KEY" -nosalt -e)
    cmp_hex "$ft" "$ssl" "des-ebc: encrypt ${n}-byte input vs openssl"
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DES-CBC (explicit des-cbc command)"
echo "════════════════════════════════════════"

# Helper: run ft_ssl des-cbc and capture hex
ft_cbc_hex() {
    "$BIN" des-cbc "$@" 2>/dev/null | xxd -p | tr -d '\n'
}

label="des-cbc: encrypt matches openssl des-cbc"
ft=$(echo -n "$MSG" | ft_cbc_hex -e -k "$KEY" -v "$IV")
ssl=$(echo -n "$MSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="des-cbc: encrypt multi-block matches openssl des-cbc"
ft=$(echo -n "$LONGMSG" | ft_cbc_hex -e -k "$KEY" -v "$IV")
ssl=$(echo -n "$LONGMSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

label="des-cbc: output identical to 'des' (same key+IV)"
ft_plain=$(echo -n "$LONGMSG" | ft_hex -e -k "$KEY" -v "$IV")
ft_cbc=$(echo -n "$LONGMSG" | ft_cbc_hex -e -k "$KEY" -v "$IV")
cmp_hex "$ft_cbc" "$ft_plain" "$label"

label="des-cbc: decrypt roundtrip"
cipher=$(echo -n "$LONGMSG" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
ft=$(printf "%b" "$(echo "$cipher" | sed 's/../\\x&/g')" | ft_cbc_hex -d -k "$KEY" -v "$IV")
exp=$(echo -n "$LONGMSG" | xxd -p | tr -d '\n')
cmp_hex "$ft" "$exp" "$label"

label="des-cbc: base64 roundtrip"
roundtrip=$(echo -n "$LONGMSG" | "$BIN" des-cbc -e -a -k "$KEY" -v "$IV" 2>/dev/null \
    | "$BIN" des-cbc -d -a -k "$KEY" -v "$IV" 2>/dev/null)
[ "$roundtrip" = "$LONGMSG" ] && ok "$label" || { fail "$label"; echo "       got: $roundtrip"; }

label="des-cbc: FIPS-81 vector"
ft=$(echo -n "$FIPS_MSG" | ft_cbc_hex -e -k "$FIPS_KEY" -v "$FIPS_IV")
ssl=$(echo -n "$FIPS_MSG" | ssl_hex -K "$FIPS_KEY" -iv "$FIPS_IV" -nosalt -e)
cmp_hex "$ft" "$ssl" "$label"

# CBC property: identical plaintext blocks → different ciphertext blocks
label="des-cbc: identical plaintext blocks → different ciphertext blocks"
IDENT_CBC=$(head -c 16 /dev/zero | tr '\0' 'A')
cipher=$(printf '%s' "$IDENT_CBC" | ft_cbc_hex -e -k "$KEY" -v "$IV")
c1="${cipher:0:16}"
c2="${cipher:16:16}"
if [ "$c1" != "$c2" ]; then
    ok "$label"
else
    fail "$label"
    echo "       both blocks: $c1  (should differ in CBC)"
fi

# Block boundary sizes for des-cbc vs openssl
for n in $(seq 1 24); do
    msg=$(head -c "$n" /dev/zero | tr '\0' 'C')
    ft=$(printf '%s' "$msg" | ft_cbc_hex -e -k "$KEY" -v "$IV")
    ssl=$(printf '%s' "$msg" | ssl_hex -K "$KEY" -iv "$IV" -nosalt -e)
    cmp_hex "$ft" "$ssl" "des-cbc: encrypt ${n}-byte input vs openssl"
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
printf " Results: ${GRN}%d passed${RST}  ${RED}%d failed${RST}\n" "$PASS" "$FAIL"
echo "════════════════════════════════════════"
echo ""

[ "$FAIL" -eq 0 ] && exit 0 || exit 1

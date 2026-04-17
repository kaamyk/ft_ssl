#!/bin/bash

# Test script for ft_ssl des -P option
# Compares ft_ssl output against openssl des-ecb -P where possible.
#
# What CAN be compared against openssl:
#   - Output format (salt= / key= lines)
#   - Salt value when fixed with -s / -S
#
# What CANNOT be compared (noted inline):
#   - Key value with password: ft_ssl uses PBKDF2, openssl uses EVP_BytesToKey
#   - Key value with -k: ft_ssl does not print key when raw_key is set (known bug)

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

cmp_str() {
    local got="$1" exp="$2" label="$3"
    if [ "$(echo "$got" | tr '[:upper:]' '[:lower:]')" = "$(echo "$exp" | tr '[:upper:]' '[:lower:]')" ]; then
        ok "$label"
    else
        fail "$label"
        echo "       got: $got"
        echo "       exp: $exp"
    fi
}

# Run ft_ssl des -P and return its stdout (stderr silenced)
ft_P() {
    echo -n "data" | "$BIN" des -e -P "$@" 2>/dev/null
}

# Run openssl des-ecb -P and return its stdout (warnings and errors silenced)
# Note: openssl exits 1 after printing params — that is expected, output is still valid
ssl_P() {
    echo -n "data" | openssl enc -provider legacy -provider default -des-ecb -pbkdf2 -P "$@" 2>/dev/null
    return 0
}

# ─────────────────────────────────────────────────────────────────────────────
info "Building..."
make -C "$(dirname "$BIN")" -s 2>/dev/null || { echo "Build failed"; exit 1; }
echo ""

SALT="DEADBEEFCAFEBABE"
PASSWD="password"
KEY="AABBCCDDAABBCCDD"

# ─────────────────────────────────────────────────────────────────────────────
echo "════════════════════════════════════════"
echo " OUTPUT FORMAT  (compared to openssl)"
echo "════════════════════════════════════════"

ft_out=$(ft_P  -p "$PASSWD" -s "$SALT")
ssl_out=$(ssl_P -pass pass:"$PASSWD" -S "$SALT")

# openssl outputs 2 lines for des-ecb (no IV line)
ssl_lines=$(echo "$ssl_out" | wc -l)
ft_lines=$(echo "$ft_out"  | wc -l)
if [ "$ft_lines" -eq "$ssl_lines" ]; then
    ok "line count matches openssl ($ft_lines lines)"
else
    fail "line count matches openssl"
    echo "       ft_ssl:  $ft_lines lines"
    echo "       openssl: $ssl_lines lines"
fi

# Both must have a salt= line
ssl_salt_line=$(echo "$ssl_out" | grep '^salt=')
ft_salt_line=$(echo  "$ft_out"  | grep '^salt=')
if [ -n "$ft_salt_line" ] && [ -n "$ssl_salt_line" ]; then
    ok "both have a salt= line"
else
    fail "both have a salt= line"
    echo "       ft_ssl:  '${ft_salt_line}'"
    echo "       openssl: '${ssl_salt_line}'"
fi

# Both must have a key= line
ssl_key_line=$(echo "$ssl_out" | grep '^key=')
ft_key_line=$(echo  "$ft_out"  | grep '^key=')
if [ -n "$ft_key_line" ] && [ -n "$ssl_key_line" ]; then
    ok "both have a key= line"
else
    fail "both have a key= line"
    echo "       ft_ssl:  '${ft_key_line}'"
    echo "       openssl: '${ssl_key_line}'"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " SALT VALUE  (compared to openssl)"
echo "════════════════════════════════════════"

# When -s / -S is provided, both must report the same salt
for S in "DEADBEEFCAFEBABE" "AABBCCDDEEFF0011" "CAFEBABE12345678"; do
    ft_s=$(ft_P  -p "$PASSWD" -s "$S"        | grep '^salt=' | cut -d= -f2)
    sl_s=$(ssl_P -pass pass:"$PASSWD" -S "$S" | grep '^salt=' | cut -d= -f2)
    cmp_str "$ft_s" "$sl_s" "salt=$S reported correctly vs openssl"
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " KEY VALUE  (cannot compare — KDF differs)"
echo "════════════════════════════════════════"
# openssl: EVP_BytesToKey (MD5-based)   ft_ssl: PBKDF2 (SHA-256, 10000 iterations)
# Keys will differ for same password+salt. Shown for information only.
ft_k=$(ft_P  -p "$PASSWD" -s "$SALT" | grep '^key=' | cut -d= -f2)
sl_k=$(ssl_P -pass pass:"$PASSWD" -S "$SALT" | grep '^key=' | cut -d= -f2)
info "ft_ssl key : $ft_k"
info "openssl key: $sl_k  (EVP_BytesToKey — expected to differ)"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " NO CIPHERTEXT PRODUCED"
echo "════════════════════════════════════════"

out=$(ft_P -p "$PASSWD" -s "$SALT")
extra=$(echo "$out" | grep -v '^salt=' | grep -v '^key=')
if [ -z "$extra" ]; then
    ok "-P produces no ciphertext output"
else
    fail "-P produces no ciphertext output"
    echo "       extra: '$extra'"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DETERMINISM WITH FIXED SALT"
echo "════════════════════════════════════════"

for PASSWD2 in "password" "42isnice" "s3cr3t!"; do
    out1=$(ft_P -p "$PASSWD2" -s "$SALT")
    out2=$(ft_P -p "$PASSWD2" -s "$SALT")
    if [ "$out1" = "$out2" ]; then
        ok "deterministic: password='$PASSWD2' salt=$SALT"
    else
        fail "deterministic: password='$PASSWD2' salt=$SALT"
        echo "       run1: '$out1'"
        echo "       run2: '$out2'"
    fi
done

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " RANDOM SALT WITHOUT -s"
echo "════════════════════════════════════════"

# BUG NOTE: if srand(time(NULL)) is used, runs within the same second collide.
# Sleep 1s to expose the real randomness behaviour.
salt1=$(ft_P -p "$PASSWD" | grep '^salt=' | cut -d= -f2)
sleep 1
salt2=$(ft_P -p "$PASSWD" | grep '^salt=' | cut -d= -f2)
if [ "$salt1" != "$salt2" ]; then
    ok "random salt: consecutive runs differ"
else
    fail "random salt: consecutive runs differ"
    echo "       both gave salt='$salt1'"
    echo "       hint: use arc4random/getrandom instead of srand(time(NULL))"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DIFFERENT PASSWORDS -> DIFFERENT KEYS"
echo "════════════════════════════════════════"

key_a=$(ft_P -p "aaaa" -s "$SALT" | grep '^key=' | cut -d= -f2)
key_b=$(ft_P -p "bbbb" -s "$SALT" | grep '^key=' | cut -d= -f2)
if [ "$key_a" != "$key_b" ]; then
    ok "different passwords produce different keys (same salt)"
else
    fail "different passwords produce different keys"
    echo "       both gave key='$key_a'"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " DIFFERENT SALTS -> DIFFERENT KEYS"
echo "════════════════════════════════════════"

key_s1=$(ft_P -p "samepassword" -s "1111111111111111" | grep '^key=' | cut -d= -f2)
key_s2=$(ft_P -p "samepassword" -s "2222222222222222" | grep '^key=' | cut -d= -f2)
if [ "$key_s1" != "$key_s2" ]; then
    ok "different salts produce different keys (same password)"
else
    fail "different salts produce different keys"
    echo "       both gave key='$key_s1'"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " EDGE CASES — SALT FORMAT"
echo "════════════════════════════════════════"

# Salt with leading zeros must be printed in full (zero-padded to 16 chars)
for S in "000000000000000A" "0000000000000000" "FFFFFFFFFFFFFFFF"; do
    ft_s=$(ft_P  -p "pass" -s "$S"        | grep '^salt=' | cut -d= -f2)
    sl_s=$(openssl enc -provider legacy -provider default -des-ecb \
           -P -pass pass:pass -S "$S" 2>/dev/null | grep '^salt=' | cut -d= -f2)
    cmp_str "$ft_s" "$sl_s" "leading-zero salt preserved: $S"
done

# Short salt (< 16 hex chars) must be zero-padded on the left
ft_s=$(ft_P -p "pass" -s "AABB" | grep '^salt=' | cut -d= -f2)
cmp_str "$ft_s" "000000000000AABB" "short salt zero-padded: AABB -> 000000000000AABB"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " EDGE CASES — PASSWORD FORMAT"
echo "════════════════════════════════════════"

# Single-char password: must produce a valid key line
k=$(ft_P -p "a" -s "$SALT" | grep '^key=')
if echo "$k" | grep -qE '^key=[0-9A-F]{16}$'; then
    ok "single-char password produces valid key"
else
    fail "single-char password produces valid key"
    echo "       got: '$k'"
fi

# Password with spaces
k=$(ft_P -p "my password" -s "$SALT" | grep '^key=')
if echo "$k" | grep -qE '^key=[0-9A-F]{16}$'; then
    ok "password with spaces produces valid key"
else
    fail "password with spaces produces valid key"
    echo "       got: '$k'"
fi

# Password with special characters
k=$(ft_P -p 'p@$$w0rd!' -s "$SALT" | grep '^key=')
if echo "$k" | grep -qE '^key=[0-9A-F]{16}$'; then
    ok "password with special chars produces valid key"
else
    fail "password with special chars produces valid key"
    echo "       got: '$k'"
fi

# Very long password (100 chars)
LONGPASS=$(printf 'A%.0s' {1..100})
k=$(ft_P -p "$LONGPASS" -s "$SALT" | grep '^key=')
if echo "$k" | grep -qE '^key=[0-9A-F]{16}$'; then
    ok "100-char password produces valid key"
else
    fail "100-char password produces valid key"
    echo "       got: '$k'"
fi

# Different password lengths must produce different keys
k_short=$(ft_P -p "a"  -s "$SALT" | grep '^key=' | cut -d= -f2)
k_long=$(ft_P  -p "aa" -s "$SALT" | grep '^key=' | cut -d= -f2)
if [ "$k_short" != "$k_long" ]; then
    ok "password length affects key (\"a\" != \"aa\")"
else
    fail "password length affects key"
    echo "       both gave key='$k_short'"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " EDGE CASES — MODE FLAGS"
echo "════════════════════════════════════════"

# Key derivation is direction-independent: -e and -d must give the same key
k_enc=$(ft_P -e -p "pass" -s "$SALT" | grep '^key=' | cut -d= -f2)
k_dec=$(echo -n "data" | "$BIN" des -d -P -p "pass" -s "$SALT" 2>/dev/null | grep '^key=' | cut -d= -f2)
cmp_str "$k_enc" "$k_dec" "-e and -d produce the same key (KDF is direction-independent)"

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo " EDGE CASES — ERROR HANDLING"
echo "════════════════════════════════════════"

# invalid hex salt must exit with a non-zero code
echo -n "x" | "$BIN" des -e -P -p "pass" -s "ZZZZZZZZZZZZZZZZ" 2>/dev/null
if [ $? -ne 0 ]; then
    ok "invalid hex salt exits non-zero"
else
    fail "invalid hex salt exits non-zero"
fi

# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
TOTAL=$((PASS + FAIL))
echo " Results: ${GRN}${PASS}/${TOTAL} passed${RST}  ${RED}${FAIL} failed${RST}"
echo "════════════════════════════════════════"
echo ""

[ "$FAIL" -eq 0 ]

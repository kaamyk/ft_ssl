#!/bin/bash

# Comprehensive test suite for ft_ssl
# Usage: bash test_ft_ssl.sh

set -o pipefail

FT_SSL="./ft_ssl"
OPENSSL="openssl"
OPENSSL_DES_FLAGS="-provider legacy -provider default -pbkdf2"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Counters
TOTAL=0
PASSED=0
FAILED=0
SKIPPED=0

# Temp directory
TMPDIR=$(mktemp -d /tmp/ft_ssl_test.XXXXXX)
trap 'rm -rf "$TMPDIR"' EXIT

# Valgrind
HAS_VALGRIND=0
if command -v valgrind &>/dev/null; then
    HAS_VALGRIND=1
fi

pass() {
    ((TOTAL++))
    ((PASSED++))
    echo -e "  ${GREEN}PASS${NC} $1"
}

fail() {
    ((TOTAL++))
    ((FAILED++))
    echo -e "  ${RED}FAIL${NC} $1"
    if [ -n "$2" ]; then
        echo -e "       ${RED}Command: $2${NC}"
    fi
    if [ -n "$3" ]; then
        echo -e "       ${RED}Detail: $3${NC}"
    fi
}

skip() {
    ((TOTAL++))
    ((SKIPPED++))
    echo -e "  ${YELLOW}SKIP${NC} $1"
}

section() {
    echo ""
    echo -e "${CYAN}${BOLD}=== $1 ===${NC}"
}

# Compare two values; pass if equal
assert_eq() {
    local desc="$1" got="$2" expected="$3" cmd="$4"
    if [ "$got" = "$expected" ]; then
        pass "$desc"
    else
        fail "$desc" "$cmd" "expected='$expected' got='$got'"
    fi
}

# Compare two files; pass if identical
assert_files_eq() {
    local desc="$1" file_a="$2" file_b="$3" cmd="$4"
    if cmp -s "$file_a" "$file_b"; then
        pass "$desc"
    else
        fail "$desc" "$cmd" "files differ"
    fi
}

# Check that a command exits with non-zero
assert_fail() {
    local desc="$1"
    shift
    local cmd="$*"
    eval "$cmd" >/dev/null 2>&1
    local rc=$?
    if [ $rc -ne 0 ] && [ $rc -ne 139 ] && [ $rc -ne 134 ] && [ $rc -ne 135 ]; then
        pass "$desc"
    else
        fail "$desc" "$cmd" "expected non-zero exit, got $rc"
    fi
}

# Check command does not crash (exit code < 128 or == 0 or == 1)
assert_no_crash() {
    local desc="$1"
    shift
    local cmd="$*"
    eval "$cmd" >/dev/null 2>&1
    local rc=$?
    if [ $rc -lt 128 ]; then
        pass "$desc"
    else
        local sig=$((rc - 128))
        fail "$desc" "$cmd" "crashed with signal $sig (exit code $rc)"
    fi
}

# Run valgrind on a command, check for leaks
valgrind_check() {
    local desc="$1"
    shift
    local cmd="$*"
    if [ $HAS_VALGRIND -eq 0 ]; then
        skip "$desc (valgrind not available)"
        return
    fi
    local vg_log="$TMPDIR/vg_$RANDOM.log"
    eval "valgrind --leak-check=full --error-exitcode=42 --log-file='$vg_log' $cmd" >/dev/null 2>&1
    local rc=$?
    if [ $rc -eq 42 ]; then
        fail "$desc" "$cmd" "valgrind detected errors (see $vg_log)"
    elif [ $rc -ge 128 ]; then
        local sig=$((rc - 128))
        fail "$desc" "$cmd" "crashed with signal $sig under valgrind"
    else
        pass "$desc"
    fi
}

########################################################################
# 1. BASE64 TESTS
########################################################################
section "Base64 - Encoding"

# Simple encode
got=$(echo "toto" | $FT_SSL base64 -e)
expected=$(echo "toto" | $OPENSSL base64 -e)
assert_eq "encode 'toto'" "$got" "$expected" "echo 'toto' | $FT_SSL base64 -e"

# Default mode is encode (no -e)
got=$(echo "toto" | $FT_SSL base64)
expected=$(echo "toto" | $OPENSSL base64 -e)
assert_eq "default mode is encode" "$got" "$expected" "echo 'toto' | $FT_SSL base64"

# Encode empty input
got=$(echo -n "" | $FT_SSL base64 -e)
expected=$(echo -n "" | $OPENSSL base64 -e)
assert_eq "encode empty input" "$got" "$expected" "echo -n '' | $FT_SSL base64 -e"

# Encode longer string
got=$(echo "Hello, World! This is a longer test string." | $FT_SSL base64 -e)
expected=$(echo "Hello, World! This is a longer test string." | $OPENSSL base64 -e)
assert_eq "encode longer string" "$got" "$expected"

# Encode binary data
dd if=/dev/urandom bs=256 count=1 of="$TMPDIR/bin_input" 2>/dev/null
$FT_SSL base64 -e < "$TMPDIR/bin_input" > "$TMPDIR/b64_ft"
$OPENSSL base64 -e < "$TMPDIR/bin_input" > "$TMPDIR/b64_ssl"
assert_files_eq "encode binary data (256 bytes)" "$TMPDIR/b64_ft" "$TMPDIR/b64_ssl"

section "Base64 - Decoding"

# Simple decode
got=$(echo "dG90bwo=" | $FT_SSL base64 -d)
assert_eq "decode 'dG90bwo='" "$got" "toto" "echo 'dG90bwo=' | $FT_SSL base64 -d"

# Decode with whitespace
got=$(echo "d G9 0bwo =" | $FT_SSL base64 -d)
assert_eq "decode with whitespace" "$got" "toto" "echo 'd G9 0bwo =' | $FT_SSL base64 -d"

# Decode empty input
got=$(echo -n "" | $FT_SSL base64 -d 2>/dev/null)
expected=$(echo -n "" | $OPENSSL base64 -d 2>/dev/null)
assert_eq "decode empty input" "$got" "$expected"

section "Base64 - Round-trip"

# Round-trip text
echo "Round trip test 12345!@#" > "$TMPDIR/rt_input"
$FT_SSL base64 -e < "$TMPDIR/rt_input" | $FT_SSL base64 -d > "$TMPDIR/rt_output"
assert_files_eq "round-trip text" "$TMPDIR/rt_input" "$TMPDIR/rt_output"

# Round-trip binary
dd if=/dev/urandom bs=1024 count=10 of="$TMPDIR/rt_bin_input" 2>/dev/null
$FT_SSL base64 -e < "$TMPDIR/rt_bin_input" | $FT_SSL base64 -d > "$TMPDIR/rt_bin_output"
assert_files_eq "round-trip large binary (10KB)" "$TMPDIR/rt_bin_input" "$TMPDIR/rt_bin_output"

# Cross round-trip: ft_ssl encode -> openssl decode
dd if=/dev/urandom bs=512 count=1 of="$TMPDIR/cross_input" 2>/dev/null
$FT_SSL base64 -e < "$TMPDIR/cross_input" | $OPENSSL base64 -d > "$TMPDIR/cross_output"
assert_files_eq "cross round-trip: ft_ssl encode -> openssl decode" "$TMPDIR/cross_input" "$TMPDIR/cross_output"

# Cross round-trip: openssl encode -> ft_ssl decode
$OPENSSL base64 -e < "$TMPDIR/cross_input" | $FT_SSL base64 -d > "$TMPDIR/cross_output2"
assert_files_eq "cross round-trip: openssl encode -> ft_ssl decode" "$TMPDIR/cross_input" "$TMPDIR/cross_output2"

section "Base64 - File flags"

echo "file flag test content" > "$TMPDIR/b64_file_in"

# -i flag
$FT_SSL base64 -e -i "$TMPDIR/b64_file_in" > "$TMPDIR/b64_file_enc"
$OPENSSL base64 -e -in "$TMPDIR/b64_file_in" > "$TMPDIR/b64_file_enc_ssl"
assert_files_eq "encode with -i flag" "$TMPDIR/b64_file_enc" "$TMPDIR/b64_file_enc_ssl"

# -o flag
$FT_SSL base64 -e -i "$TMPDIR/b64_file_in" -o "$TMPDIR/b64_file_out"
assert_files_eq "encode with -i and -o flags" "$TMPDIR/b64_file_out" "$TMPDIR/b64_file_enc_ssl"

# -o decode
$FT_SSL base64 -d -i "$TMPDIR/b64_file_enc" -o "$TMPDIR/b64_file_dec"
assert_files_eq "decode with -i and -o flags" "$TMPDIR/b64_file_dec" "$TMPDIR/b64_file_in"

########################################################################
# 2. DES-ECB TESTS
########################################################################
section "DES-ECB - Basic encryption"

# Subject test: echo "foo bar" with key 6162636461626364, -a -nopad equivalent
# We test with openssl -nopad to verify raw block alignment
got=$(echo -n "foo bar" | $OPENSSL des-ecb -K 6162636461626364 -a -nopad $OPENSSL_DES_FLAGS 2>/dev/null)
assert_eq "openssl subject test (nopad)" "$got" "YZF3QKaabXU=" "echo -n 'foo bar' | openssl des-ecb -K 6162636461626364 -a -nopad ..."

# Encrypt with ft_ssl, decrypt with openssl (PKCS padding)
echo -n "foo bar" | $FT_SSL des-ecb -k 6162636461626364 -a > "$TMPDIR/ecb_enc"
got=$($OPENSSL des-ecb -d -K 6162636461626364 -a $OPENSSL_DES_FLAGS < "$TMPDIR/ecb_enc" 2>/dev/null)
assert_eq "ft_ssl encrypt -> openssl decrypt (ecb, PKCS)" "$got" "foo bar"

# Encrypt with openssl, decrypt with ft_ssl
echo -n "test data!" | $OPENSSL des-ecb -K 6162636461626364 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_ssl_enc" 2>/dev/null
got=$($FT_SSL des-ecb -d -k 6162636461626364 -a < "$TMPDIR/ecb_ssl_enc")
assert_eq "openssl encrypt -> ft_ssl decrypt (ecb)" "$got" "test data!"

section "DES-ECB - Key handling"

# Short key padding: FF12CD -> FF12CD0000000000
echo -n "key test" | $FT_SSL des-ecb -k FF12CD -a > "$TMPDIR/ecb_short_ft"
echo -n "key test" | $OPENSSL des-ecb -K FF12CD0000000000 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_short_ssl" 2>/dev/null
assert_files_eq "short key padding (FF12CD)" "$TMPDIR/ecb_short_ft" "$TMPDIR/ecb_short_ssl"

# Very short key: FF1 -> FF10000000000000
echo -n "key test" | $FT_SSL des-ecb -k FF1 -a > "$TMPDIR/ecb_vshort_ft"
echo -n "key test" | $OPENSSL des-ecb -K FF10000000000000 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_vshort_ssl" 2>/dev/null
assert_files_eq "very short key padding (FF1)" "$TMPDIR/ecb_vshort_ft" "$TMPDIR/ecb_vshort_ssl"

# Long key truncation: key longer than 16 hex -> truncated to first 16
echo -n "key test" | $FT_SSL des-ecb -k 6162636461626364AABBCCDD -a > "$TMPDIR/ecb_long_ft"
echo -n "key test" | $OPENSSL des-ecb -K 6162636461626364 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_long_ssl" 2>/dev/null
assert_files_eq "long key truncation" "$TMPDIR/ecb_long_ft" "$TMPDIR/ecb_long_ssl"

section "DES-ECB - Various inputs"

# Empty input (PKCS padding should produce 8 bytes of padding)
echo -n "" | $FT_SSL des-ecb -k 6162636461626364 -a > "$TMPDIR/ecb_empty_ft"
echo -n "" | $OPENSSL des-ecb -K 6162636461626364 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_empty_ssl" 2>/dev/null
assert_files_eq "empty input (PKCS padding)" "$TMPDIR/ecb_empty_ft" "$TMPDIR/ecb_empty_ssl"

# Multi-block input (more than 8 bytes)
echo -n "This is a multi-block input for DES-ECB testing!" | $FT_SSL des-ecb -k AABBCCDDEEFF0011 -a > "$TMPDIR/ecb_multi_ft"
echo -n "This is a multi-block input for DES-ECB testing!" | $OPENSSL des-ecb -K AABBCCDDEEFF0011 -a $OPENSSL_DES_FLAGS > "$TMPDIR/ecb_multi_ssl" 2>/dev/null
assert_files_eq "multi-block input" "$TMPDIR/ecb_multi_ft" "$TMPDIR/ecb_multi_ssl"

# Round-trip with ft_ssl only
echo -n "ecb round-trip" | $FT_SSL des-ecb -k 1122334455667788 -a | $FT_SSL des-ecb -d -k 1122334455667788 -a > "$TMPDIR/ecb_rt"
assert_eq "round-trip ft_ssl only (ecb)" "$(cat "$TMPDIR/ecb_rt")" "ecb round-trip"

# Large input round-trip
dd if=/dev/urandom bs=1024 count=64 of="$TMPDIR/ecb_large_in" 2>/dev/null
$FT_SSL des-ecb -k AABBCCDDEEFF0011 < "$TMPDIR/ecb_large_in" > "$TMPDIR/ecb_large_enc"
$FT_SSL des-ecb -d -k AABBCCDDEEFF0011 < "$TMPDIR/ecb_large_enc" > "$TMPDIR/ecb_large_dec"
assert_files_eq "large input round-trip (64KB, ecb)" "$TMPDIR/ecb_large_in" "$TMPDIR/ecb_large_dec"

# Large input cross-compat
$FT_SSL des-ecb -k AABBCCDDEEFF0011 < "$TMPDIR/ecb_large_in" > "$TMPDIR/ecb_large_ft_enc"
$OPENSSL des-ecb -d -K AABBCCDDEEFF0011 $OPENSSL_DES_FLAGS < "$TMPDIR/ecb_large_ft_enc" > "$TMPDIR/ecb_large_ssl_dec" 2>/dev/null
assert_files_eq "large input: ft_ssl encrypt -> openssl decrypt (ecb)" "$TMPDIR/ecb_large_in" "$TMPDIR/ecb_large_ssl_dec"

########################################################################
# 3. DES-CBC TESTS
########################################################################
section "DES-CBC - Basic encryption"

# Subject test
got=$(echo "one deep secret" | $FT_SSL des-cbc -a -k 6162636461626364 -v 0011223344556677)
assert_eq "subject test (des-cbc)" "$got" "zqYWONX68rWNxl7msIdGC67Uh2HfVEBo"

# Encrypt with ft_ssl, decrypt with openssl
echo -n "cbc cross test" | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a > "$TMPDIR/cbc_enc"
got=$($OPENSSL des-cbc -d -K 6162636461626364 -iv 0011223344556677 -a $OPENSSL_DES_FLAGS < "$TMPDIR/cbc_enc" 2>/dev/null)
assert_eq "ft_ssl encrypt -> openssl decrypt (cbc)" "$got" "cbc cross test"

# Encrypt with openssl, decrypt with ft_ssl
echo -n "cbc reverse" | $OPENSSL des-cbc -K 6162636461626364 -iv 0011223344556677 -a $OPENSSL_DES_FLAGS > "$TMPDIR/cbc_ssl_enc" 2>/dev/null
got=$($FT_SSL des-cbc -d -k 6162636461626364 -v 0011223344556677 -a < "$TMPDIR/cbc_ssl_enc")
assert_eq "openssl encrypt -> ft_ssl decrypt (cbc)" "$got" "cbc reverse"

section "DES-CBC - IV tests"

# Different IVs produce different ciphertext
echo -n "iv test data" | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a > "$TMPDIR/cbc_iv1"
echo -n "iv test data" | $FT_SSL des-cbc -k 6162636461626364 -v AABBCCDDEEFF0011 -a > "$TMPDIR/cbc_iv2"
if ! cmp -s "$TMPDIR/cbc_iv1" "$TMPDIR/cbc_iv2"; then
    pass "different IVs produce different ciphertext"
else
    fail "different IVs produce different ciphertext" "" "outputs are identical"
fi

# IV of all zeros
echo -n "zero iv test" | $FT_SSL des-cbc -k 6162636461626364 -v 0000000000000000 -a > "$TMPDIR/cbc_ziv_ft"
echo -n "zero iv test" | $OPENSSL des-cbc -K 6162636461626364 -iv 0000000000000000 -a $OPENSSL_DES_FLAGS > "$TMPDIR/cbc_ziv_ssl" 2>/dev/null
assert_files_eq "IV of all zeros" "$TMPDIR/cbc_ziv_ft" "$TMPDIR/cbc_ziv_ssl"

section "DES-CBC - Various inputs"

# Round-trip various sizes
for size in 1 7 8 9 15 16 17 100 1000; do
    dd if=/dev/urandom bs=1 count=$size of="$TMPDIR/cbc_rt_${size}" 2>/dev/null
    $FT_SSL des-cbc -k AABBCCDDEEFF0011 -v 1122334455667788 < "$TMPDIR/cbc_rt_${size}" | \
        $FT_SSL des-cbc -d -k AABBCCDDEEFF0011 -v 1122334455667788 > "$TMPDIR/cbc_rt_${size}_dec"
    assert_files_eq "round-trip ${size} bytes (cbc)" "$TMPDIR/cbc_rt_${size}" "$TMPDIR/cbc_rt_${size}_dec"
done

# Large input round-trip (cbc)
dd if=/dev/urandom bs=1024 count=64 of="$TMPDIR/cbc_large_in" 2>/dev/null
$FT_SSL des-cbc -k AABBCCDDEEFF0011 -v 1122334455667788 < "$TMPDIR/cbc_large_in" > "$TMPDIR/cbc_large_enc"
$FT_SSL des-cbc -d -k AABBCCDDEEFF0011 -v 1122334455667788 < "$TMPDIR/cbc_large_enc" > "$TMPDIR/cbc_large_dec"
assert_files_eq "large input round-trip (64KB, cbc)" "$TMPDIR/cbc_large_in" "$TMPDIR/cbc_large_dec"

########################################################################
# 4. DES ALIAS TEST
########################################################################
section "DES alias (des == des-cbc)"

# Same encryption output
echo -n "alias test data" | $FT_SSL des -k 6162636461626364 -v 0011223344556677 -a > "$TMPDIR/des_alias"
echo -n "alias test data" | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a > "$TMPDIR/des_cbc_ref"
assert_files_eq "des produces same output as des-cbc" "$TMPDIR/des_alias" "$TMPDIR/des_cbc_ref"

# Encrypt with des, decrypt with des-cbc
echo -n "alias cross" | $FT_SSL des -k AABBCCDDEEFF0011 -v 1122334455667788 -a > "$TMPDIR/des_alias_enc"
got=$($FT_SSL des-cbc -d -k AABBCCDDEEFF0011 -v 1122334455667788 -a < "$TMPDIR/des_alias_enc")
assert_eq "encrypt with des, decrypt with des-cbc" "$got" "alias cross"

# Cross-compat with openssl des alias
echo -n "openssl alias" | $FT_SSL des -k 6162636461626364 -v 0011223344556677 -a > "$TMPDIR/des_alias_ft"
got=$($OPENSSL des -d -K 6162636461626364 -iv 0011223344556677 -a $OPENSSL_DES_FLAGS < "$TMPDIR/des_alias_ft" 2>/dev/null)
assert_eq "ft_ssl des -> openssl des decrypt" "$got" "openssl alias"

########################################################################
# 5. PASSWORD-BASED ENCRYPTION (PBKDF2)
########################################################################
section "Password-based encryption (PBKDF2)"

# Encrypt with password + fixed salt, compare ft_ssl vs openssl
ft_pw=$(echo "test" | $FT_SSL des-cbc -p "password" -s "AABBCCDD11223344" -a)
ssl_pw=$(echo "test" | $OPENSSL des-cbc -pass pass:password -S AABBCCDD11223344 -a $OPENSSL_DES_FLAGS 2>/dev/null)
assert_eq "password encrypt: ft_ssl vs openssl (des-cbc)" "$ft_pw" "$ssl_pw" \
    "echo 'test' | ./ft_ssl des-cbc -p 'password' -s 'AABBCCDD11223344' -a"

# Decrypt openssl output with ft_ssl
echo "decrypt me" | $OPENSSL des-cbc -pass pass:secret -S 0011223344556677 -a $OPENSSL_DES_FLAGS > "$TMPDIR/pw_ssl_enc" 2>/dev/null
got=$($FT_SSL des-cbc -d -p "secret" -s "0011223344556677" -a < "$TMPDIR/pw_ssl_enc")
assert_eq "decrypt openssl password output with ft_ssl" "$got" "decrypt me"

# Decrypt ft_ssl output with openssl
echo "reverse pw" | $FT_SSL des-cbc -p "mypass" -s "AABB112233445566" -a > "$TMPDIR/pw_ft_enc"
got=$($OPENSSL des-cbc -d -pass pass:mypass -S AABB112233445566 -a $OPENSSL_DES_FLAGS < "$TMPDIR/pw_ft_enc" 2>/dev/null)
assert_eq "decrypt ft_ssl password output with openssl" "$got" "reverse pw"

# Salted__ header check (raw binary, no -a)
echo "salt header" | $FT_SSL des-cbc -p "password" -s "AABBCCDD11223344" > "$TMPDIR/pw_raw"
header=$(head -c 8 "$TMPDIR/pw_raw")
assert_eq "Salted__ header present" "$header" "Salted__" "head -c 8 of encrypted output"

# Salt bytes match
salt_hex=$(xxd -p -l 8 -s 8 "$TMPDIR/pw_raw" | tr 'a-f' 'A-F')
assert_eq "salt bytes match specified salt" "$salt_hex" "AABBCCDD11223344"

# Decrypt with password, no explicit salt (read from Salted__ header)
echo "auto salt" | $FT_SSL des-cbc -p "password" -s "1122334455667788" > "$TMPDIR/pw_autosalt_enc"
got=$($FT_SSL des-cbc -d -p "password" < "$TMPDIR/pw_autosalt_enc")
assert_eq "decrypt with auto-salt from Salted__ header" "$got" "auto salt"

# Round-trip with password only (random salt)
echo "random salt rt" | $FT_SSL des-cbc -p "randompass" > "$TMPDIR/pw_randsalt_enc"
got=$($FT_SSL des-cbc -d -p "randompass" < "$TMPDIR/pw_randsalt_enc")
assert_eq "round-trip with random salt" "$got" "random salt rt"

# Password-based ECB
ft_pw_ecb=$(echo "ecb pw test" | $FT_SSL des-ecb -p "password" -s "AABBCCDD11223344" -a)
ssl_pw_ecb=$(echo "ecb pw test" | $OPENSSL des-ecb -pass pass:password -S AABBCCDD11223344 -a $OPENSSL_DES_FLAGS 2>/dev/null)
assert_eq "password encrypt: ft_ssl vs openssl (des-ecb)" "$ft_pw_ecb" "$ssl_pw_ecb"

########################################################################
# 6. ERROR HANDLING / EDGE CASES
########################################################################
section "Error handling"

# Invalid command
assert_fail "invalid command 'foobar'" "$FT_SSL foobar"

# Check that error message lists available commands
err_out=$($FT_SSL foobar 2>&1)
if echo "$err_out" | grep -qi "base64\|des\|command"; then
    pass "invalid command shows help/command list"
else
    fail "invalid command shows help/command list" "$FT_SSL foobar" "output: $err_out"
fi

# Missing key argument
assert_fail "missing key value after -k" "echo test | $FT_SSL des-ecb -k"

# Invalid hex key
assert_fail "invalid hex key (ZZZZ)" "echo test | $FT_SSL des-ecb -k ZZZZ"

# Missing IV for CBC (should error or use zero IV — just must not crash)
assert_no_crash "missing IV for CBC (no crash)" "echo test | $FT_SSL des-cbc -k 6162636461626364"

# Non-existent input file
assert_fail "non-existent input file" "$FT_SSL base64 -i nonexistent_file_xyz"

# Permission denied output file
touch "$TMPDIR/readonly"
chmod 000 "$TMPDIR/readonly"
assert_fail "permission denied on output file" "echo test | $FT_SSL base64 -o $TMPDIR/readonly"
chmod 644 "$TMPDIR/readonly"

########################################################################
# 7. CRASH AND LEAK TESTS
########################################################################
section "Crash resistance"

# No segfault on various operations
assert_no_crash "base64 encode (no crash)" "echo 'hello' | $FT_SSL base64 -e"
assert_no_crash "base64 decode (no crash)" "echo 'aGVsbG8K' | $FT_SSL base64 -d"
assert_no_crash "des-ecb encrypt (no crash)" "echo 'test' | $FT_SSL des-ecb -k 6162636461626364 -a"
assert_no_crash "des-cbc encrypt (no crash)" "echo 'test' | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a"

# Pipe to /dev/null
echo "test" | $FT_SSL base64 > /dev/null 2>&1; rc=$?
if [ $rc -lt 128 ]; then pass "pipe to /dev/null (base64)"; else fail "pipe to /dev/null (base64)" "" "exit $rc"; fi

echo "test" | $FT_SSL des-ecb -k 6162636461626364 > /dev/null 2>&1; rc=$?
if [ $rc -lt 128 ]; then pass "pipe to /dev/null (des-ecb)"; else fail "pipe to /dev/null (des-ecb)" "" "exit $rc"; fi

# Very large input (1MB)
dd if=/dev/urandom bs=1024 count=1024 of="$TMPDIR/large_1mb" 2>/dev/null
assert_no_crash "1MB base64 encode (no crash)" "$FT_SSL base64 -e < $TMPDIR/large_1mb > /dev/null"
assert_no_crash "1MB des-ecb encrypt (no crash)" "$FT_SSL des-ecb -k 6162636461626364 < $TMPDIR/large_1mb > /dev/null"
assert_no_crash "1MB des-cbc encrypt (no crash)" "$FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 < $TMPDIR/large_1mb > /dev/null"

# 1MB round-trip correctness
$FT_SSL des-cbc -k AABBCCDDEEFF0011 -v 1122334455667788 < "$TMPDIR/large_1mb" | \
    $FT_SSL des-cbc -d -k AABBCCDDEEFF0011 -v 1122334455667788 > "$TMPDIR/large_1mb_dec"
assert_files_eq "1MB round-trip correctness (cbc)" "$TMPDIR/large_1mb" "$TMPDIR/large_1mb_dec"

# Rapidly repeated encrypt/decrypt cycles
ok=1
for i in $(seq 1 50); do
    echo -n "cycle $i" | $FT_SSL des-ecb -k 6162636461626364 | $FT_SSL des-ecb -d -k 6162636461626364 > "$TMPDIR/cycle_out" 2>/dev/null
    got=$(cat "$TMPDIR/cycle_out")
    if [ "$got" != "cycle $i" ]; then
        ok=0
        break
    fi
done
if [ $ok -eq 1 ]; then pass "50 rapid encrypt/decrypt cycles"; else fail "50 rapid encrypt/decrypt cycles" "" "failed at cycle $i"; fi

section "Fuzz-like tests (should not crash)"

# Random binary as base64 decode input
dd if=/dev/urandom bs=64 count=1 2>/dev/null | $FT_SSL base64 -d > /dev/null 2>&1
rc=$?
if [ $rc -lt 128 ]; then pass "random binary as base64 decode input"; else fail "random binary as base64 decode input" "" "exit $rc"; fi

# Truncated ciphertext
echo -n "AB" | xxd -r -p | $FT_SSL des-ecb -d -k 6162636461626364 > /dev/null 2>&1
rc=$?
if [ $rc -lt 128 ]; then pass "truncated ciphertext (des-ecb decrypt)"; else fail "truncated ciphertext (des-ecb decrypt)" "" "exit $rc"; fi

echo -n "AB" | xxd -r -p | $FT_SSL des-cbc -d -k 6162636461626364 -v 0011223344556677 > /dev/null 2>&1
rc=$?
if [ $rc -lt 128 ]; then pass "truncated ciphertext (des-cbc decrypt)"; else fail "truncated ciphertext (des-cbc decrypt)" "" "exit $rc"; fi

# Garbage data as DES ciphertext
dd if=/dev/urandom bs=64 count=1 2>/dev/null | $FT_SSL des-ecb -d -k 6162636461626364 > /dev/null 2>&1
rc=$?
if [ $rc -lt 128 ]; then pass "garbage ciphertext (des-ecb decrypt)"; else fail "garbage ciphertext (des-ecb decrypt)" "" "exit $rc"; fi

dd if=/dev/urandom bs=64 count=1 2>/dev/null | $FT_SSL des-cbc -d -k 6162636461626364 -v 0011223344556677 > /dev/null 2>&1
rc=$?
if [ $rc -lt 128 ]; then pass "garbage ciphertext (des-cbc decrypt)"; else fail "garbage ciphertext (des-cbc decrypt)" "" "exit $rc"; fi

section "Valgrind leak checks"

valgrind_check "base64 encode (leaks)" "echo 'hello' | $FT_SSL base64 -e > /dev/null"
valgrind_check "base64 decode (leaks)" "echo 'aGVsbG8K' | $FT_SSL base64 -d > /dev/null"
valgrind_check "des-ecb encrypt (leaks)" "echo 'test' | $FT_SSL des-ecb -k 6162636461626364 -a > /dev/null"
valgrind_check "des-ecb decrypt (leaks)" "echo 'test' | $FT_SSL des-ecb -k 6162636461626364 -a | $FT_SSL des-ecb -d -k 6162636461626364 -a > /dev/null"
valgrind_check "des-cbc encrypt (leaks)" "echo 'test' | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a > /dev/null"
valgrind_check "des-cbc decrypt (leaks)" "echo 'test' | $FT_SSL des-cbc -k 6162636461626364 -v 0011223344556677 -a | $FT_SSL des-cbc -d -k 6162636461626364 -v 0011223344556677 -a > /dev/null"
valgrind_check "password encrypt (leaks)" "echo 'test' | $FT_SSL des-cbc -p password -s AABBCCDD11223344 > /dev/null"
valgrind_check "1MB encrypt (leaks)" "$FT_SSL des-ecb -k 6162636461626364 < $TMPDIR/large_1mb > /dev/null"

########################################################################
# SUMMARY
########################################################################
echo ""
echo -e "${BOLD}========================================${NC}"
echo -e "${BOLD}  Test Summary${NC}"
echo -e "${BOLD}========================================${NC}"
echo -e "  Total:   $TOTAL"
echo -e "  ${GREEN}Passed:  $PASSED${NC}"
echo -e "  ${RED}Failed:  $FAILED${NC}"
echo -e "  ${YELLOW}Skipped: $SKIPPED${NC}"
echo -e "${BOLD}========================================${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}${BOLD}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}${BOLD}Some tests failed.${NC}"
    exit 1
fi

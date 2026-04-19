#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$ROOT_DIR/build/cvm"

if [[ ! -x "$BIN" ]]; then
  echo "Missing binary: $BIN"
  echo "Build first with: cmake -S . -B build && cmake --build build -j\$(nproc)"
  exit 1
fi

status=0

run_and_check() {
  local script="$1"
  local expected="$2"
  local output

  if [[ "$script" == "tests/test_input.cvm" ]]; then
    output="$(printf '7\n' | "$BIN" "$ROOT_DIR/$script")"
  else
    output="$("$BIN" "$ROOT_DIR/$script")"
  fi

  if [[ "$output" == "$expected" ]]; then
    echo "[PASS] $script"
  else
    echo "[FAIL] $script"
    echo "Expected:"
    printf '%s\n' "$expected"
    echo "Got:"
    printf '%s\n' "$output"
    status=1
  fi
}

run_and_check "tests/test_arithmetic.cvm" $'25\n200\n20\n-10\n40'
run_and_check "tests/test_booleans.cvm" $'true\ntrue\nfalse\nfalse\nfalse\ntrue\ntrue\nfalse\ntrue'
run_and_check "tests/test_if_else.cvm" $'0\n42\n3\n15'
run_and_check "tests/test_while.cvm" $'1\n2\n3\n4\n5\n55\n120'
run_and_check "tests/test_fizzbuzz.cvm" $'1\n2\n3\n4\n5\n3\n7\n8\n3\n5\n11\n3\n13\n14\n15\n16\n17\n3\n19\n5'
run_and_check "tests/test_input.cvm" $'7\n14'

if [[ $status -eq 0 ]]; then
  echo "All tests passed."
else
  echo "One or more tests failed."
fi

exit "$status"

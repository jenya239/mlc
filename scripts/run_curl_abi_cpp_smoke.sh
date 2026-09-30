#!/usr/bin/env bash
# HTTPS client STEP=3: curl ABI against a local TLS server. No network beyond
# 127.0.0.1. SKIP when libcurl headers are missing unless HTTPS_CLIENT_REQUIRE=1.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCE="$ROOT_DIR/runtime/test/test_curl_abi.cpp"
DIRECTORY="${HTTPS_CURL_ABI_DIR:-$ROOT_DIR/tmp/https_curl_abi}"
CXX="${CXX:-c++}"

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[curl abi] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[curl abi] SKIP: libcurl headers missing" >&2
  exit 0
fi

SERVER_PID=""
cleanup() {
  if [[ -n "$SERVER_PID" ]]; then
    kill "$SERVER_PID" >/dev/null 2>&1 || true
    wait "$SERVER_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

rm -rf "$DIRECTORY"
mkdir -p "$DIRECTORY"

ruby "$ROOT_DIR/scripts/https_test_server.rb" "$DIRECTORY" \
  >"$DIRECTORY/server_stdout.txt" 2>"$DIRECTORY/server_stderr.txt" &
SERVER_PID=$!

ready=0
for _ in $(seq 1 100); do
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/silent_port" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[curl abi] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

compile_and_run() {
  local binary="$1"
  shift
  echo "[curl abi] stage=compile $binary" >&2
  if ! "$CXX" "$@" -o "$binary" "$SOURCE" "${LINK_FLAGS[@]}"; then
    echo "[curl abi] FAIL stage=compile" >&2
    return 1
  fi
  echo "[curl abi] stage=run $binary" >&2
  HTTPS_CURL_ABI_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
    "$binary" >"$binary.stdout" 2>"$binary.stderr"
  if grep -q 'SENTINEL' "$binary.stdout" "$binary.stderr"; then
    echo "[curl abi] FAIL: secret leaked to test output" >&2
    return 1
  fi
}

INCLUDE="$ROOT_DIR/runtime/include"
COMMON_FLAGS=(-std=c++20 -pthread -I"$INCLUDE" -Wall -Wextra)
LINK_FLAGS=($(pkg-config --libs libcurl))

compile_and_run "$DIRECTORY/test_curl_abi" "${COMMON_FLAGS[@]}"

echo "[curl abi] stage=tsan" >&2
TSAN_OPTIONS="${TSAN_OPTIONS:-halt_on_error=1}" \
  compile_and_run "$DIRECTORY/test_curl_abi_tsan" \
  "${COMMON_FLAGS[@]}" -g -O1 -fsanitize=thread

echo "[curl abi] ok"

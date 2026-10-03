#!/usr/bin/env bash
# Retry https_send. Delay math is pure. The live check uses the local TLS
# server: /retry recovers, /retry-single stops after one attempt, /once does
# not retry a 404. request_count must be 4.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_retry_smoke.mlc"
DIRECTORY="${HTTPS_RETRY_OUT:-$ROOT_DIR/tmp/https_retry_smoke}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https retry] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[https retry] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[https retry] SKIP: libcurl headers missing" >&2
  exit 0
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR"
rm -rf "$DIRECTORY"
mkdir -p "$DIRECTORY"

INCLUDE="$ROOT_DIR/runtime/include"
echo "[https retry] stage=sleep_abi" >&2
if ! "$CXX" -std=c++20 -pthread -I"$INCLUDE" -Wall -Wextra \
  -o "$DIRECTORY/test_sleep_abi" "$ROOT_DIR/runtime/test/test_sleep_abi.cpp"; then
  echo "[https retry] FAIL stage=sleep_abi_compile" >&2
  exit 1
fi
set +e
"$DIRECTORY/test_sleep_abi"
sleep_status=$?
set -e
if [[ "$sleep_status" -ne 0 ]]; then
  echo "[https retry] FAIL stage=sleep_abi exit=$sleep_status" >&2
  exit 1
fi

SERVER_PID=""
cleanup() {
  if [[ -n "$SERVER_PID" ]]; then
    kill "$SERVER_PID" >/dev/null 2>&1 || true
    wait "$SERVER_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

ruby "$ROOT_DIR/scripts/https_test_server.rb" "$DIRECTORY" \
  >"$DIRECTORY/server_stdout.txt" 2>"$DIRECTORY/server_stderr.txt" &
SERVER_PID=$!

ready=0
for _ in $(seq 1 100); do
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/good_port" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[https retry] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

OUT_DIR="$DIRECTORY/generated"
mkdir -p "$OUT_DIR"
echo "[https retry] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https retry] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https retry] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https retry] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_retry_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/https_retry_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https retry] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

export HTTPS_TEST_PORT
export HTTPS_TEST_CA
HTTPS_TEST_PORT="$(cat "$DIRECTORY/good_port")"
HTTPS_TEST_CA="$(cat "$DIRECTORY/certificate_authority_path")"

echo "[https retry] stage=run" >&2
set +e
"$DIRECTORY/https_retry_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https retry] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

request_count="$(cat "$DIRECTORY/request_count")"
if [[ "$request_count" != "4" ]]; then
  echo "[https retry] FAIL stage=request_count got=$request_count" >&2
  exit 1
fi

echo "[https retry] ok"

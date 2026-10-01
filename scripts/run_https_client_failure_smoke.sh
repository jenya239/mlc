#!/usr/bin/env bash
# HTTPS client STEP=5: failure kinds against the local TLS server.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_client_failure_smoke.mlc"
DIRECTORY="${HTTPS_FAILURE_OUT:-$ROOT_DIR/tmp/https_client_failure}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https failure] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[https failure] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[https failure] SKIP: libcurl headers missing" >&2
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

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR" "$DIRECTORY"
rm -f "$DIRECTORY/ready" "$DIRECTORY/good_port" "$DIRECTORY/wrong_port" "$DIRECTORY/silent_port"

ruby "$ROOT_DIR/scripts/https_test_server.rb" "$DIRECTORY" \
  >"$DIRECTORY/server_stdout.txt" 2>"$DIRECTORY/server_stderr.txt" &
SERVER_PID=$!

ready=0
for _ in $(seq 1 300); do
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/silent_port" && -f "$DIRECTORY/good_port" && -f "$DIRECTORY/wrong_port" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[https failure] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

OUT_DIR="$DIRECTORY/generated"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[https failure] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https failure] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https failure] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https failure] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_client_failure_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/https_client_failure_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https failure] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

CLOSED_PORT="$(ruby -e 'require "socket"; server = TCPServer.new("127.0.0.1", 0); port = server.addr[1]; server.close; puts port')"

export HTTPS_TEST_PORT HTTPS_TEST_WRONG_PORT HTTPS_TEST_SILENT_PORT HTTPS_TEST_CA
export HTTPS_TEST_CLOSED_PORT HTTPS_TEST_CONNECTION_COUNT
HTTPS_TEST_PORT="$(cat "$DIRECTORY/good_port")"
HTTPS_TEST_WRONG_PORT="$(cat "$DIRECTORY/wrong_port")"
HTTPS_TEST_SILENT_PORT="$(cat "$DIRECTORY/silent_port")"
HTTPS_TEST_CA="$(cat "$DIRECTORY/certificate_authority_path")"
HTTPS_TEST_CLOSED_PORT="$CLOSED_PORT"
HTTPS_TEST_CONNECTION_COUNT="$DIRECTORY/connection_count"

set +e
"$DIRECTORY/https_client_failure_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https failure] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

echo "[https failure] ok"

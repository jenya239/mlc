#!/usr/bin/env bash
# An explicit proxy_url tunnels HTTPS through the fixture CONNECT proxy.
# An empty proxy_url still ignores https_proxy, even when that variable
# points at a closed port.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_proxy_smoke.mlc"
DIRECTORY="${HTTPS_PROXY_OUT:-$ROOT_DIR/tmp/https_proxy_smoke}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https proxy] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[https proxy] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[https proxy] SKIP: libcurl headers missing" >&2
  exit 0
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR"
rm -rf "$DIRECTORY"
mkdir -p "$DIRECTORY"

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
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/proxy_port" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[https proxy] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

OUT_DIR="$DIRECTORY/generated"
mkdir -p "$OUT_DIR"
echo "[https proxy] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https proxy] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https proxy] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https proxy] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_proxy_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/https_proxy_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https proxy] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

export HTTPS_TEST_PORT
export HTTPS_TEST_CA
export HTTPS_TEST_PROXY
HTTPS_TEST_PORT="$(cat "$DIRECTORY/good_port")"
HTTPS_TEST_CA="$(cat "$DIRECTORY/certificate_authority_path")"
HTTPS_TEST_PROXY="http://127.0.0.1:$(cat "$DIRECTORY/proxy_port")"

echo "[https proxy] stage=run" >&2
set +e
https_proxy="http://127.0.0.1:9" "$DIRECTORY/https_proxy_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https proxy] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

proxy_connect_count="$(cat "$DIRECTORY/proxy_connect_count")"
if [[ "$proxy_connect_count" != "1" ]]; then
  echo "[https proxy] FAIL stage=counts proxy_connect=$proxy_connect_count" >&2
  exit 1
fi

echo "[https proxy] ok"

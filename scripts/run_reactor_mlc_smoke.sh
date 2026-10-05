#!/usr/bin/env bash
# Reactor MLC: https_send_async, task_all, task_then, redirects, proxy, and body limit.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
DIRECTORY="${REACTOR_MLC_DIR:-${TMPDIR:-$ROOT_DIR/tmp}/reactor_mlc}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[reactor mlc] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  echo "[reactor mlc] FAIL: libcurl headers or pkg-config missing" >&2
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

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
rm -rf "$DIRECTORY"
mkdir -p "$TMPDIR" "$DIRECTORY"

ruby "$ROOT_DIR/scripts/https_test_server.rb" "$DIRECTORY" \
  >"$DIRECTORY/server_stdout.txt" 2>"$DIRECTORY/server_stderr.txt" &
SERVER_PID=$!

ready=0
for _ in $(seq 1 100); do
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/good_port" && -f "$DIRECTORY/certificate_authority_path" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[reactor mlc] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

export HTTPS_TEST_PORT
export HTTPS_TEST_CA
export HTTPS_TEST_PROXY
HTTPS_TEST_PORT="$(cat "$DIRECTORY/good_port")"
HTTPS_TEST_CA="$(cat "$DIRECTORY/certificate_authority_path")"
HTTPS_TEST_PROXY="http://127.0.0.1:$(cat "$DIRECTORY/proxy_port")"

compile_and_run() {
  local entry="$1"
  local name="$2"
  local out_dir="$DIRECTORY/${name}_generated"
  local binary="$DIRECTORY/$name"
  rm -rf "$out_dir" "$binary"
  mkdir -p "$out_dir"
  echo "[reactor mlc] stage=codegen name=$name" >&2
  if ! "$MLCC" -o "$out_dir" "$entry" >"$out_dir/mlcc_stdout.txt" 2>"$out_dir/mlcc_stderr.txt"; then
    cat "$out_dir/mlcc_stdout.txt" >&2 || true
    cat "$out_dir/mlcc_stderr.txt" >&2 || true
    echo "[reactor mlc] FAIL stage=codegen name=$name" >&2
    exit 1
  fi
  if grep -q 'W-EXTERN-ATTR' "$out_dir/mlcc_stderr.txt"; then
    cat "$out_dir/mlcc_stderr.txt" >&2
    echo "[reactor mlc] FAIL stage=codegen W-EXTERN-ATTR name=$name" >&2
    exit 1
  fi
  if [[ ! -f "$out_dir/mlc_link_libs.txt" ]] || ! grep -qx 'curl' "$out_dir/mlc_link_libs.txt"; then
    echo "[reactor mlc] FAIL stage=link_libs_file missing curl name=$name" >&2
    exit 1
  fi
  echo "[reactor mlc] stage=cpp_link name=$name" >&2
  if ! MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME="$name" \
    "$ROOT_DIR/compiler/build_bin.sh" "$out_dir" "$binary" \
    >"$out_dir/link_stdout.txt" 2>"$out_dir/link_stderr.txt"; then
    cat "$out_dir/link_stderr.txt" >&2 || true
    echo "[reactor mlc] FAIL stage=cpp_link name=$name" >&2
    exit 1
  fi
  echo "[reactor mlc] stage=run name=$name" >&2
  if ! https_proxy="http://127.0.0.1:9" "$binary"; then
    echo "[reactor mlc] FAIL stage=run name=$name" >&2
    exit 1
  fi
}

compile_and_run "$ROOT_DIR/misc/examples/https_send_async_basic.mlc" https_send_async_basic
compile_and_run "$ROOT_DIR/misc/examples/https_send_async_sequential.mlc" https_send_async_sequential
compile_and_run "$ROOT_DIR/misc/examples/https_task_all_smoke.mlc" https_task_all_smoke
compile_and_run "$ROOT_DIR/misc/examples/https_task_then_smoke.mlc" https_task_then_smoke
compile_and_run "$ROOT_DIR/misc/examples/https_redirect_async_smoke.mlc" https_redirect_async_smoke
compile_and_run "$ROOT_DIR/misc/examples/https_limit_async_smoke.mlc" https_limit_async_smoke
compile_and_run "$ROOT_DIR/misc/examples/https_proxy_async_smoke.mlc" https_proxy_async_smoke

proxy_connect_count="$(cat "$DIRECTORY/proxy_connect_count")"
if [[ "$proxy_connect_count" != "1" ]]; then
  echo "[reactor mlc] FAIL stage=proxy_connect_count got=$proxy_connect_count" >&2
  exit 1
fi

echo "[reactor mlc] ok"

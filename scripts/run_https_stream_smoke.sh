#!/usr/bin/env bash
# Stream an HTTPS body as chunks and reassemble server-sent events.
# The C++ check also stops a transfer that would otherwise sit on /silent.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_stream_smoke.mlc"
DIRECTORY="${HTTPS_STREAM_DIR:-$ROOT_DIR/tmp/https_stream_smoke}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https stream] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[https stream] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[https stream] SKIP: libcurl headers missing" >&2
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
  echo "[https stream] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

INCLUDE="$ROOT_DIR/runtime/include"
LINK_FLAGS=($(pkg-config --libs libcurl))
COMMON_FLAGS=(-std=c++20 -pthread -I"$INCLUDE" -Wall -Wextra)

echo "[https stream] stage=cpp" >&2
if ! "$CXX" "${COMMON_FLAGS[@]}" -o "$DIRECTORY/test_curl_stream" \
  "$ROOT_DIR/runtime/test/test_curl_stream.cpp" "${LINK_FLAGS[@]}"; then
  echo "[https stream] FAIL stage=cpp_compile" >&2
  exit 1
fi
export HTTPS_STREAM_DIR="$DIRECTORY"
set +e
"$DIRECTORY/test_curl_stream" >"$DIRECTORY/cpp_stdout.txt" 2>"$DIRECTORY/cpp_stderr.txt"
cpp_status=$?
set -e
if [[ "$cpp_status" -ne 0 ]]; then
  cat "$DIRECTORY/cpp_stderr.txt" >&2
  echo "[https stream] FAIL stage=cpp exit=$cpp_status" >&2
  exit 1
fi

echo "[https stream] stage=tsan" >&2
if ! "$CXX" "${COMMON_FLAGS[@]}" -g -O1 -fsanitize=thread \
  -o "$DIRECTORY/test_curl_stream_tsan" \
  "$ROOT_DIR/runtime/test/test_curl_stream.cpp" "${LINK_FLAGS[@]}"; then
  echo "[https stream] FAIL stage=tsan_compile" >&2
  exit 1
fi
set +e
TSAN_OPTIONS="${TSAN_OPTIONS:-halt_on_error=1}" \
  "$DIRECTORY/test_curl_stream_tsan" >"$DIRECTORY/tsan_stdout.txt" 2>"$DIRECTORY/tsan_stderr.txt"
tsan_status=$?
set -e
if [[ "$tsan_status" -ne 0 ]]; then
  cat "$DIRECTORY/tsan_stderr.txt" >&2
  echo "[https stream] FAIL stage=tsan exit=$tsan_status" >&2
  exit 1
fi

OUT_DIR="$DIRECTORY/generated"
mkdir -p "$OUT_DIR"
echo "[https stream] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https stream] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https stream] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https stream] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_stream_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/https_stream_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https stream] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

export HTTPS_TEST_PORT
export HTTPS_TEST_CA
HTTPS_TEST_PORT="$(cat "$DIRECTORY/good_port")"
HTTPS_TEST_CA="$(cat "$DIRECTORY/certificate_authority_path")"

echo "[https stream] stage=run" >&2
set +e
"$DIRECTORY/https_stream_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https stream] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

echo "[https stream] ok"

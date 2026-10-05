#!/usr/bin/env bash
# Reactor step 2: one HTTPS transfer on the event loop, compared with curl_easy_perform.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCE="$ROOT_DIR/runtime/test/test_reactor_https.cpp"
DIRECTORY="${REACTOR_HTTPS_DIR:-${TMPDIR:-$ROOT_DIR/tmp}/reactor_https}"
CXX="${CXX:-c++}"

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  echo "[reactor https] FAIL: libcurl headers or pkg-config missing" >&2
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

rm -rf "$DIRECTORY"
mkdir -p "$DIRECTORY"

ruby "$ROOT_DIR/scripts/https_test_server.rb" "$DIRECTORY" \
  >"$DIRECTORY/server_stdout.txt" 2>"$DIRECTORY/server_stderr.txt" &
SERVER_PID=$!

ready=0
for _ in $(seq 1 100); do
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/wrong_port" ]]; then
    ready=1
    break
  fi
  if ! kill -0 "$SERVER_PID" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done
if [[ "$ready" -ne 1 ]]; then
  echo "[reactor https] FAIL: test server did not become ready" >&2
  cat "$DIRECTORY/server_stderr.txt" >&2 || true
  exit 1
fi

LINK_FLAGS=($(pkg-config --libs libcurl))
echo "[reactor https] stage=compile" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_https" "$SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor https] FAIL stage=compile" >&2
  exit 1
fi

echo "[reactor https] stage=run" >&2
if ! REACTOR_HTTPS_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
  "$DIRECTORY/test_reactor_https"; then
  echo "[reactor https] FAIL stage=run" >&2
  exit 1
fi

echo "[reactor https] ok"

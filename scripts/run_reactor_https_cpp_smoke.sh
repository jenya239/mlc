#!/usr/bin/env bash
# Reactor HTTPS: one transfer, then two transfers on one thread.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SOURCE="$ROOT_DIR/runtime/test/test_reactor_https.cpp"
PARALLEL_SOURCE="$ROOT_DIR/runtime/test/test_reactor_parallel.cpp"
CANCEL_SOURCE="$ROOT_DIR/runtime/test/test_reactor_cancel.cpp"
LIFECYCLE_SOURCE="$ROOT_DIR/runtime/test/test_reactor_lifecycle.cpp"
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
  if [[ -f "$DIRECTORY/ready" && -f "$DIRECTORY/wrong_port" && -f "$DIRECTORY/silent_port" ]]; then
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

# The first lookup of this name exceeds the 5s transfer timeout. A second
# lookup is a fast resolve failure. Warm the resolver so both paths return code 2.
ruby -e 'require "socket"; begin; Addrinfo.getaddrinfo("mlc-reactor-unresolvable.invalid", 443); rescue StandardError; end'

echo "[reactor https] stage=run" >&2
if ! REACTOR_HTTPS_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
  "$DIRECTORY/test_reactor_https"; then
  echo "[reactor https] FAIL stage=run" >&2
  exit 1
fi

echo "[reactor https] stage=compile_parallel" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_parallel" "$PARALLEL_SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor https] FAIL stage=compile_parallel" >&2
  exit 1
fi

echo "[reactor https] stage=run_parallel" >&2
if ! REACTOR_HTTPS_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
  "$DIRECTORY/test_reactor_parallel"; then
  echo "[reactor https] FAIL stage=run_parallel" >&2
  exit 1
fi

echo "[reactor https] stage=compile_cancel" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_cancel" "$CANCEL_SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor https] FAIL stage=compile_cancel" >&2
  exit 1
fi

echo "[reactor https] stage=run_cancel" >&2
if ! REACTOR_HTTPS_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
  "$DIRECTORY/test_reactor_cancel"; then
  echo "[reactor https] FAIL stage=run_cancel" >&2
  exit 1
fi

echo "[reactor https] stage=compile_lifecycle" >&2
if ! "$CXX" -std=c++20 -pthread -Wall -Wextra -I"$ROOT_DIR/runtime/include" \
  -o "$DIRECTORY/test_reactor_lifecycle" "$LIFECYCLE_SOURCE" "${LINK_FLAGS[@]}"; then
  echo "[reactor https] FAIL stage=compile_lifecycle" >&2
  exit 1
fi

echo "[reactor https] stage=run_lifecycle" >&2
if ! REACTOR_HTTPS_DIR="$DIRECTORY" https_proxy="http://127.0.0.1:9" \
  "$DIRECTORY/test_reactor_lifecycle"; then
  echo "[reactor https] FAIL stage=run_lifecycle" >&2
  exit 1
fi

echo "[reactor https] ok"

#!/usr/bin/env bash
# TRACK_STDLIB_HTTP_MLC STEP=4 — Tcp+spawn HttpServer demo; curl GET / and /health.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
if [[ ! -x "$MLCC" ]]; then
  echo "[http_server curl] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_http_curl}/http_server_curl_gate"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[http_server curl] codegen" >&2
"$MLCC" -o "$OUT_DIR" misc/examples/http_server_curl_demo.mlc

echo "[http_server curl] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=http_server_curl_demo \
  compiler/build_bin.sh "$OUT_DIR" "$OUT_DIR/http_server_curl_demo"

port_file="$OUT_DIR/http_server_port.txt"
rm -f "$port_file"
server_log="$OUT_DIR/server.log"
(
  cd "$OUT_DIR"
  ./http_server_curl_demo
) >"$server_log" 2>&1 &
server_pid=$!

cleanup() {
  kill "$server_pid" 2>/dev/null || true
  wait "$server_pid" 2>/dev/null || true
}
trap cleanup EXIT

port=""
deadline=$((SECONDS + 20))
while (( SECONDS < deadline )); do
  if [[ -f "$port_file" ]] && [[ -s "$port_file" ]]; then
    port="$(tr -d '[:space:]' <"$port_file")"
    break
  fi
  if ! kill -0 "$server_pid" 2>/dev/null; then
    echo "[http_server curl] FAIL: server exited early" >&2
    cat "$server_log" >&2 || true
    exit 1
  fi
  sleep 0.05
done

if [[ -z "$port" ]]; then
  echo "[http_server curl] FAIL: no port file" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[http_server curl] curl → 127.0.0.1:$port" >&2
home_body="$(curl -sS --max-time 5 "http://127.0.0.1:${port}/")"
health_body="$(curl -sS --max-time 5 "http://127.0.0.1:${port}/health")"

if [[ "$home_body" != "home" ]]; then
  echo "[http_server curl] FAIL GET / body='$home_body' expected=home" >&2
  cat "$server_log" >&2 || true
  exit 1
fi
if [[ "$health_body" != "ok" ]]; then
  echo "[http_server curl] FAIL GET /health body='$health_body' expected=ok" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

wait "$server_pid"
server_status=$?
trap - EXIT
if [[ "$server_status" -ne 0 ]]; then
  echo "[http_server curl] FAIL server exit=$server_status" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[http_server curl] ok" >&2

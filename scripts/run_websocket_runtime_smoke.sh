#!/usr/bin/env bash
# TRACK_STDLIB_WEBSOCKET_TO_MLC STEP=5 — MLC echo demo + Ruby WS client.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"
if [[ ! -x "$MLCC" ]]; then
  echo "[websocket runtime] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

OUT_DIR="${TMPDIR:-$ROOT/.tmp_websocket_mlc}/websocket_echo_gate"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[websocket runtime] codegen" >&2
"$MLCC" -o "$OUT_DIR" misc/examples/websocket_echo_demo.mlc

echo "[websocket runtime] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=websocket_echo_demo \
  compiler/build_bin.sh "$OUT_DIR" "$OUT_DIR/websocket_echo_demo"

server_log="$OUT_DIR/server.log"
(
  cd "$OUT_DIR"
  ./websocket_echo_demo
) >"$server_log" 2>&1 &
server_pid=$!

cleanup() {
  kill "$server_pid" 2>/dev/null || true
  wait "$server_pid" 2>/dev/null || true
}
trap cleanup EXIT

port=""
deadline=$((SECONDS + 20))
port_file="$OUT_DIR/websocket_echo_port.txt"
while (( SECONDS < deadline )); do
  if [[ -f "$port_file" ]] && [[ -s "$port_file" ]]; then
    port="$(tr -d '[:space:]' <"$port_file")"
    if [[ "$port" =~ ^[0-9]+$ ]]; then
      break
    fi
  fi
  if ! kill -0 "$server_pid" 2>/dev/null; then
    echo "[websocket runtime] FAIL: server exited early" >&2
    cat "$server_log" >&2 || true
    exit 1
  fi
  sleep 0.05
done

if [[ -z "$port" ]]; then
  echo "[websocket runtime] FAIL: no port" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[websocket runtime] client → 127.0.0.1:$port" >&2
ruby scripts/websocket_echo_client.rb 127.0.0.1 "$port" hello-ws
echo "websocket runtime smoke OK"

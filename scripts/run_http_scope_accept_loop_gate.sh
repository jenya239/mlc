#!/usr/bin/env bash
# TRACK_CONCURRENCY_SPAWN_DETACH STEP=4 — N parallel curls vs scoped accept-loop.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
if [[ ! -x "$MLCC" ]]; then
  echo "[http scope accept] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_http_scope}/http_scope_accept_gate"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

CONNECTION_COUNT=4
HANDLER_SLEEP_MS=250
# Serial would be ~N*sleep; parallel must stay near one sleep.
MAX_PARALLEL_MS=$(( HANDLER_SLEEP_MS * 2 ))

echo "[http scope accept] codegen" >&2
"$MLCC" -o "$OUT_DIR" misc/examples/http_scope_accept_loop_demo.mlc

if ! grep -q 'TaskScope' "$OUT_DIR"/*.cpp; then
  echo "[http scope accept] FAIL: generated C++ missing TaskScope" >&2
  exit 1
fi
if ! grep -q '\.spawn(' "$OUT_DIR"/*.cpp; then
  echo "[http scope accept] FAIL: generated C++ missing .spawn(" >&2
  exit 1
fi

echo "[http scope accept] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=http_scope_accept_loop_demo \
  compiler/build_bin.sh "$OUT_DIR" "$OUT_DIR/http_scope_accept_loop_demo"

port_file="$OUT_DIR/http_scope_accept_port.txt"
rm -f "$port_file"
server_log="$OUT_DIR/server.log"
(
  cd "$OUT_DIR"
  ./http_scope_accept_loop_demo
) >"$server_log" 2>&1 &
server_pid=$!

cleanup() {
  kill "$server_pid" 2>/dev/null || true
  wait "$server_pid" 2>/dev/null || true
}
trap cleanup EXIT

port=""
deadline=$((SECONDS + 30))
while (( SECONDS < deadline )); do
  if [[ -f "$port_file" ]] && [[ -s "$port_file" ]]; then
    port="$(tr -d '[:space:]' <"$port_file")"
    break
  fi
  if ! kill -0 "$server_pid" 2>/dev/null; then
    echo "[http scope accept] FAIL: server exited early" >&2
    cat "$server_log" >&2 || true
    exit 1
  fi
  sleep 0.05
done

if [[ -z "$port" ]]; then
  echo "[http scope accept] FAIL: no port file" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[http scope accept] ${CONNECTION_COUNT} parallel curls → 127.0.0.1:$port" >&2
start_ns="$(date +%s%N)"
pids=()
bodies=()
for index in $(seq 1 "$CONNECTION_COUNT"); do
  body_file="$OUT_DIR/body_${index}.txt"
  (
    curl -sS --max-time 10 "http://127.0.0.1:${port}/" >"$body_file"
  ) &
  pids+=($!)
done

curl_fail=0
for pid in "${pids[@]}"; do
  if ! wait "$pid"; then
    curl_fail=1
  fi
done
end_ns="$(date +%s%N)"
wall_ms=$(( (end_ns - start_ns) / 1000000 ))

if [[ "$curl_fail" -ne 0 ]]; then
  echo "[http scope accept] FAIL: curl exited non-zero" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

for index in $(seq 1 "$CONNECTION_COUNT"); do
  body="$(tr -d '[:space:]' <"$OUT_DIR/body_${index}.txt" || true)"
  if [[ "$body" != "ok" ]]; then
    echo "[http scope accept] FAIL curl #$index body='$body' expected=ok" >&2
    cat "$server_log" >&2 || true
    exit 1
  fi
done

wait "$server_pid"
server_status=$?
trap - EXIT
if [[ "$server_status" -ne 0 ]]; then
  echo "[http scope accept] FAIL server exit=$server_status" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[http scope accept] wall_ms=${wall_ms} max=${MAX_PARALLEL_MS}" >&2
if [[ "$wall_ms" -ge "$MAX_PARALLEL_MS" ]]; then
  echo "[http scope accept] FAIL: wall ${wall_ms}ms >= ${MAX_PARALLEL_MS}ms (looks serial)" >&2
  exit 1
fi
# Also require clearly below serial floor (3× sleep).
serial_floor_ms=$(( HANDLER_SLEEP_MS * 3 ))
if [[ "$wall_ms" -ge "$serial_floor_ms" ]]; then
  echo "[http scope accept] FAIL: wall ${wall_ms}ms >= serial floor ${serial_floor_ms}ms" >&2
  exit 1
fi

echo "[http scope accept] PASS (wall ${wall_ms}ms < ${MAX_PARALLEL_MS}ms)"

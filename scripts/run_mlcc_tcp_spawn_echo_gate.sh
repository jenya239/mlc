#!/usr/bin/env bash
# TRACK_PIPELINE_MERGE_TCP_SPAWN STEP=4 — Tcp + spawn, two parallel clients.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

mlcc="${MLCC:-$root/compiler/out/mlcc}"
if [[ ! -x "$mlcc" ]]; then
  echo "mlcc missing: $mlcc (run compiler/build.sh)" >&2
  exit 1
fi

out_dir="${TMPDIR:-$root/.tmp_mlcc_tcp_spawn}/mlcc_tcp_spawn_echo_gate"
rm -rf "$out_dir"
mkdir -p "$out_dir"

echo "[mlcc tcp spawn echo] codegen" >&2
"$mlcc" -o "$out_dir" misc/examples/tcp_spawn_echo_mlcc.mlc

echo "[mlcc tcp spawn echo] link server" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=tcp_spawn_echo_mlcc compiler/build_bin.sh "$out_dir" "$out_dir/tcp_spawn_echo"

echo "[mlcc tcp spawn echo] link client" >&2
"${CXX:-g++}" -std=c++20 -O0 -pthread -o "$out_dir/tcp_echo_client" runtime/test/tcp_echo_client.cpp

port_file="$out_dir/tcp_spawn_port.txt"
marker_a="$out_dir/tcp_spawn_marker_a.txt"
marker_b="$out_dir/tcp_spawn_marker_b.txt"
rm -f "$port_file" "$marker_a" "$marker_b"
server_log="$out_dir/server.log"
(
  cd "$out_dir"
  ./tcp_spawn_echo
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
    echo "server exited early:" >&2
    cat "$server_log" >&2 || true
    exit 1
  fi
  sleep 0.05
done

if [[ -z "$port" ]]; then
  echo "server did not write port file" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[mlcc tcp spawn echo] two clients → port $port" >&2
client_a_log="$out_dir/client_a.log"
client_b_log="$out_dir/client_b.log"
"$out_dir/tcp_echo_client" "$port" >"$client_a_log" 2>&1 &
client_a_pid=$!
"$out_dir/tcp_echo_client" "$port" >"$client_b_log" 2>&1 &
client_b_pid=$!

saw_overlap=0
overlap_deadline=$((SECONDS + 30))
while (( SECONDS < overlap_deadline )); do
  a_state=""
  b_state=""
  if [[ -f "$marker_a" ]]; then
    a_state="$(tr -d '[:space:]' <"$marker_a" || true)"
  fi
  if [[ -f "$marker_b" ]]; then
    b_state="$(tr -d '[:space:]' <"$marker_b" || true)"
  fi
  if [[ "$a_state" == "start" && "$b_state" == "start" ]]; then
    saw_overlap=1
    break
  fi
  if ! kill -0 "$server_pid" 2>/dev/null; then
    break
  fi
  sleep 0.01
done

client_a_status=0
client_b_status=0
wait "$client_a_pid" || client_a_status=$?
wait "$client_b_pid" || client_b_status=$?

if [[ "$client_a_status" -ne 0 ]]; then
  echo "client A failed: $client_a_status" >&2
  cat "$client_a_log" >&2 || true
  exit 1
fi
if [[ "$client_b_status" -ne 0 ]]; then
  echo "client B failed: $client_b_status" >&2
  cat "$client_b_log" >&2 || true
  exit 1
fi

wait "$server_pid"
server_status=$?
trap - EXIT
if [[ "$server_status" -ne 0 ]]; then
  echo "server exit=$server_status" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

if [[ "$saw_overlap" -ne 1 ]]; then
  echo "handlers did not overlap (markers a=$(cat "$marker_a" 2>/dev/null) b=$(cat "$marker_b" 2>/dev/null))" >&2
  exit 1
fi

echo "[mlcc tcp spawn echo] OK (overlap proven)"

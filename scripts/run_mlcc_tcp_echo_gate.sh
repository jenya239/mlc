#!/usr/bin/env bash
# TRACK_PIPELINE_MERGE_TCP_SPAWN STEP=3 — Tcp echo via self-hosted mlcc (no spawn).
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

mlcc="${MLCC:-$root/compiler/out/mlcc}"
if [[ ! -x "$mlcc" ]]; then
  echo "mlcc missing: $mlcc (run compiler/build.sh)" >&2
  exit 1
fi

out_dir="${TMPDIR:-$root/.tmp_mlcc_tcp_echo}/mlcc_tcp_echo_gate"
rm -rf "$out_dir"
mkdir -p "$out_dir"

echo "[mlcc tcp echo] codegen" >&2
"$mlcc" -o "$out_dir" misc/examples/tcp_echo_mlcc.mlc

echo "[mlcc tcp echo] link server" >&2
MLCC_ENTRY_BASENAME=tcp_echo_mlcc compiler/build_bin.sh "$out_dir" "$out_dir/tcp_echo"

echo "[mlcc tcp echo] link client" >&2
"${CXX:-g++}" -std=c++20 -O0 -o "$out_dir/tcp_echo_client" runtime/test/tcp_echo_client.cpp

port_file="$out_dir/tcp_echo_port.txt"
rm -f "$port_file"
server_log="$out_dir/server.log"
(
  cd "$out_dir"
  ./tcp_echo
) >"$server_log" 2>&1 &
server_pid=$!

cleanup() {
  kill "$server_pid" 2>/dev/null || true
  wait "$server_pid" 2>/dev/null || true
}
trap cleanup EXIT

port=""
deadline=$((SECONDS + 10))
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

echo "[mlcc tcp echo] client → port $port" >&2
"$out_dir/tcp_echo_client" "$port"

wait "$server_pid"
server_status=$?
trap - EXIT
if [[ "$server_status" -ne 0 ]]; then
  echo "server exit=$server_status" >&2
  cat "$server_log" >&2 || true
  exit 1
fi

echo "[mlcc tcp echo] OK"

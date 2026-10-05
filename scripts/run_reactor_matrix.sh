#!/usr/bin/env bash
# Reactor matrix: timer and HTTPS smokes, then the HTTPS regression.
# A non-zero status from any step stops the script.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
mkdir -p "$TMPDIR"

run_step() {
  local name="$1"
  shift
  echo "[reactor matrix] step=$name" >&2
  if ! "$@"; then
    echo "[reactor matrix] FAIL step=$name" >&2
    exit 1
  fi
}

run_step reactor_cpp bash "$ROOT_DIR/scripts/run_reactor_cpp_smoke.sh"
run_step reactor_https bash "$ROOT_DIR/scripts/run_reactor_https_cpp_smoke.sh"
run_step reactor_mlc bash "$ROOT_DIR/scripts/run_reactor_mlc_smoke.sh"

run_step https_gate env HTTPS_CLIENT_REQUIRE=1 bash "$ROOT_DIR/scripts/run_https_client_gate.sh"
run_step curl_abi bash "$ROOT_DIR/scripts/run_curl_abi_cpp_smoke.sh"
run_step https_proxy bash "$ROOT_DIR/scripts/run_https_proxy_smoke.sh"
run_step https_stream bash "$ROOT_DIR/scripts/run_https_stream_smoke.sh"
run_step https_session bash "$ROOT_DIR/scripts/run_https_session_smoke.sh"
run_step https_redirect bash "$ROOT_DIR/scripts/run_https_redirect_smoke.sh"
run_step https_retry bash "$ROOT_DIR/scripts/run_https_retry_smoke.sh"
run_step https_live bash "$ROOT_DIR/scripts/run_https_client_live_smoke.sh"

echo "[reactor matrix] ok"

#!/usr/bin/env bash
# TRACK_STDLIB_LOGIC_TO_MLC STEP=6 — Env/Log via mlcc (not Ruby-only).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"

if [[ ! -x "$MLCC" ]]; then
  echo "[env_log gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

echo "[env_log gate] Env runtime smoke (getenv abi)"
bash scripts/run_env_log_runtime_smoke.sh

echo "[env_log gate] mlcc Env abi smoke"
OUT_ENV="${TMPDIR:-$ROOT/.tmp_env_log_gate}/env_mlc_abi_smoke"
rm -rf "$OUT_ENV"
mkdir -p "$OUT_ENV"
"$MLCC" -o "$OUT_ENV" misc/examples/env_mlc_abi_smoke.mlc
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=env_mlc_abi_smoke \
  compiler/build_bin.sh "$OUT_ENV" "$OUT_ENV/env_mlc_abi_smoke"
"$OUT_ENV/env_mlc_abi_smoke"

echo "[env_log gate] mlcc Log JSON-lines smoke"
OUT_LOG="${TMPDIR:-$ROOT/.tmp_env_log_gate}/log_mlc_smoke"
rm -rf "$OUT_LOG"
mkdir -p "$OUT_LOG"
"$MLCC" -o "$OUT_LOG" misc/examples/log_mlc_smoke.mlc
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=log_mlc_smoke \
  compiler/build_bin.sh "$OUT_LOG" "$OUT_LOG/log_mlc_smoke"
stderr_file="$OUT_LOG/stderr.txt"
"$OUT_LOG/log_mlc_smoke" 2>"$stderr_file"
bundle exec ruby -e '
expected = [
  "{\"level\":\"info\",\"msg\":\"hello\"}",
  "{\"level\":\"warn\",\"msg\":\"a\\\"b\\\\c\\nd\"}",
  "{\"level\":\"error\",\"msg\":\"boom\"}",
  "{\"level\":\"debug\",\"msg\":\"trace\"}",
]
actual = File.read(ARGV[0]).split("\n")
if actual != expected
  warn "[env_log gate] FAIL: Log stderr mismatch"
  warn "expected=#{expected.inspect}"
  warn "actual=#{actual.inspect}"
  exit 1
end
' "$stderr_file"

echo "[env_log gate] OK"

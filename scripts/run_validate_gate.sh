#!/usr/bin/env bash
# TRACK_STDLIB_LOGIC_TO_MLC STEP=6 — Validate via mlcc (not Ruby-only).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
MLCC="${MLCC:-$ROOT/compiler/out/mlcc}"

if [[ ! -x "$MLCC" ]]; then
  echo "[validate gate] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

echo "[validate gate] mlcc Validate smoke (valid/invalid)"
OUT="${TMPDIR:-$ROOT/.tmp_validate_gate}/validate_mlc_smoke"
rm -rf "$OUT"
mkdir -p "$OUT"
"$MLCC" -o "$OUT" misc/examples/validate_mlc_smoke.mlc
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=validate_mlc_smoke \
  compiler/build_bin.sh "$OUT" "$OUT/validate_mlc_smoke"
"$OUT/validate_mlc_smoke"

echo "[validate gate] mlcc bare-name Env/Log/Validate smoke"
OUT_BARE="${TMPDIR:-$ROOT/.tmp_validate_gate}/stdlib_logic_bare_smoke"
rm -rf "$OUT_BARE"
mkdir -p "$OUT_BARE"
"$MLCC" -o "$OUT_BARE" misc/examples/stdlib_logic_bare_smoke.mlc
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=stdlib_logic_bare_smoke \
  compiler/build_bin.sh "$OUT_BARE" "$OUT_BARE/stdlib_logic_bare_smoke"
"$OUT_BARE/stdlib_logic_bare_smoke" >/dev/null

echo "[validate gate] OK"

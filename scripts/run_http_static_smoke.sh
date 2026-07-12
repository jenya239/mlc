#!/usr/bin/env bash
# TRACK_STDLIB_HTTP_HARDENING STEP=4 — serve_static unit smoke (no network).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${HTTP_STATIC_SMOKE_OUT:-${TMPDIR:-$ROOT_DIR/tmp}/http_static_smoke}"

if [[ ! -x "$MLCC" ]]; then
  echo "[http static] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[http static] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/http_static_smoke.mlc"

echo "[http static] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=http_static_smoke \
  TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}" \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/http_static_smoke"

mkdir -p "$OUT_DIR/static_root"
set +e
(cd "$OUT_DIR" && ./http_static_smoke)
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[http static] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[http static] ok exit=$status" >&2

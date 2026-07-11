#!/usr/bin/env bash
# TRACK_STDLIB_HTTP_MLC STEP=2 — pure-MLC HTTP parse/format (no network; native link).
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
OUT_DIR="${TMPDIR:-$ROOT_DIR/.tmp_http_parse}/http_server_parse_smoke"

if [[ ! -x "$MLCC" ]]; then
  echo "[http_server parse] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[http_server parse] codegen" >&2
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/examples/http_server_parse_smoke.mlc"

echo "[http_server parse] link" >&2
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=http_server_parse_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/http_server_parse_smoke"

set +e
"$OUT_DIR/http_server_parse_smoke"
status=$?
set -e
if [[ "$status" -ne 0 ]]; then
  echo "[http_server parse] FAIL exit=$status expected=0" >&2
  exit 1
fi
echo "[http_server parse] ok exit=$status" >&2

#!/usr/bin/env bash
# ASan/UBSan on FreeType + HarfBuzz smoke fixtures (TRACK_TEXT_RENDERING STEP=4.3).
# Requires MLC_SANITIZE support in compiler/build_bin.sh.
set -euo pipefail

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
FREETYPE_SMOKE="${COMPILER_DIR}/tests/run_freetype_glyph_smoke.sh"
HARFBUZZ_SMOKE="${COMPILER_DIR}/tests/run_harfbuzz_shape_smoke.sh"

if [[ ! -x "$FREETYPE_SMOKE" || ! -x "$HARFBUZZ_SMOKE" ]]; then
  echo "[text shim sanitize] FAIL: missing freetype/harfbuzz smoke scripts" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
mkdir -p "$TMPDIR"

# Fresh objects per sanitizer (tag includes _asan/_ubsan in build_bin.sh).
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

for sanitize in address undefined; do
  echo "[text shim sanitize] MLC_SANITIZE=${sanitize} freetype" >&2
  MLC_SANITIZE="${sanitize}" \
    FREETYPE_SMOKE_OUT="${ROOT_DIR}/tmp/freetype_glyph_smoke_${sanitize}" \
    "$FREETYPE_SMOKE"
  echo "[text shim sanitize] MLC_SANITIZE=${sanitize} harfbuzz" >&2
  MLC_SANITIZE="${sanitize}" \
    HARFBUZZ_SMOKE_OUT="${ROOT_DIR}/tmp/harfbuzz_shape_smoke_${sanitize}" \
    "$HARFBUZZ_SMOKE"
done

echo "[text shim sanitize] ok" >&2

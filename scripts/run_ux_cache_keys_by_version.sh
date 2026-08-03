#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107e — scenario cache_keys_by_version (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/cache_keys_by_version.mlc"
OUT_DIR="${UX_CACHE_KEYS_BY_VERSION_OUT:-$ROOT_DIR/tmp/ux_cache_keys_by_version}"
BIN_OUT="$OUT_DIR/bin"
PIECE="$ROOT_DIR/misc/editor/document/piece_table.mlc"
WRAP="$ROOT_DIR/misc/editor/layout/wrap_cache.mlc"
OVERFLOW="$ROOT_DIR/misc/editor/ux/overflow.mlc"
SPANS="$ROOT_DIR/misc/editor/ux/syntax_span_cache.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"

if [ ! -x "$MLCC" ]; then
  echo "[ux cache_keys_by_version] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

# Arch: version field present; full-text cache compares gone.
if ! grep -Eq 'version[[:space:]]*:[[:space:]]*i32' "$PIECE"; then
  echo "[ux cache_keys_by_version] FAIL: TextDocument.version: i32 missing" >&2
  exit 1
fi
if grep -q 'state.text != next_text' "$WRAP" "$OVERFLOW"; then
  echo "[ux cache_keys_by_version] FAIL: wrap/overflow still full-text compare" >&2
  exit 1
fi
if grep -q 'state.text == text' "$SPANS"; then
  echo "[ux cache_keys_by_version] FAIL: syntax_span_cache still full-text compare" >&2
  exit 1
fi
if grep -q 'prev_text != next_text' "$MINIMAP"; then
  echo "[ux cache_keys_by_version] FAIL: minimap still full-text compare" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

"$MLCC" -o "$OUT_DIR" "$ENTRY"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
set +e
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[ux cache_keys_by_version] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok cache_keys_by_version'; then
  echo "[ux cache_keys_by_version] FAIL: missing ux_ok cache_keys_by_version" >&2
  exit 1
fi
echo "[ux cache_keys_by_version] ok" >&2

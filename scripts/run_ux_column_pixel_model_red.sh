#!/usr/bin/env bash
# TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX STEP=1 — confirm both L1/L2 stubs are still red.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"

run_red() {
  local name="$1"
  local entry="$ROOT_DIR/misc/editor/ux_scenarios/${name}.mlc"
  local out_dir="$ROOT_DIR/tmp/ux_column_pixel_model_red_${name}"
  local bin_out="$out_dir/bin"
  local fail_token="ux_fail ${name} stub"
  rm -rf "$out_dir"
  mkdir -p "$out_dir"
  "$MLCC" -o "$out_dir" "$entry"
  "$COMPILER_DIR/build_bin.sh" "$out_dir" "$bin_out"
  set +e
  output=$("$bin_out" 2>&1)
  status=$?
  set -e
  printf '%s\n' "$output"
  if [ "$status" -eq 0 ]; then
    echo "[ux column_pixel_model_red] FAIL $name: exit 0 (expected red)" >&2
    exit 1
  fi
  if ! printf '%s\n' "$output" | grep -q "$fail_token"; then
    echo "[ux column_pixel_model_red] FAIL $name: missing $fail_token" >&2
    exit 1
  fi
}

run_red selection_highlight_matches_glyph_x
run_red selection_full_line_uses_display_columns

echo "ux_ok column_pixel_model_red"
echo "[ux column_pixel_model_red] ok" >&2

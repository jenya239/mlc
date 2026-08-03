#!/usr/bin/env bash
# TRACK_EDITOR_RETAINED_PAINT §108b — chrome-only reuses retained text+gutter batch.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/hover_no_text_layer_rebuild.mlc"
OUT_DIR="${UX_HOVER_NO_TEXT_LAYER_OUT:-$ROOT_DIR/tmp/ux_hover_no_text_layer_rebuild}"
BIN_OUT="$OUT_DIR/bin"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
BATCH="$ROOT_DIR/misc/editor/ux/text_layer_batch.mlc"

fail() {
  echo "[ux hover_no_text_layer_rebuild] FAIL: $1" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$BATCH" ] || fail "missing text_layer_batch.mlc"
[ -f "$DEMO" ] || fail "missing demo_live.mlc"

grep -q 'editor_ux_text_layer_batch_can_replay' "$BATCH" || fail "batch missing can_replay"
grep -q 'editor_ux_text_layer_batch_store' "$BATCH" || fail "batch missing store"
grep -q 'text_layer_batch' "$DEMO" || fail "demo_live missing text_layer_batch"
grep -q 'editor_ux_text_layer_batch_can_replay' "$DEMO" || fail "demo_live missing can_replay"
grep -q 'retained_editor_lines\|text_layer_batch.lines\|batch.lines' "$DEMO" || fail "demo_live missing retain replay"

# Document path must skip fresh editor_lines rebuild when replaying.
if ! grep -q 'editor_ux_text_layer_batch_can_replay' "$DEMO"; then
  fail "demo_live not gated by can_replay"
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
  fail "binary exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok hover_no_text_layer_rebuild'; then
  fail "missing ux_ok"
fi
echo "[ux hover_no_text_layer_rebuild] ok" >&2

#!/usr/bin/env bash
# TRACK_EDITOR_SCROLL_CONTENT_DIRTY §111a0 STEP=2 Green —
# scroll change ⇒ content_dirty + layout_skip=0 + drop visual row cache.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
HELPER="$ROOT_DIR/misc/editor/app/scroll_frame.mlc"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/scroll_content_dirty_forces_collect.mlc"
OUT_DIR="${EDITOR_SCROLL_CONTENT_DIRTY_OUT:-$ROOT_DIR/tmp/editor_scroll_content_dirty}"
BIN_OUT="$OUT_DIR/bin"

fail() {
  echo "[editor scroll content dirty] FAIL: $*" >&2
  exit 1
}

[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"
[ -f "$HELPER" ] || fail "missing $HELPER"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -f "$DEMO" ] || fail "missing $DEMO"

grep -q 'export fn editor_scroll_frame_note_changed' "$HELPER" ||
  fail "helper missing editor_scroll_frame_note_changed"
grep -q 'editor_scroll_frame_note_changed\|editor_live_note_scroll_changed\|editor_scroll_frame_after_offsets' "$DEMO" ||
  fail "demo_live missing scroll frame note wire"

# Wire must clear layout_skip / cache on scroll paths (Decision sabotage targets).
wire_count="$(grep -c 'editor_scroll_frame_after_offsets\|editor_live_note_scroll_changed\|editor_scroll_frame_note_changed' "$DEMO" || true)"
if [ "$wire_count" -lt 4 ]; then
  fail "demo_live scroll note call sites too few (count=$wire_count)"
fi
if ! grep -q 'minimap_scroll' "$DEMO"; then
  fail "demo_live missing minimap_scroll path"
fi

# §111a1 S1 — clamp/ensure must note scroll (Decision sabotage: strip note → fail).
ruby "$ROOT_DIR/scripts/check_scroll_note_neighbors.rb" "$DEMO" ||
  fail "clamp/ensure missing editor_live_note_scroll_changed neighbor"

# Sabotage: helper must not no-op on scroll_changed=1.
if grep -q 'return flags$' "$HELPER"; then
  # Allowed early return when scroll_changed==0; require dirty assignment exists.
  grep -q 'content_dirty: 1' "$HELPER" || fail "helper no-ops content_dirty (Decision sabotage)"
  grep -q 'layout_skip: 0' "$HELPER" || fail "helper no-ops layout_skip (Decision sabotage)"
  grep -q 'cached_visual_rows_valid: 0' "$HELPER" || fail "helper no-ops cache clear (Decision sabotage)"
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
if ! printf '%s\n' "$output" | grep -q 'ux_ok scroll_content_dirty_forces_collect'; then
  fail "missing ux_ok scroll_content_dirty_forces_collect"
fi

echo "[editor scroll content dirty] ok" >&2

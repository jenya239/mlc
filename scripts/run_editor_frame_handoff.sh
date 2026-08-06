#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110g — Green: docs handoff pointers.
# Docs-only grep/link checks — no GLFW / mlcc required.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
RED="$ROOT_DIR/scripts/run_editor_frame_handoff_red.sh"
GUI_ARCH="$ROOT_DIR/docs/GUI_ARCHITECTURE.md"
EDITOR_DOC="$ROOT_DIR/docs/EDITOR.md"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"
REPORT_DIR="${EDITOR_FRAME_HANDOFF_OUT:-$ROOT_DIR/.tmp/editor_frame_handoff}"
REPORT_FILE="$REPORT_DIR/report.txt"
RED_OUT="$REPORT_DIR/red.out"

fail() {
  echo "[editor_frame_handoff] FAIL: $1" >&2
  exit 1
}

[ -f "$RED" ] || fail "missing red harness"
[ -f "$GUI_ARCH" ] || fail "missing GUI_ARCHITECTURE.md"
[ -f "$EDITOR_DOC" ] || fail "missing EDITOR.md"
[ -f "$TRACK" ] || fail "missing TRACK"

gui_arch_link=fail
editor_doc_link=fail
residual_named=fail
harness_index=fail
destination_named=fail

grep -q 'TRACK_EDITOR_FRAME_ARCHITECTURE' "$GUI_ARCH" && gui_arch_link=ok
grep -q 'TRACK_EDITOR_FRAME_ARCHITECTURE' "$EDITOR_DOC" && editor_doc_link=ok

# Destination one-liner keywords in both docs.
if grep -Eqi 'paint list' "$GUI_ARCH" && grep -Eqi 'dirty' "$GUI_ARCH" && \
   grep -Eqi 'paint list' "$EDITOR_DOC" && grep -Eqi 'dirty' "$EDITOR_DOC"; then
  destination_named=ok
fi

# Residual must be named open (sabotage 3: docs must not claim it fixed).
if grep -Fq 'Residual (open' "$GUI_ARCH" && grep -Fq 'Residual (open' "$EDITOR_DOC" && \
   grep -Fqi 'paint list every frame' "$GUI_ARCH" && \
   grep -Fqi 'paint list every frame' "$EDITOR_DOC" && \
   ! grep -Eqi 'residual is (fixed|closed)|paint-list residual (fixed|closed)|every frame is (fixed|closed)' "$GUI_ARCH" && \
   ! grep -Eqi 'residual is (fixed|closed)|paint-list residual (fixed|closed)|every frame is (fixed|closed)' "$EDITOR_DOC"; then
  residual_named=ok
fi

# Harness index in GUI_ARCHITECTURE.
need=(
  run_editor_perf_dogfood_gate.sh
  run_editor_frame_ownership.sh
  run_editor_paint_list.sh
  run_editor_batch_stream.sh
  run_editor_glyph_damage.sh
  run_editor_overlay_metrics.sh
)
missing=0
for name in "${need[@]}"; do
  grep -q "$name" "$GUI_ARCH" || missing=1
done
[ "$missing" = 0 ] && harness_index=ok

# SceneNode chrome Deviation unchanged pointer.
grep -Eqi 'SceneNode chrome Deviation|SceneNode.*unchanged|do not reparent' "$GUI_ARCH" || \
  fail "GUI_ARCHITECTURE missing SceneNode chrome Deviation unchanged note"

[ "$gui_arch_link" = ok ] || fail "GUI_ARCHITECTURE lacks TRACK_EDITOR_FRAME_ARCHITECTURE link"
[ "$editor_doc_link" = ok ] || fail "EDITOR.md lacks TRACK_EDITOR_FRAME_ARCHITECTURE link"
[ "$destination_named" = ok ] || fail "docs lack destination keywords (paint list / dirty)"
[ "$residual_named" = ok ] || fail "residual paint-list-every-frame not named open"
[ "$harness_index" = ok ] || fail "GUI_ARCHITECTURE missing load-bearing harness index"

rm -rf "$REPORT_DIR"
mkdir -p "$REPORT_DIR"

if bash "$RED" >"$RED_OUT" 2>&1; then
  fail "red unexpectedly exited 0"
fi
grep -Eq 'already present|already links' "$RED_OUT" || \
  fail "red did not report already-present ($(head -1 "$RED_OUT"))"

{
  echo "gui_arch_link=$gui_arch_link"
  echo "editor_doc_link=$editor_doc_link"
  echo "destination_named=$destination_named"
  echo "residual_named=$residual_named"
  echo "harness_index=$harness_index"
  echo "red_already_present=ok"
} | tee "$REPORT_FILE"

echo "[editor_frame_handoff] OK" >&2

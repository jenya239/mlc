#!/usr/bin/env bash
# TRACK_EDITOR_FRAME_ARCHITECTURE §110g — Red: no frame-handoff docs harness.
# Excluded from run_ux_gate (*_red.sh). Green adds
# scripts/run_editor_frame_handoff.sh + GUI_ARCHITECTURE/EDITOR pointers.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
GREEN="$ROOT_DIR/scripts/run_editor_frame_handoff.sh"
GUI_ARCH="$ROOT_DIR/docs/GUI_ARCHITECTURE.md"
EDITOR_DOC="$ROOT_DIR/docs/EDITOR.md"
TRACK="$ROOT_DIR/docs/agent/TRACK_EDITOR_FRAME_ARCHITECTURE.md"
DOGFOOD_GATE="$ROOT_DIR/scripts/run_editor_perf_dogfood_gate.sh"
OVERLAY="$ROOT_DIR/scripts/run_editor_overlay_metrics.sh"

fail() {
  echo "[editor_frame_handoff_red] FAIL: $1" >&2
  exit 1
}

[ -f "$TRACK" ] || fail "missing TRACK_EDITOR_FRAME_ARCHITECTURE.md (unexpected drift)"
[ -f "$GUI_ARCH" ] || fail "missing GUI_ARCHITECTURE.md (unexpected drift)"
[ -f "$EDITOR_DOC" ] || fail "missing EDITOR.md (unexpected drift)"
[ -f "$DOGFOOD_GATE" ] || fail "missing dogfood gate (unexpected drift — §109k required)"
[ -f "$OVERLAY" ] || fail "missing overlay-metrics harness (unexpected drift — §110f required)"

# Red: green handoff harness absent until Green.
if [ -f "$GREEN" ]; then
  fail "green run_editor_frame_handoff.sh already present (expected gap until Green)"
fi

# Red: product docs lack §110 TRACK link until Green.
if grep -Fq 'TRACK_EDITOR_FRAME_ARCHITECTURE.md' "$GUI_ARCH"; then
  fail "GUI_ARCHITECTURE.md already links TRACK_EDITOR_FRAME_ARCHITECTURE.md (expected gap until Green)"
fi
if grep -Fq 'TRACK_EDITOR_FRAME_ARCHITECTURE.md' "$EDITOR_DOC"; then
  fail "EDITOR.md already links TRACK_EDITOR_FRAME_ARCHITECTURE.md (expected gap until Green)"
fi

# Red: no §110g Green measured paste yet.
if grep -A40 '### §110g Green measured\|Green measured (§110g)\|### Green measured (§110g)' "$TRACK" 2>/dev/null | \
  grep -Eq 'gui_arch_link=ok|editor_doc_link=ok|residual_named=ok|harness_index=ok'; then
  fail "TRACK already has frame-handoff post-cut measured counters (expected gap until Green)"
fi

fail "no editor frame handoff harness (Red expected)"

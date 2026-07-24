#!/usr/bin/env bash
# TRACK_EDITOR_WRAP_BUDGET_ADVANCE STEP=1 — no pixel-budget wrap by glyph advance yet.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
WRAP="$ROOT_DIR/misc/editor/layout/word_wrap.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_wrap_budget_advance_stable.sh"

if [ ! -f "$WRAP" ]; then
  echo "[ux wrap_budget_advance_red] FAIL: missing $WRAP" >&2
  exit 1
fi

# Current wrap is display-column × mono cell (gap vs shaped advances).
if ! grep -q 'utf8_count_display_columns' "$WRAP"; then
  echo "[ux wrap_budget_advance_red] FAIL: word_wrap missing display-column wrap" >&2
  exit 1
fi
if ! grep -q 'wrap_max_columns' "$WRAP"; then
  echo "[ux wrap_budget_advance_red] FAIL: word_wrap missing wrap_max_columns" >&2
  exit 1
fi
if grep -qE 'text_shaping_glyph_advance|glyph_advance|pixel_budget|advance_sum' "$WRAP"; then
  echo "[ux wrap_budget_advance_red] FAIL: word_wrap already has advance budget (expected gap)" >&2
  exit 1
fi

# Gap: stable stub must exist but must not assert wrap advance yet.
if [ ! -f "$STABLE" ]; then
  echo "[ux wrap_budget_advance_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok wrap_budget_advance_stable|pixel-budget wrap ok' "$STABLE"; then
  echo "[ux wrap_budget_advance_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[ux wrap_budget_advance_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok wrap_budget_advance_red"
echo "[ux wrap_budget_advance_red] ok" >&2

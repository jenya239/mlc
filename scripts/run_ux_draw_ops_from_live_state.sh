#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107q — EditorPaintOp live paint (q1 chrome bands + q2 tab slots).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PAINT_OPS="$ROOT_DIR/misc/editor/ux/paint_ops.mlc"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/tests/paint_ops_chrome_bands_unit.mlc"
OUT_DIR="${UX_DRAW_OPS_OUT:-$ROOT_DIR/tmp/ux_draw_ops_from_live_state}"
BIN_OUT="$OUT_DIR/bin"

fail() {
  echo "[ux draw_ops_from_live_state] FAIL: $1" >&2
  exit 1
}

[ -f "$DEMO" ] || fail "missing $DEMO"
[ -f "$PAINT_OPS" ] || fail "missing $PAINT_OPS"
[ -f "$ENTRY" ] || fail "missing $ENTRY"
[ -x "$MLCC" ] || fail "mlcc not found at $MLCC"

if ! grep -q 'export type EditorPaintOp' "$PAINT_OPS"; then
  fail "EditorPaintOp missing in paint_ops.mlc"
fi
if ! grep -q 'export fn editor_paint_ops_flatten' "$ROOT_DIR/misc/editor/ux/paint_ops_flatten.mlc"; then
  fail "editor_paint_ops_flatten missing"
fi
if ! grep -q 'export fn editor_paint_ops_chrome_bands' "$PAINT_OPS"; then
  fail "editor_paint_ops_chrome_bands missing"
fi
if ! grep -q "from './ux/paint_ops'" "$DEMO"; then
  fail "demo_live does not import paint_ops"
fi
if ! grep -q "from './ux/paint_ops_flatten'" "$DEMO"; then
  fail "demo_live does not import paint_ops_flatten"
fi
if ! grep -q 'editor_paint_ops_chrome_bands' "$DEMO"; then
  fail "demo_live does not build chrome band ops"
fi
if ! grep -q 'editor_paint_ops_flatten' "$DEMO"; then
  fail "demo_live does not flatten paint ops"
fi

# q1: no direct chrome-band quartet after paint begin (bg/header/tab_bar/toolbar strips).
if grep -qE 'solid_renderer_rect\(\s*$' "$DEMO"; then
  if awk '
    /solid = solid_renderer_begin\(solid\)/ { in_begin=1; next }
    in_begin && /solid_renderer_rect/ { rect=1 }
    rect && /0, 0, width, height/ { bg=1 }
    rect && /theme\.background_/ { bg_color=1 }
    /let mut hover_tab_index/ { in_begin=0; rect=0 }
    END { exit (bg && bg_color) ? 0 : 1 }
  ' "$DEMO"; then
    fail "demo_live still has direct background solid_renderer_rect quartet"
  fi
fi
if grep -qE '0, toolbar_y, width, toolbar_height\(\)' "$DEMO"; then
  fail "demo_live still has direct toolbar strip solid_renderer_rect"
fi

# q2: tab strip slots via ops (standalone tab_slot_ops or §108c chrome_layer_ops).
if ! grep -Eq 'tab_slot_ops|chrome_layer_ops' "$DEMO"; then
  fail "demo_live missing tab_slot_ops/chrome_layer_ops (q2)"
fi
if ! grep -q '"tab_slot"' "$DEMO"; then
  fail "demo_live missing tab_slot paint op id (q2)"
fi
if grep -q 'tab_fill.x, tab_fill.y, tab_fill.width, tab_fill.height' "$DEMO"; then
  fail "demo_live still paints tab_fill via direct solid_renderer_rect (q2)"
fi

# q3: tree / breadcrumb / folder-nav via ops (nav_chrome_ops or §108c chrome_layer_ops).
if ! grep -Eq 'nav_chrome_ops|chrome_layer_ops' "$DEMO"; then
  fail "demo_live missing nav_chrome_ops/chrome_layer_ops (q3)"
fi
if ! grep -q '"tree_panel"' "$DEMO"; then
  fail "demo_live missing tree_panel paint op id (q3)"
fi
if ! grep -q '"breadcrumb"' "$DEMO"; then
  fail "demo_live missing breadcrumb paint op id (q3)"
fi
if grep -q 'tree_rect.x, tree_rect.y, tree_rect.width, tree_rect.height' "$DEMO"; then
  fail "demo_live still paints tree_rect via direct solid_renderer_rect (q3)"
fi
if grep -q 'breadcrumb_fill.x, breadcrumb_fill.y, breadcrumb_fill.width, breadcrumb_fill.height' "$DEMO"; then
  fail "demo_live still paints breadcrumb_fill via direct solid_renderer_rect (q3)"
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE="${MLC_GLFW_VISIBLE:-0}"

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
  fail "paint_ops_chrome_bands_unit exit=$status"
fi
if ! printf '%s\n' "$output" | grep -q '\[ux\] draw_ops_from_live_state ok'; then
  fail "missing draw_ops_from_live_state ok"
fi

echo "ux_ok draw_ops_from_live_state"
echo "[ux draw_ops_from_live_state] ok" >&2

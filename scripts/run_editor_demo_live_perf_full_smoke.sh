#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107d — demo_live full-path perf smoke (no skip branches).
# Baseline remains scripts/run_editor_demo_live_perf_smoke.sh (MLC_EDITOR_PERF=1).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
OUT_DIR="${EDITOR_DEMO_LIVE_PERF_FULL_OUT:-$ROOT_DIR/tmp/editor_demo_live_perf_full_compile}"
BIN_OUT="$OUT_DIR/bin"
FIX_DIR="$ROOT_DIR/tmp/editor_demo_live_perf_full"
FIX_FILE="$FIX_DIR/perf_full.txt"
# Full wrap+spans+minimap on 100k is multi-minute/frame; 10k still exercises the
# real path while remaining a usable gate (baseline smoke keeps 100k + skips).
LINES="${MLC_EDITOR_PERF_LINES:-10000}"
# Full path is far costlier per frame; default 5 (baseline keeps 30).
FRAMES="${MLC_EDITOR_PERF_FRAMES:-5}"
# Ceiling: measured 2026-08-04 §109e Green total_us=6565154 / 5 frames on 10k-line
# fixture (wrap+spans+minimap+editor retained glyph batch). Default = measured × 1.25
# (< §109d ceiling 13259730).
TOTAL_US_MAX="${MLC_EDITOR_PERF_FULL_TOTAL_US_MAX:-8206442}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[editor demo_live_perf_full] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[editor demo_live_perf_full] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[editor demo_live_perf_full] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi
if [ ! -f "$DEMO" ]; then
  echo "[editor demo_live_perf_full] FAIL: missing $DEMO" >&2
  exit 1
fi

# Red: full path absent until Green wires MLC_EDITOR_PERF_FULL (no skip branches).
if ! grep -q 'MLC_EDITOR_PERF_FULL' "$DEMO"; then
  echo "[editor demo_live_perf_full] FAIL: demo_live does not handle MLC_EDITOR_PERF_FULL (baseline skip path still the only smoke)" >&2
  exit 1
fi
if ! grep -q 'demo_live_perf_full' "$ROOT_DIR/misc/editor/ui/perf.mlc" "$DEMO"; then
  echo "[editor demo_live_perf_full] FAIL: missing demo_live_perf_full tag helper/print" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=0
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_EDITOR_PERF_FULL=1
export MLC_EDITOR_PERF_FRAMES="$FRAMES"

mkdir -p "$FIX_DIR"
ruby -e '
path = ARGV[0]
n = ARGV[1].to_i
File.open(path, "w") do |file|
  n.times { |index| file.puts("line #{index} aabbccddeeffgg") }
end
' "$FIX_FILE" "$LINES"
export MLC_EDITOR_PERF_OPEN="$FIX_FILE"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
"$MLCC" -o "$OUT_DIR" "$DEMO"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ ! -x "$BIN_OUT" ]; then
  echo "[editor demo_live_perf_full] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi

set +e
# Stream so long first-frame 100k open is visible (do not buffer until exit).
output=$("$BIN_OUT" 2>&1 | tee /dev/stderr)
status=${PIPESTATUS[0]}
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[editor demo_live_perf_full] FAIL: binary exit=$status" >&2
  exit 1
fi

line="$(printf '%s\n' "$output" | grep -E '\[mlc-editor\] demo_live_perf_full frames=' | tail -n 1 || true)"
if [ -z "$line" ]; then
  echo "[editor demo_live_perf_full] FAIL: missing demo_live_perf_full line" >&2
  exit 1
fi

frames="$(printf '%s\n' "$line" | sed -n 's/.*frames=\([0-9][0-9]*\).*/\1/p')"
layout_us="$(printf '%s\n' "$line" | sed -n 's/.*layout_us=\([0-9][0-9]*\).*/\1/p')"
draw_us="$(printf '%s\n' "$line" | sed -n 's/.*draw_us=\([0-9][0-9]*\).*/\1/p')"
total_us="$(printf '%s\n' "$line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
if [ -z "$frames" ] || [ -z "$layout_us" ] || [ -z "$draw_us" ] || [ -z "$total_us" ]; then
  echo "[editor demo_live_perf_full] FAIL: could not parse timings from: $line" >&2
  exit 1
fi
if [ "$frames" -lt "$FRAMES" ]; then
  echo "[editor demo_live_perf_full] FAIL: frames=$frames < $FRAMES" >&2
  exit 1
fi
if [ "$layout_us" -le 0 ] && [ "$draw_us" -le 0 ] && [ "$total_us" -le 0 ]; then
  echo "[editor demo_live_perf_full] FAIL: all timings zero: $line" >&2
  exit 1
fi
if [ "$TOTAL_US_MAX" -gt 0 ] && [ "$total_us" -gt "$TOTAL_US_MAX" ]; then
  echo "[editor demo_live_perf_full] FAIL: total_us=$total_us > TOTAL_US_MAX=$TOTAL_US_MAX" >&2
  exit 1
fi

echo "ux_ok demo_live_perf_full"
echo "[editor demo_live_perf_full] ok frames=$frames layout_us=$layout_us draw_us=$draw_us total_us=$total_us" >&2

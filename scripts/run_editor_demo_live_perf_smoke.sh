#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97a STEP=2 — real demo_live wall-clock perf smoke.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
OUT_DIR="${EDITOR_DEMO_LIVE_PERF_OUT:-$ROOT_DIR/tmp/editor_demo_live_perf_compile}"
BIN_OUT="$OUT_DIR/bin"
FIX_DIR="$ROOT_DIR/tmp/editor_demo_live_perf"
FIX_FILE="$FIX_DIR/perf_100k.txt"
LINES="${MLC_EDITOR_PERF_LINES:-100000}"
FRAMES="${MLC_EDITOR_PERF_FRAMES:-30}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[editor demo_live_perf] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[editor demo_live_perf] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[editor demo_live_perf] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=0
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_EDITOR_PERF=1
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
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/editor/demo_live.mlc"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ ! -x "$BIN_OUT" ]; then
  echo "[editor demo_live_perf] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi

set +e
# Stream stdout so long first-frame 100k open is visible in logs.
output=$("$BIN_OUT" 2>&1)
status=$?
set -e
printf '%s\n' "$output"
if [ "$status" -ne 0 ]; then
  echo "[editor demo_live_perf] FAIL: binary exit=$status" >&2
  exit 1
fi

line="$(printf '%s\n' "$output" | grep -E '\[mlc-editor\] demo_live_perf frames=' | tail -n 1 || true)"
if [ -z "$line" ]; then
  echo "[editor demo_live_perf] FAIL: missing demo_live_perf line" >&2
  exit 1
fi

frames="$(printf '%s\n' "$line" | sed -n 's/.*frames=\([0-9][0-9]*\).*/\1/p')"
layout_us="$(printf '%s\n' "$line" | sed -n 's/.*layout_us=\([0-9][0-9]*\).*/\1/p')"
draw_us="$(printf '%s\n' "$line" | sed -n 's/.*draw_us=\([0-9][0-9]*\).*/\1/p')"
total_us="$(printf '%s\n' "$line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
if [ -z "$frames" ] || [ -z "$layout_us" ] || [ -z "$draw_us" ] || [ -z "$total_us" ]; then
  echo "[editor demo_live_perf] FAIL: could not parse timings from: $line" >&2
  exit 1
fi
if [ "$frames" -lt "$FRAMES" ]; then
  echo "[editor demo_live_perf] FAIL: frames=$frames < $FRAMES" >&2
  exit 1
fi
if [ "$layout_us" -le 0 ] && [ "$draw_us" -le 0 ] && [ "$total_us" -le 0 ]; then
  echo "[editor demo_live_perf] FAIL: all timings zero: $line" >&2
  exit 1
fi

echo "ux_ok demo_live_perf"
echo "[editor demo_live_perf] ok frames=$frames layout_us=$layout_us draw_us=$draw_us total_us=$total_us" >&2

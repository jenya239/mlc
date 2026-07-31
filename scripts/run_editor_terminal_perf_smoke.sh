#!/usr/bin/env bash
# TRACK_EDITOR_TERMINAL §102g — document idle budget + terminal flood budget.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
OUT_DIR="${EDITOR_TERMINAL_PERF_OUT:-$ROOT_DIR/tmp/editor_terminal_perf_compile}"
BIN_OUT="$OUT_DIR/bin"
FIX_DIR="$ROOT_DIR/tmp/editor_terminal_perf"
FIX_FILE="$FIX_DIR/perf_100k.txt"
LINES="${MLC_EDITOR_PERF_LINES:-100000}"
FRAMES="${MLC_EDITOR_PERF_FRAMES:-30}"

# Document ceiling: measured 2026-07-31 baseline total_us=557288 / 30 frames
# (§105-class). ~2.7× headroom against machine noise.
DOC_TOTAL_US_MAX="${MLC_EDITOR_DOC_PERF_TOTAL_US_MAX:-1500000}"
# Terminal flood ceiling: measured 2026-07-31 baseline total_us=735662 / 30
# frames under `seq 1 100000` on active terminal tab. ~2.7× headroom.
TERM_TOTAL_US_MAX="${MLC_EDITOR_TERM_PERF_TOTAL_US_MAX:-2000000}"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[editor terminal_perf] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -f /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf ]; then
  echo "[editor terminal_perf] SKIP: DejaVuSansMono.ttf not found" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[editor terminal_perf] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
export MLC_GLFW_VISIBLE=0
export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
export MLC_EDITOR_PERF_FRAMES="$FRAMES"

mkdir -p "$FIX_DIR"
ruby -e '
path = ARGV[0]
n = ARGV[1].to_i
File.open(path, "w") do |file|
  n.times { |index| file.puts("line #{index} aabbccddeeffgg") }
end
' "$FIX_FILE" "$LINES"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
"$MLCC" -o "$OUT_DIR" "$ROOT_DIR/misc/editor/demo_live.mlc"
"$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
if [ ! -x "$BIN_OUT" ]; then
  echo "[editor terminal_perf] FAIL: missing binary $BIN_OUT" >&2
  exit 1
fi

parse_perf_line() {
  local tag="$1"
  local output="$2"
  local line
  line="$(printf '%s\n' "$output" | grep -E "\[mlc-editor\] ${tag} frames=" | tail -n 1 || true)"
  if [ -z "$line" ]; then
    echo "[editor terminal_perf] FAIL: missing ${tag} line" >&2
    return 1
  fi
  frames="$(printf '%s\n' "$line" | sed -n 's/.*frames=\([0-9][0-9]*\).*/\1/p')"
  layout_us="$(printf '%s\n' "$line" | sed -n 's/.*layout_us=\([0-9][0-9]*\).*/\1/p')"
  draw_us="$(printf '%s\n' "$line" | sed -n 's/.*draw_us=\([0-9][0-9]*\).*/\1/p')"
  total_us="$(printf '%s\n' "$line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
  if [ -z "$frames" ] || [ -z "$layout_us" ] || [ -z "$draw_us" ] || [ -z "$total_us" ]; then
    echo "[editor terminal_perf] FAIL: could not parse timings from: $line" >&2
    return 1
  fi
  if [ "$frames" -lt "$FRAMES" ]; then
    echo "[editor terminal_perf] FAIL: ${tag} frames=$frames < $FRAMES" >&2
    return 1
  fi
  if [ "$layout_us" -le 0 ] && [ "$draw_us" -le 0 ] && [ "$total_us" -le 0 ]; then
    echo "[editor terminal_perf] FAIL: ${tag} all timings zero: $line" >&2
    return 1
  fi
  printf '%s\n' "$line"
}

# Phase A — document tab, terminal not focused (feature present, tab not open).
set +e
doc_output="$(
  env MLC_EDITOR_PERF=1 MLC_EDITOR_PERF_OPEN="$FIX_FILE" \
    "$BIN_OUT" 2>&1
)"
doc_status=$?
set -e
printf '%s\n' "$doc_output"
if [ "$doc_status" -ne 0 ]; then
  echo "[editor terminal_perf] FAIL: document phase exit=$doc_status" >&2
  exit 1
fi
doc_line="$(parse_perf_line demo_live_perf "$doc_output")"
doc_total_us="$(printf '%s\n' "$doc_line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
if [ "$doc_total_us" -gt "$DOC_TOTAL_US_MAX" ]; then
  echo "[editor terminal_perf] FAIL: document total_us=$doc_total_us > max=$DOC_TOTAL_US_MAX" >&2
  exit 1
fi
echo "[editor terminal_perf] document ok $doc_line (max_total_us=$DOC_TOTAL_US_MAX)" >&2

# Phase B — terminal tab active under high-throughput seq flood.
set +e
term_output="$(
  env MLC_EDITOR_TERMINAL_PERF=1 \
    "$BIN_OUT" 2>&1
)"
term_status=$?
set -e
printf '%s\n' "$term_output"
if [ "$term_status" -ne 0 ]; then
  echo "[editor terminal_perf] FAIL: terminal phase exit=$term_status" >&2
  exit 1
fi
term_line="$(parse_perf_line demo_live_terminal_perf "$term_output")"
term_total_us="$(printf '%s\n' "$term_line" | sed -n 's/.*total_us=\([0-9][0-9]*\).*/\1/p')"
if [ "$term_total_us" -gt "$TERM_TOTAL_US_MAX" ]; then
  echo "[editor terminal_perf] FAIL: terminal total_us=$term_total_us > max=$TERM_TOTAL_US_MAX" >&2
  exit 1
fi
echo "[editor terminal_perf] terminal ok $term_line (max_total_us=$TERM_TOTAL_US_MAX)" >&2

echo "ux_ok terminal_perf"
echo "[editor terminal_perf] ok doc_total_us=$doc_total_us term_total_us=$term_total_us" >&2

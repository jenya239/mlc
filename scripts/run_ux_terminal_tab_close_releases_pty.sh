#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107g — terminal tab-strip close releases PTY (L2).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="$ROOT_DIR/misc/editor/ux_scenarios/terminal_tab_close_releases_pty.mlc"
OUT_DIR="${UX_TERMINAL_TAB_CLOSE_RELEASES_PTY_OUT:-$ROOT_DIR/tmp/ux_terminal_tab_close_releases_pty}"
BIN_OUT="$OUT_DIR/bin"
STATE="$ROOT_DIR/misc/editor/app/state.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PTY_CPP="$ROOT_DIR/runtime/src/terminal/pty_abi.cpp"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists glfw3; then
  echo "[ux terminal_tab_close_releases_pty] SKIP: glfw3 not found (pkg-config)" >&2
  exit 0
fi
if [ ! -x "$MLCC" ]; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

# Arch: tab-strip tears down terminal; demo exit closes before context end; SIGHUP.
click_block="$(
  ruby -e '
    source = File.read(ARGV[0])
    start_at = source.index("export fn editor_app_click_tab_strip(")
    abort "missing click_tab_strip" if start_at.nil?
    end_at = source.index("\nexport fn ", start_at + 1)
    abort "missing next export" if end_at.nil?
    print source[start_at...end_at]
  ' "$STATE"
)"
if ! printf '%s\n' "$click_block" | grep -q 'editor_app_close_terminal'; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: tab-strip missing editor_app_close_terminal" >&2
  exit 1
fi
if ! awk '
  /fn main\(/ { in_main=1 }
  in_main && /editor_app_close_terminal\(/ { saw_close=1 }
  in_main && /glfw_gl_context_end\(/ {
    if (saw_close) { found=1 }
  }
  END { exit(found ? 0 : 1) }
' "$DEMO"; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: demo_live missing close before glfw_gl_context_end" >&2
  exit 1
fi
if ! grep -q 'SIGHUP' "$PTY_CPP"; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: pty_close missing SIGHUP" >&2
  exit 1
fi
if ! grep -q 'SIGKILL' "$PTY_CPP"; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: pty_close missing SIGKILL" >&2
  exit 1
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
  echo "[ux terminal_tab_close_releases_pty] FAIL binary exit=$status" >&2
  exit 1
fi
if ! printf '%s\n' "$output" | grep -q 'ux_ok terminal_tab_close_releases_pty'; then
  echo "[ux terminal_tab_close_releases_pty] FAIL: missing ux_ok" >&2
  exit 1
fi
echo "[ux terminal_tab_close_releases_pty] ok" >&2

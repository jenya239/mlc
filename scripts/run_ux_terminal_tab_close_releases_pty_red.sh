#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107g — Red: terminal teardown gaps still present.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_terminal_tab_close_releases_pty.sh +
# ux_scenarios/terminal_tab_close_releases_pty.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
STATE="$ROOT_DIR/misc/editor/app/state.mlc"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
PTY_CPP="$ROOT_DIR/runtime/src/terminal/pty_abi.cpp"
STABLE="$ROOT_DIR/scripts/run_ux_terminal_tab_close_releases_pty.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/terminal_tab_close_releases_pty.mlc"

fail() {
  echo "[ux terminal_tab_close_releases_pty_red] FAIL: $1" >&2
  exit 1
}

for path in "$STATE" "$DEMO" "$PTY_CPP"; do
  [ -f "$path" ] || fail "missing $path"
done

# Red: tab-strip close arm does not tear down the terminal session.
click_block="$(
  awk '/export fn editor_app_click_tab_strip\(/,/^export fn /' "$STATE" | head -n -1
)"
if [ -z "$click_block" ]; then
  click_block="$(awk '/export fn editor_app_click_tab_strip\(/,/^end$/' "$STATE")"
fi
if printf '%s\n' "$click_block" | grep -Eq 'editor_app_close_terminal|terminal_panel_session_close'; then
  fail "editor_app_click_tab_strip already tears down terminal (expected gap until Green)"
fi
if ! printf '%s\n' "$click_block" | grep -q 'tab_set_request_close'; then
  fail "editor_app_click_tab_strip missing tab_set_request_close (unexpected drift)"
fi

# Red: live exit path has no editor_app_close_terminal before final context end.
# CmdCloseTab is the only production call site today — window close still leaks.
demo_close_count="$(grep -c 'editor_app_close_terminal(' "$DEMO" || true)"
if [ "$demo_close_count" -ne 1 ]; then
  fail "demo_live editor_app_close_terminal count=$demo_close_count (expected 1 = CmdCloseTab only)"
fi
if ! awk '
  /fn main\(/ { in_main=1 }
  in_main && /editor_app_close_terminal\(/ { saw_close=1 }
  in_main && /glfw_gl_context_end\(/ {
    if (saw_close) { exit 2 }
  }
  END { exit 0 }
' "$DEMO"; then
  fail "demo_live already closes terminal before a glfw_gl_context_end (expected gap)"
fi

# Red: pty_close still WNOHANG-only (no SIGHUP/SIGKILL).
pty_close_block="$(
  awk '/std::int32_t pty_close\(/,/^}$/' "$PTY_CPP"
)"
if [ -z "$pty_close_block" ]; then
  fail "pty_close not found in pty_abi.cpp"
fi
if printf '%s\n' "$pty_close_block" | grep -Eq 'SIGHUP|SIGKILL|::kill\('; then
  fail "pty_close already signals child (expected gap until Green)"
fi
if ! printf '%s\n' "$pty_close_block" | grep -q 'WNOHANG'; then
  fail "pty_close missing WNOHANG (unexpected drift)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "tab-strip/window terminal teardown absent; pty_close still WNOHANG-only (Red expected)"

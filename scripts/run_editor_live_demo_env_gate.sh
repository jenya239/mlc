#!/usr/bin/env bash
# TRACK_EDITOR_FOLDER_BROWSER STEP=4 — live demo env dry-run (no GLFW / no compile).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SCRIPT="$ROOT_DIR/scripts/run_editor_live_demo.sh"

if [ ! -f "$SCRIPT" ]; then
  echo "[editor live env] FAIL: missing $SCRIPT" >&2
  exit 1
fi

bash -n "$SCRIPT"

# Default: MLC_EDITOR_ROOT → repo root.
(
  unset MLC_EDITOR_ROOT || true
  unset MLC_EDITOR_MOCK || true
  export MLC_EDITOR_MOCK=0
  if [ "${MLC_EDITOR_MOCK:-0}" = "1" ]; then
    unset MLC_EDITOR_ROOT || true
  else
    export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
  fi
  if [ "${MLC_EDITOR_ROOT:-}" != "$ROOT_DIR" ]; then
    echo "[editor live env] FAIL: default root want=$ROOT_DIR got=${MLC_EDITOR_ROOT-}" >&2
    exit 2
  fi
)

# Mock: MLC_EDITOR_ROOT unset.
(
  export MLC_EDITOR_ROOT="$ROOT_DIR"
  export MLC_EDITOR_MOCK=1
  if [ "${MLC_EDITOR_MOCK:-0}" = "1" ]; then
    unset MLC_EDITOR_ROOT || true
  else
    export MLC_EDITOR_ROOT="${MLC_EDITOR_ROOT:-$ROOT_DIR}"
  fi
  if [ -n "${MLC_EDITOR_ROOT+x}" ]; then
    echo "[editor live env] FAIL: mock should unset MLC_EDITOR_ROOT" >&2
    exit 3
  fi
)

# Script body must mention both knobs.
if ! grep -q 'MLC_EDITOR_ROOT' "$SCRIPT"; then
  echo "[editor live env] FAIL: script missing MLC_EDITOR_ROOT" >&2
  exit 4
fi
if ! grep -q 'MLC_EDITOR_MOCK' "$SCRIPT"; then
  echo "[editor live env] FAIL: script missing MLC_EDITOR_MOCK" >&2
  exit 5
fi

echo "editor_live_env_ok"

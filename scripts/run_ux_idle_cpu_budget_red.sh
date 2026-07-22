#!/usr/bin/env bash
# TRACK_EDITOR_IDLE_CPU_BUDGET STEP=1 — no live /proc idle CPU sample yet.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_idle_cpu_budget_stable.sh"

if [ ! -f "$DEMO" ]; then
  echo "[ux idle_cpu_budget_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Idle path present (#38) — something to sample.
if ! grep -q 'glfw_gl_context_wait_events_timeout' "$DEMO"; then
  echo "[ux idle_cpu_budget_red] FAIL: demo_live missing idle wait" >&2
  exit 1
fi
if ! grep -q 'content_dirty' "$DEMO"; then
  echo "[ux idle_cpu_budget_red] FAIL: demo_live missing content_dirty idle path" >&2
  exit 1
fi

# Gap: stable stub must exist but must not sample /proc yet.
if [ ! -f "$STABLE" ]; then
  echo "[ux idle_cpu_budget_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE '/proc/|utime|stime|jiffies|cpu_percent' "$STABLE"; then
  echo "[ux idle_cpu_budget_red] FAIL: stable already has live sample (expected gap)" >&2
  exit 1
fi

echo "ux_ok idle_cpu_budget_red"
echo "[ux idle_cpu_budget_red] ok" >&2

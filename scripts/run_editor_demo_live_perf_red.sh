#!/usr/bin/env bash
# TRACK_EDITOR_RENDER_ARCHITECTURE §97a STEP=1 — demo_live still has no real perf harness.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEMO="$ROOT_DIR/misc/editor/demo_live.mlc"
STABLE="$ROOT_DIR/scripts/run_editor_demo_live_perf_smoke.sh"
STUB_SMOKE="$ROOT_DIR/scripts/run_editor_perf_smoke.sh"

if [ ! -f "$DEMO" ]; then
  echo "[editor demo_live_perf_red] FAIL: missing $DEMO" >&2
  exit 1
fi

# Gap: demo_live has no MLC_EDITOR_PERF / Profile / editor_perf wiring.
if grep -qE 'MLC_EDITOR_PERF|editor_perf_|Profile\.scope_' "$DEMO"; then
  echo "[editor demo_live_perf_red] FAIL: demo_live already has perf wiring (expected gap)" >&2
  exit 1
fi

# Historical stub still exists and still asserts zeros (not the real harness).
if [ ! -f "$STUB_SMOKE" ]; then
  echo "[editor demo_live_perf_red] FAIL: missing historical stub $STUB_SMOKE" >&2
  exit 1
fi
if ! grep -q 'main.mlc' "$STUB_SMOKE"; then
  echo "[editor demo_live_perf_red] FAIL: stub smoke should still target main.mlc" >&2
  exit 1
fi
if ! grep -q 'layout_us=0' "$STUB_SMOKE"; then
  echo "[editor demo_live_perf_red] FAIL: stub smoke should still assert zero timings" >&2
  exit 1
fi

if [ ! -f "$STABLE" ]; then
  echo "[editor demo_live_perf_red] FAIL: missing stable stub $STABLE" >&2
  exit 1
fi
if grep -qE 'ux_ok demo_live_perf$|\[mlc-editor\] demo_live_perf ' "$STABLE"; then
  echo "[editor demo_live_perf_red] FAIL: stable already green (expected gap)" >&2
  exit 1
fi
if ! grep -q 'not implemented' "$STABLE"; then
  echo "[editor demo_live_perf_red] FAIL: stable stub missing not-implemented marker" >&2
  exit 1
fi

echo "ux_ok demo_live_perf_red"
echo "[editor demo_live_perf_red] ok" >&2

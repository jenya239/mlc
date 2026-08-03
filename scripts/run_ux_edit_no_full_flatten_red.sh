#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107f — Red: incremental line index / apply_edit snapshot absent.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_edit_no_full_flatten.sh + ux_scenarios/edit_no_full_flatten.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
LINE_INDEX="$ROOT_DIR/misc/editor/document/line_index.mlc"
FRAME_SNAPSHOT="$ROOT_DIR/misc/editor/document/frame_snapshot.mlc"
FRAME_CACHE="$ROOT_DIR/misc/editor/document/frame_snapshot_cache.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_edit_no_full_flatten.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/edit_no_full_flatten.mlc"

fail() {
  echo "[ux edit_no_full_flatten_red] FAIL: $1" >&2
  exit 1
}

for path in "$LINE_INDEX" "$FRAME_SNAPSHOT" "$FRAME_CACHE"; do
  [ -f "$path" ] || fail "missing $path"
done

# Red: incremental apply_edit API absent.
if grep -Eq 'export fn line_index_apply_edit\(' "$LINE_INDEX"; then
  fail "line_index_apply_edit already present (expected gap until Green)"
fi
if grep -Eq 'document_frame_snapshot_apply_edit|frame_snapshot_cache_apply_edit' \
  "$FRAME_SNAPSHOT" "$FRAME_CACHE"; then
  fail "apply_edit snapshot/cache API already present (expected gap until Green)"
fi

# Red: today's snapshot path still full-flattens + full line-index rebuild.
if ! grep -q 'document_to_string(document)' "$FRAME_SNAPSHOT"; then
  fail "frame_snapshot no longer calls document_to_string (expected today's gap)"
fi
if ! grep -q 'line_index_from_string(text)' "$FRAME_SNAPSHOT"; then
  fail "frame_snapshot no longer calls line_index_from_string (expected today's gap)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "line_index_apply_edit absent; edit snapshot still full-flattens (Red expected)"

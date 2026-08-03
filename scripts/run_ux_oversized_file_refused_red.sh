#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107m — Red: open_buffer_from_path has no size cap.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_oversized_file_refused.sh +
# ux_scenarios/oversized_file_refused.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SAVE="$ROOT_DIR/misc/editor/document/save.mlc"
FILE_ABI="$ROOT_DIR/runtime/include/mlc/io/file_abi.hpp"
STABLE="$ROOT_DIR/scripts/run_ux_oversized_file_refused.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/oversized_file_refused.mlc"

fail() {
  echo "[ux oversized_file_refused_red] FAIL: $1" >&2
  exit 1
}

[ -f "$SAVE" ] || fail "missing $SAVE"
[ -f "$FILE_ABI" ] || fail "missing $FILE_ABI"

if ! grep -q 'export fn open_buffer_from_path(' "$SAVE"; then
  fail "missing open_buffer_from_path"
fi

# Red: no size-cap API / env wiring yet.
if grep -Eq 'editor_max_open_bytes|MLC_EDITOR_MAX_OPEN_BYTES' "$SAVE"; then
  fail "size-cap API already present (expected gap until Green)"
fi
if grep -Eq 'file_byte_size|file_size_value' "$SAVE" "$FILE_ABI"; then
  fail "file_byte_size already present (expected gap until Green)"
fi

# Red: open path still does separate NUL + UTF-8 full walks.
if ! grep -q 'text_contains_nul(raw)' "$SAVE"; then
  fail "open path no longer calls text_contains_nul(raw) (expected today's gap)"
fi
if ! grep -q 'utf8_text_is_valid(body)' "$SAVE"; then
  fail "open path no longer calls utf8_text_is_valid(body) (expected today's gap)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

fail "open_buffer_from_path still accepts arbitrary size + multi-pass normalize (Red expected)"

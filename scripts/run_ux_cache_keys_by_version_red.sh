#!/usr/bin/env bash
# TRACK_EDITOR_HYGIENE §107e — Red: document version + version-keyed caches absent.
# Excluded from run_ux_gate.sh (*_red.sh). Green replaces this with
# scripts/run_ux_cache_keys_by_version.sh + ux_scenarios/cache_keys_by_version.mlc.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PIECE="$ROOT_DIR/misc/editor/document/piece_table.mlc"
DELETE="$ROOT_DIR/misc/editor/document/document_delete.mlc"
WRAP="$ROOT_DIR/misc/editor/layout/wrap_cache.mlc"
OVERFLOW="$ROOT_DIR/misc/editor/ux/overflow.mlc"
SPANS="$ROOT_DIR/misc/editor/ux/syntax_span_cache.mlc"
MINIMAP="$ROOT_DIR/misc/editor/ux/minimap.mlc"
STABLE="$ROOT_DIR/scripts/run_ux_cache_keys_by_version.sh"
SCENARIO="$ROOT_DIR/misc/editor/ux_scenarios/cache_keys_by_version.mlc"

fail() {
  echo "[ux cache_keys_by_version_red] FAIL: $1" >&2
  exit 1
}

for path in "$PIECE" "$DELETE" "$WRAP" "$OVERFLOW" "$SPANS" "$MINIMAP"; do
  [ -f "$path" ] || fail "missing $path"
done

# Red: TextDocument has no monotonic version field yet.
if grep -Eq 'version[[:space:]]*:[[:space:]]*i64' "$PIECE"; then
  fail "TextDocument already has version: i64 (expected gap until Green)"
fi

# Red: insert/delete do not bump a document version.
if grep -Eq 'version[[:space:]]*:' "$PIECE" "$DELETE"; then
  fail "document mutate path already mentions version (expected gap until Green)"
fi

# Red: all four caches still invalidate by full-text compare.
if ! grep -q 'state.text != next_text' "$WRAP"; then
  fail "wrap_cache no longer full-text compares (expected today's gap)"
fi
if ! grep -q 'state.text != next_text' "$OVERFLOW"; then
  fail "overflow no longer full-text compares (expected today's gap)"
fi
if ! grep -q 'state.text == text' "$SPANS"; then
  fail "syntax_span_cache no longer full-text compares (expected today's gap)"
fi
if ! grep -q 'prev_text != next_text' "$MINIMAP"; then
  fail "minimap no longer full-text compares (expected today's gap)"
fi

# Red: green gate artifacts must be absent.
if [ -f "$STABLE" ] || [ -f "$SCENARIO" ]; then
  fail "green gate already present (expected gap until Green)"
fi

# Structural gap proven — surface as a failing red gate for Driver STEP=1.
fail "TextDocument.version absent; caches still key on full text (Red expected)"

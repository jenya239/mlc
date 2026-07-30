#!/usr/bin/env bash
# Self-hosting-correctness round-trip: an existing mlcc translates the entry
# (stage p1), mlcc2 (built from p1's own C++ output via build_bin.sh)
# re-translates the same entry (stage p2) — diff must be empty. Encapsulates
# the manual steps documented in
# .cursor/rules/mlcc-self-host-verification.mdc. Review Step 22
# (mlc-support/responses/review_20260629_144027.md:453-473, "bootstrap-full.sh
# — stage2→stage3 identical check").
# Usage: compiler/scripts/bootstrap-full.sh [entry.mlc]
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${1:-$COMPILER_DIR/main.mlc}"

if [ ! -x "$MLCC" ]; then
  echo "bootstrap-full: missing $MLCC — run compiler/build.sh first" >&2
  exit 1
fi

WORK="${TMPDIR:-/tmp}/mlc_bootstrap_full"
rm -rf "$WORK"
mkdir -p "$WORK/p1" "$WORK/p2"

echo "bootstrap-full: entry=$ENTRY mlcc=$MLCC"
"$MLCC" -o "$WORK/p1" "$ENTRY"

"$COMPILER_DIR/build_bin.sh" "$WORK/p1" "$WORK/mlcc2"

"$WORK/mlcc2" -o "$WORK/p2" "$ENTRY"

DIFF_LOG="$WORK/diff.log"
if diff -rq --exclude=obj "$WORK/p1" "$WORK/p2" >"$DIFF_LOG"; then
  echo "STAGE IDENTICAL"
  exit 0
else
  echo "bootstrap-full: STAGE MISMATCH" >&2
  cat "$DIFF_LOG" >&2
  exit 1
fi

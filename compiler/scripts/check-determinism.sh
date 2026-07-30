#!/usr/bin/env bash
# Determinism check for --dump-mir/--dump-sem: an existing mlcc dumps the
# entry twice, independently, and the 2 dumps must be byte-identical — any
# diff means a codegen/checker pass iterates a Map (unordered) instead of an
# ordered array somewhere upstream of the dump. Review Step 23
# (mlc-support/responses/review_20260629_144027.md:477-494). Depends on
# Step 9 (--dump-mir/--dump-sem themselves — already implemented) and Step 22
# (bootstrap tooling convention this script follows).
# Usage: compiler/scripts/check-determinism.sh [entry.mlc]
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${MLCC:-$COMPILER_DIR/out/mlcc}"
ENTRY="${1:-$COMPILER_DIR/main.mlc}"

if [ ! -x "$MLCC" ]; then
  echo "check-determinism: missing $MLCC — run compiler/build.sh first" >&2
  exit 1
fi

WORK="${TMPDIR:-/tmp}/mlc_check_determinism"
rm -rf "$WORK"
mkdir -p "$WORK/d1" "$WORK/d2" "$WORK/s1" "$WORK/s2"

echo "check-determinism: entry=$ENTRY mlcc=$MLCC"

"$MLCC" --dump-mir --check-only -o "$WORK/d1" "$ENTRY" > "$WORK/mir1.txt" 2>&1
"$MLCC" --dump-mir --check-only -o "$WORK/d2" "$ENTRY" > "$WORK/mir2.txt" 2>&1
if diff "$WORK/mir1.txt" "$WORK/mir2.txt" > "$WORK/mir.diff"; then
  echo "MIR DETERMINISTIC"
else
  echo "check-determinism: MIR NON-DETERMINISTIC" >&2
  cat "$WORK/mir.diff" >&2
  exit 1
fi

"$MLCC" --dump-sem --check-only -o "$WORK/s1" "$ENTRY" > "$WORK/sem1.txt" 2>&1
"$MLCC" --dump-sem --check-only -o "$WORK/s2" "$ENTRY" > "$WORK/sem2.txt" 2>&1
if diff "$WORK/sem1.txt" "$WORK/sem2.txt" > "$WORK/sem.diff"; then
  echo "SEM DETERMINISTIC"
else
  echo "check-determinism: SEM NON-DETERMINISTIC" >&2
  cat "$WORK/sem.diff" >&2
  exit 1
fi

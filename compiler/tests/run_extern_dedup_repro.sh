#!/usr/bin/env bash
# TRACK_FFI_EXTERN_DEDUP — identical-sig redeclare must build+run (Hybrid skip-emit).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_DIR="$ROOT/compiler"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURE="$COMPILER_DIR/tests/fixtures/extern_dedup/redeclare.mlc"
WORK="${TMPDIR:-/tmp}/mlc_extern_dedup_repro_$$"
OUT="$WORK/out"
BIN="$WORK/bin"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

"$MLCC" -o "$OUT" "$FIXTURE"
if grep -n 'provider::sleep_ms_probe' "$OUT/redeclare.hpp" | grep -q 'noexcept;'; then
  echo "FAIL: consumer header still declares provider::sleep_ms_probe" >&2
  cat "$OUT/redeclare.hpp" >&2
  exit 1
fi
"$COMPILER_DIR/build_bin.sh" "$OUT" "$BIN"
STATUS=0
"$BIN" || STATUS=$?
if [[ "$STATUS" -ne 0 ]]; then
  echo "FAIL: redeclare binary exit=$STATUS (want 0)" >&2
  exit 1
fi
echo "extern_dedup_repro: ok"

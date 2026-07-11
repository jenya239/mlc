#!/usr/bin/env bash
# TRACK_STDLIB_LOGIC_TO_MLC STEP=4 — validate.hpp deleted; C++ smoke retired.
# Keep script so run_validate_gate.sh still has a runtime phase (no-op OK).
set -euo pipefail
echo "validate runtime smoke: skipped (logic in MLC; see misc/examples/validate_mlc_smoke.mlc)"
echo "validate runtime smoke OK"

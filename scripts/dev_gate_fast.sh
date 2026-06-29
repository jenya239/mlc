#!/usr/bin/env bash
# Tier A dev gate - default Driver verify between sub-steps.� default Driver verify between sub-steps.
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
bash compiler/tests/build_tests_fast.sh

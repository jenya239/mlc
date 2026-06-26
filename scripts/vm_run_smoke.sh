#!/usr/bin/env bash
# VM smoke via run_tests suite (no mlcc link required).
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
bash scripts/dev_gate_fast.sh 2>&1 | grep -E 'vm smoke|passed|FAIL'

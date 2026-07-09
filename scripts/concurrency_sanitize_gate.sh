#!/usr/bin/env bash
# Sanitizer matrix for concurrency stress (TRACK_CONCURRENCY_TEST_HARNESS T4).
# Loops asan / ubsan / tsan over run_concurrency_smoke.sh; fails on first error.

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
smoke="${root}/runtime/test/run_concurrency_smoke.sh"

if [[ ! -x "${smoke}" ]]; then
  echo "missing executable: ${smoke}" >&2
  exit 1
fi

# Prefer repo-local TMPDIR when /tmp is full (clang needs temp files).
if [[ -z "${TMPDIR:-}" ]]; then
  export TMPDIR="${root}/tmp"
  mkdir -p "${TMPDIR}"
fi

for sanitize in address undefined thread; do
  echo "[concurrency sanitize gate] MLC_SANITIZE=${sanitize}"
  MLC_SANITIZE="${sanitize}" "${smoke}"
done

echo "[concurrency sanitize gate] ok"

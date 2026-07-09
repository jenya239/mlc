#!/usr/bin/env bash
# Reddit / community demo helpers (TRACK_REDDIT_DEMO STEP=1).
#
# Usage:
#   scripts/reddit_demo.sh              # one-liner commands + quick status
#   scripts/reddit_demo.sh --commands   # one-liner commands only
#   scripts/reddit_demo.sh --run        # measure self-host timing + test count
#   scripts/reddit_demo.sh --record-baseline  # STEP=2: p1/p2 sec, tests, diff_exit
#   scripts/reddit_demo.sh --dry-run-gate       # STEP=5: demo + codegen determinism
#
# Full verify gate (TRACK):
#   compiler/tests/build_tests.sh
#   MLCC_BOOTSTRAP=1 compiler/build.sh
#   compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
compiler_dir="${root}/compiler"
out_dir="${compiler_dir}/out"
mlcc_binary="${out_dir}/mlcc"
bootstrap_binary="${out_dir}/bootstrap/mlcc_bootstrap"
entry_main="${compiler_dir}/main.mlc"
run_tests="${out_dir}/tests/run_tests"
baseline_file="${root}/docs/archive/tracks/reddit_demo_baseline.txt"

measure_seconds() {
  local output
  if ! output="$(/usr/bin/time -f '%e' "$@" 2>&1)"; then
    echo "$output" >&2
    return 1
  fi
  echo "$output" | tail -1
}

extract_test_summary() {
  "$@" 2>&1 | grep -oE '[0-9]+ passed, [0-9]+ failed' | tail -1 || true
}

print_one_liner_commands() {
  cat <<EOF
# MLC self-hosted compiler — copy/paste demo commands

# 1) Unit tests (Ruby builds run_tests, then executes)
compiler/tests/build_tests.sh

# 2) Build mlcc + bootstrap binary
MLCC_BOOTSTRAP=1 compiler/build.sh

# 3) Self-host codegen timing
time compiler/out/mlcc -o /tmp/mlc_selfhost_p1 compiler/main.mlc

# 4) E2E programs (compile + link + run)
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc

# 5) Full demo measurements
scripts/reddit_demo.sh --run
EOF
}

print_quick_status() {
  echo "[reddit_demo] repo=${root}"
  if [ -x "$mlcc_binary" ]; then
    echo "[reddit_demo] mlcc=${mlcc_binary} (present)"
  else
    echo "[reddit_demo] mlcc=missing (run: MLCC_FORCE_RUBY=1 compiler/build.sh)"
  fi
  if [ -x "$bootstrap_binary" ]; then
    echo "[reddit_demo] mlcc_bootstrap=${bootstrap_binary} (present)"
  else
    echo "[reddit_demo] mlcc_bootstrap=missing (run: MLCC_BOOTSTRAP=1 compiler/build.sh)"
  fi
  local module_count
  module_count="$(find "$compiler_dir" -name '*.mlc' | wc -l | tr -d ' ')"
  echo "[reddit_demo] compiler_modules=${module_count}"
  if [ -x "$run_tests" ]; then
    local test_summary
    test_summary="$(extract_test_summary "$run_tests")"
    if [ -n "$test_summary" ]; then
      echo "[reddit_demo] ${test_summary}"
    else
      echo "[reddit_demo] run_tests=failed to parse summary"
    fi
  else
    echo "[reddit_demo] run_tests=missing (run: compiler/tests/build_tests.sh)"
  fi
}

run_measurements() {
  if [ ! -x "$mlcc_binary" ]; then
    echo "[reddit_demo] FAIL: mlcc not found at ${mlcc_binary}" >&2
    echo "[reddit_demo] hint: MLCC_FORCE_RUBY=1 compiler/build.sh" >&2
    exit 1
  fi

  local temporary_output_directory
  temporary_output_directory="$(mktemp -d "${TMPDIR:-/tmp}/mlc_reddit_demo.XXXXXX")"

  local selfhost_seconds
  echo "[reddit_demo] self-host codegen → ${temporary_output_directory}" >&2
  selfhost_seconds="$(measure_seconds "$mlcc_binary" -o "$temporary_output_directory" "$entry_main")"
  echo "selfhost_codegen_sec=${selfhost_seconds}"

  local generated_cpp_count
  generated_cpp_count="$(find "$temporary_output_directory" -maxdepth 1 -name '*.cpp' | wc -l | tr -d ' ')"
  echo "selfhost_generated_cpp=${generated_cpp_count}"

  if [ -x "$run_tests" ]; then
    local test_summary
    test_summary="$(extract_test_summary "$run_tests")"
    if [ -z "$test_summary" ]; then
      echo "[reddit_demo] FAIL: could not parse run_tests summary" >&2
      exit 1
    fi
    echo "compiler_tests=${test_summary}"
  else
    echo "[reddit_demo] run_tests missing — run compiler/tests/build_tests.sh first" >&2
    exit 1
  fi

  if [ -x "$bootstrap_binary" ]; then
    echo "bootstrap_binary=${bootstrap_binary}"
  else
    echo "bootstrap_binary=missing"
  fi

  echo "[reddit_demo] ok"
  rm -rf "$temporary_output_directory"
}

run_selfhost_diff() {
  local phase_one_directory="$1"
  local phase_two_directory="$2"
  rm -rf "$phase_one_directory" "$phase_two_directory"
  mkdir -p "$phase_one_directory" "$phase_two_directory"
  local phase_one_seconds
  phase_one_seconds="$(measure_seconds "$mlcc_binary" -o "$phase_one_directory" "$entry_main")"
  local phase_two_seconds
  phase_two_seconds="$(measure_seconds "$mlcc_binary" -o "$phase_two_directory" "$entry_main")"
  local diff_exit_code=0
  if ! diff -rq "$phase_one_directory" "$phase_two_directory" >/dev/null; then
    diff_exit_code=1
  fi
  echo "mlcc_p1_sec=${phase_one_seconds}"
  echo "mlcc_p2_sec=${phase_two_seconds}"
  echo "diff_exit=${diff_exit_code}"
  return "$diff_exit_code"
}

record_baseline() {
  if [ ! -x "$mlcc_binary" ]; then
    echo "[reddit_demo] FAIL: mlcc not found at ${mlcc_binary}" >&2
    exit 1
  fi
  if [ ! -x "$run_tests" ]; then
    echo "[reddit_demo] FAIL: run_tests missing — run compiler/tests/build_tests.sh" >&2
    exit 1
  fi

  local work_directory
  work_directory="$(mktemp -d "${TMPDIR:-/tmp}/mlc_reddit_baseline.XXXXXX")"
  local phase_one_directory="${work_directory}/p1"
  local phase_two_directory="${work_directory}/p2"

  local test_summary
  test_summary="$(extract_test_summary "$run_tests")"
  if [ -z "$test_summary" ]; then
    echo "[reddit_demo] FAIL: could not parse run_tests summary" >&2
    rm -rf "$work_directory"
    exit 1
  fi

  echo "[reddit_demo] self-host p1 → p2 diff …" >&2
  set +e
  local selfhost_output
  selfhost_output="$(run_selfhost_diff "$phase_one_directory" "$phase_two_directory")"
  local selfhost_exit=$?
  set -e
  local generated_cpp_count
  generated_cpp_count="$(find "$phase_one_directory" -maxdepth 1 -name '*.cpp' | wc -l | tr -d ' ')"
  rm -rf "$work_directory"

  {
    echo "# reddit demo baseline $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "# recorded_by=scripts/reddit_demo.sh --record-baseline"
    echo "compiler_tests=${test_summary}"
    echo "$selfhost_output"
    echo "generated_cpp=${generated_cpp_count}"
  } | tee "$baseline_file"

  if [ "$selfhost_exit" -ne 0 ]; then
    echo "[reddit_demo] FAIL: self-host diff_exit!=0" >&2
    exit 1
  fi
  echo "[reddit_demo] wrote ${baseline_file}" >&2
}

dry_run_gate() {
  if [ ! -x "$mlcc_binary" ]; then
    echo "[reddit_demo] FAIL: mlcc not found at ${mlcc_binary}" >&2
    exit 1
  fi
  echo "[reddit_demo] dry-run: measurements + codegen determinism" >&2
  run_measurements
  local work_directory
  work_directory="$(mktemp -d "${TMPDIR:-/tmp}/mlc_reddit_dryrun.XXXXXX")"
  local phase_one_directory="${work_directory}/p1"
  local phase_two_directory="${work_directory}/p2"
  set +e
  local selfhost_output
  selfhost_output="$(run_selfhost_diff "$phase_one_directory" "$phase_two_directory")"
  local selfhost_exit=$?
  set -e
  rm -rf "$work_directory"
  echo "$selfhost_output"
  echo "[reddit_demo] TRACK gate (run before publish):"
  echo "  compiler/tests/build_tests.sh"
  echo "  MLCC_BOOTSTRAP=1 compiler/build.sh"
  echo "  compiler/tests/e2e/run_e2e.sh compiler/out/mlcc"
  if [ "$selfhost_exit" -ne 0 ]; then
    echo "[reddit_demo] FAIL: codegen diff_exit!=0" >&2
    exit 1
  fi
  echo "[reddit_demo] dry-run ok"
}

case "${1:-}" in
  --commands)
    print_one_liner_commands
    ;;
  --run)
    print_one_liner_commands
    echo ""
    run_measurements
    ;;
  --record-baseline)
    record_baseline
    ;;
  --dry-run-gate)
    dry_run_gate
    ;;
  --help|-h)
    echo "Usage: scripts/reddit_demo.sh [--commands | --run | --record-baseline | --dry-run-gate | --help]" >&2
    exit 0
    ;;
  "")
    print_one_liner_commands
    echo ""
    print_quick_status
    ;;
  *)
    echo "[reddit_demo] unknown option: $1" >&2
    exit 1
    ;;
esac

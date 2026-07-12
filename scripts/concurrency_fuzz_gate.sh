#!/usr/bin/env bash
# Nightly / local TestScheduler seed fuzz (TRACK_CONCURRENCY_TEST_HARNESS T6).
# Replays corpus seeds, then N random seeds via fuzz_scheduler --random.
# On fail, appends the FAIL seed line from stderr to the corpus.
#
# Env:
#   MLC_FUZZ_SEEDS   — random seed count after corpus (default 64)
#   MLC_FUZZ_SEED0   — optional seed0 for --random (omit = clock)

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
test_directory="${root}/runtime/test"
corpus_directory="${test_directory}/fuzz_corpus"
corpus_file="${corpus_directory}/scheduler_seeds.txt"
binary="${test_directory}/fuzz_scheduler"
source="${test_directory}/fuzz_scheduler.cpp"

source "${root}/compiler/scripts/select_cxx.sh"

mkdir -p "${corpus_directory}"
if [[ ! -f "${corpus_file}" ]]; then
  cat >"${corpus_file}" <<'EOF'
# Regression seeds for TestScheduler fuzz (one uint64 per line).
# Blank lines and # comments ignored. Gate appends failing seeds here.
EOF
fi

echo "[concurrency fuzz gate] build fuzz_scheduler"
"${CXX_CMD[@]}" -std=c++20 -pthread -I"${root}/runtime/include" \
  -O0 -g -o "${binary}" "${source}"

append_fail_seed() {
  local output="$1"
  local seed
  seed="$(printf '%s\n' "${output}" | sed -n 's/.*FAIL seed=\([0-9][0-9]*\).*/\1/p' | tail -n 1)"
  if [[ -n "${seed}" ]]; then
    if ! grep -qx "${seed}" "${corpus_file}" 2>/dev/null; then
      echo "${seed}" >>"${corpus_file}"
      echo "[concurrency fuzz gate] appended FAIL seed=${seed} to ${corpus_file}" >&2
    fi
  fi
}

run_one() {
  local seed="$1"
  echo "[concurrency fuzz gate] corpus seed=${seed}"
  local output=""
  set +e
  output="$("${binary}" "${seed}" 2>&1)"
  local status=$?
  set -e
  printf '%s\n' "${output}"
  if [[ "${status}" -ne 0 ]]; then
    append_fail_seed "${output}"
    echo "[concurrency fuzz gate] FAIL seed=${seed}" >&2
    exit 1
  fi
}

echo "[concurrency fuzz gate] corpus ${corpus_file}"
while IFS= read -r line || [[ -n "${line}" ]]; do
  line="${line#"${line%%[![:space:]]*}"}"
  line="${line%"${line##*[![:space:]]}"}"
  [[ -z "${line}" || "${line}" == \#* ]] && continue
  if [[ ! "${line}" =~ ^[0-9]+$ ]]; then
    echo "[concurrency fuzz gate] skip non-numeric corpus line: ${line}" >&2
    continue
  fi
  run_one "${line}"
done <"${corpus_file}"

seed_count="${MLC_FUZZ_SEEDS:-64}"
echo "[concurrency fuzz gate] random count=${seed_count}"
set +e
if [[ -n "${MLC_FUZZ_SEED0:-}" ]]; then
  random_output="$("${binary}" --random "${seed_count}" "${MLC_FUZZ_SEED0}" 2>&1)"
else
  random_output="$("${binary}" --random "${seed_count}" 2>&1)"
fi
random_status=$?
set -e
printf '%s\n' "${random_output}"
if [[ "${random_status}" -ne 0 ]]; then
  append_fail_seed "${random_output}"
  echo "[concurrency fuzz gate] FAIL random batch" >&2
  exit 1
fi

echo "[concurrency fuzz gate] ok (corpus + ${seed_count} random)"

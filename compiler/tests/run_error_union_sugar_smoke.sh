#!/usr/bin/env bash
# TRACK_LANG_ERROR_UNION STEP=4 — T!E smoke + e2e with ? (mlcc).
set -euo pipefail
COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
MLCC="${1:-$COMPILER_DIR/out/mlcc}"
FIXTURES="$COMPILER_DIR/tests/fixtures/error_union_sugar"
RT_INC="$ROOT_DIR/runtime/include"
RT_SRC="$ROOT_DIR/runtime/src/io/io.cpp $ROOT_DIR/runtime/src/core/string.cpp"
# shellcheck source=/dev/null
source "$ROOT_DIR/compiler/scripts/select_cxx.sh"

ok_output="$("$MLCC" --check-only "$FIXTURES/ok.mlc" 2>&1 || true)"
if echo "$ok_output" | grep -qiE 'error\[E|parse:'; then
  echo "FAIL unexpected error in ok.mlc:" >&2
  echo "$ok_output" >&2
  exit 1
fi

bad_output="$("$MLCC" --check-only "$FIXTURES/bang_missing_error.mlc" 2>&1 || true)"
echo "$bad_output" | grep -q 'expected type after ! in T!E' || {
  echo "FAIL expected T!E parse error in bang_missing_error.mlc:" >&2
  echo "$bad_output" >&2
  exit 1
}

question_check="$("$MLCC" --check-only "$FIXTURES/with_question.mlc" 2>&1 || true)"
if echo "$question_check" | grep -qiE 'error\[E|parse:'; then
  echo "FAIL unexpected error in with_question.mlc:" >&2
  echo "$question_check" >&2
  exit 1
fi

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT
"$MLCC" -o "$tmpdir" "$FIXTURES/with_question.mlc"
"${CXX_CMD[@]}" -std=c++20 -I "$tmpdir" -I "$RT_INC" "$tmpdir"/*.cpp $RT_SRC -o "$tmpdir/prog"
actual=$("$tmpdir/prog")
expected=$'6\ndivision by zero'
if [[ "$actual" != "$expected" ]]; then
  echo "FAIL with_question e2e: expected $(printf '%q' "$expected"), got $(printf '%q' "$actual")" >&2
  exit 1
fi

echo "error_union_sugar_smoke: ok"

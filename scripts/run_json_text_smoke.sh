#!/usr/bin/env bash
# JsonText: mlcc reads one JSON value as a sum. The body stays a string.
# The generated program must not link libcurl.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/json_text_smoke.mlc"
DIRECTORY="${JSON_TEXT_OUT:-$ROOT_DIR/tmp/json_text_smoke}"
CXX="${CXX:-c++}"
SOURCE="$ROOT_DIR/runtime/test/test_json_abi.cpp"

if [[ ! -x "$MLCC" ]]; then
  echo "[json text] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR"
rm -rf "$DIRECTORY"
mkdir -p "$DIRECTORY"

INCLUDE="$ROOT_DIR/runtime/include"
COMMON_FLAGS=(-std=c++20 -pthread -I"$INCLUDE" -Wall -Wextra)

echo "[json text] stage=abi_compile" >&2
if ! "$CXX" "${COMMON_FLAGS[@]}" -o "$DIRECTORY/test_json_abi" "$SOURCE"; then
  echo "[json text] FAIL stage=abi_compile" >&2
  exit 1
fi
echo "[json text] stage=abi_run" >&2
set +e
"$DIRECTORY/test_json_abi"
abi_status=$?
set -e
if [[ "$abi_status" -ne 0 ]]; then
  echo "[json text] FAIL stage=abi_run exit=$abi_status" >&2
  exit 1
fi

echo "[json text] stage=abi_tsan" >&2
if ! "$CXX" "${COMMON_FLAGS[@]}" -g -O1 -fsanitize=thread \
  -o "$DIRECTORY/test_json_abi_tsan" "$SOURCE"; then
  echo "[json text] FAIL stage=abi_tsan_compile" >&2
  exit 1
fi
set +e
TSAN_OPTIONS="${TSAN_OPTIONS:-halt_on_error=1}" "$DIRECTORY/test_json_abi_tsan"
tsan_status=$?
set -e
if [[ "$tsan_status" -ne 0 ]]; then
  echo "[json text] FAIL stage=abi_tsan exit=$tsan_status" >&2
  exit 1
fi

OUT_DIR="$DIRECTORY/generated"
mkdir -p "$OUT_DIR"
echo "[json text] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[json text] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[json text] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi
if grep -R -q -E 'curl_abi\.hpp|curl\.h' "$OUT_DIR" --include='*.hpp' --include='*.cpp' --include='*.txt'; then
  echo "[json text] FAIL stage=no_curl" >&2
  exit 1
fi
if [[ -f "$OUT_DIR/mlc_link_libs.txt" ]] && grep -q 'curl' "$OUT_DIR/mlc_link_libs.txt"; then
  echo "[json text] FAIL stage=link_libs" >&2
  exit 1
fi

echo "[json text] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=json_text_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/json_text_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[json text] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

echo "[json text] stage=run" >&2
set +e
"$DIRECTORY/json_text_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[json text] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

echo "[json text] ok"

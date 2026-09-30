#!/usr/bin/env bash
# HTTPS client STEP=2: request validation and header parsing are pure MLC.
# The generated C++ must not include the curl ABI header. Curl is not required.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_client_pure_smoke.mlc"
OUT_DIR="${HTTPS_PURE_OUT:-$ROOT_DIR/tmp/https_client_pure}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https pure] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[https pure] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https pure] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi

echo "[https pure] stage=no_curl_include" >&2
if grep -R -q 'curl_abi.hpp' "$OUT_DIR" --include='*.cpp'; then
  echo "[https pure] FAIL stage=no_curl_include" >&2
  exit 1
fi

echo "[https pure] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_client_pure_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/https_client_pure_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https pure] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

set +e
"$OUT_DIR/https_client_pure_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https pure] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

echo "[https pure] ok"

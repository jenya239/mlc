#!/usr/bin/env bash
# HTTPS client STEP=6: live HTTPS. Skips unless HTTPS_LIVE=1.
# HTTPS_LIVE_URL is the GET target. HTTPS_LIVE_POST_URL is an echo endpoint
# that returns the JSON body. HTTPS_LIVE_BAD_URL is optional and must present
# a certificate this machine does not trust. The CA path is SSL_CERT_FILE
# when set, otherwise libcurl's default store.
set -euo pipefail

if [[ "${HTTPS_LIVE:-0}" != "1" ]]; then
  echo "[https live] SKIP"
  exit 0
fi

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_client_live_smoke.mlc"
DIRECTORY="${HTTPS_LIVE_OUT:-$ROOT_DIR/tmp/https_client_live}"
CXX="${CXX:-c++}"

if [[ -z "${HTTPS_LIVE_URL:-}" || -z "${HTTPS_LIVE_POST_URL:-}" ]]; then
  echo "[https live] FAIL: set HTTPS_LIVE_URL and HTTPS_LIVE_POST_URL" >&2
  exit 1
fi

if [[ ! -x "$MLCC" ]]; then
  echo "[https live] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

if ! echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1 || ! pkg-config --exists libcurl; then
  echo "[https live] FAIL: libcurl headers or pkg-config missing" >&2
  exit 1
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR" "$DIRECTORY"

OUT_DIR="$DIRECTORY/generated"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[https live] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https live] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https live] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https live] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=https_client_live_smoke \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$DIRECTORY/https_client_live_smoke" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https live] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

set +e
"$DIRECTORY/https_client_live_smoke"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https live] FAIL stage=run exit=$run_status" >&2
  exit 1
fi

echo "[https live] ok"

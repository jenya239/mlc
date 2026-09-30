#!/usr/bin/env bash
# HTTPS client STEP=1: imported `extern lib "curl"` reaches mlc_link_libs.txt
# and the linked binary prints a libcurl version. No hand-written link line.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MLCC="${MLCC:-$ROOT_DIR/compiler/out/mlcc}"
ENTRY="$ROOT_DIR/misc/examples/https_link_probe/probe_main.mlc"
OUT_DIR="${HTTPS_LINK_PROBE_OUT:-$ROOT_DIR/tmp/https_link_probe}"
CXX="${CXX:-c++}"

if [[ ! -x "$MLCC" ]]; then
  echo "[https link] FAIL: mlcc not found at $MLCC" >&2
  exit 1
fi

curl_headers_present() {
  echo '#include <curl/curl.h>' | "$CXX" -E -x c++ - >/dev/null 2>&1
}

if ! curl_headers_present || ! pkg-config --exists libcurl; then
  if [[ "${HTTPS_CLIENT_REQUIRE:-0}" == "1" ]]; then
    echo "[https link] FAIL: libcurl headers or pkg-config missing" >&2
    exit 1
  fi
  echo "[https link] SKIP: libcurl headers missing" >&2
  exit 0
fi

export TMPDIR="${TMPDIR:-$ROOT_DIR/tmp}"
export MLCC_OBJ_CLEAN="${MLCC_OBJ_CLEAN:-1}"
export MLCC_PCH="${MLCC_PCH:-0}"
mkdir -p "$TMPDIR"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo "[https link] stage=codegen" >&2
set +e
"$MLCC" -o "$OUT_DIR" "$ENTRY" >"$OUT_DIR/mlcc_stdout.txt" 2>"$OUT_DIR/mlcc_stderr.txt"
codegen_status=$?
set -e
if [[ "$codegen_status" -ne 0 ]]; then
  cat "$OUT_DIR/mlcc_stdout.txt" >&2
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https link] FAIL stage=codegen exit=$codegen_status" >&2
  exit 1
fi
if grep -q 'W-EXTERN-ATTR' "$OUT_DIR/mlcc_stderr.txt"; then
  cat "$OUT_DIR/mlcc_stderr.txt" >&2
  echo "[https link] FAIL stage=codegen W-EXTERN-ATTR" >&2
  exit 1
fi

echo "[https link] stage=link_libs_file" >&2
if [[ ! -f "$OUT_DIR/mlc_link_libs.txt" ]] || ! grep -qx 'curl' "$OUT_DIR/mlc_link_libs.txt"; then
  echo "[https link] FAIL stage=link_libs_file missing curl" >&2
  if [[ -f "$OUT_DIR/mlc_link_libs.txt" ]]; then
    cat "$OUT_DIR/mlc_link_libs.txt" >&2
  fi
  exit 1
fi

echo "[https link] stage=cpp_link" >&2
set +e
MLCC_PCH=0 MLCC_DEV=1 MLCC_ENTRY_BASENAME=probe_main \
  "$ROOT_DIR/compiler/build_bin.sh" "$OUT_DIR" "$OUT_DIR/probe_main" \
  >"$OUT_DIR/link_stdout.txt" 2>"$OUT_DIR/link_stderr.txt"
link_status=$?
set -e
if [[ "$link_status" -ne 0 ]]; then
  cat "$OUT_DIR/link_stderr.txt" >&2
  echo "[https link] FAIL stage=cpp_link exit=$link_status" >&2
  exit 1
fi

set +e
version="$("$OUT_DIR/probe_main")"
run_status=$?
set -e
if [[ "$run_status" -ne 0 ]]; then
  echo "[https link] FAIL stage=run exit=$run_status output=$version" >&2
  exit 1
fi
case "$version" in
  libcurl/*) ;;
  *)
    echo "[https link] FAIL stage=run output=$version" >&2
    exit 1
    ;;
esac

echo "[https link] ok"

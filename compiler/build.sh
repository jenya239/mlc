#!/usr/bin/env bash
set -e

# Progress during modular compile/link (semantic → codegen → g++): stderr lines prefixed with "[mlc build]".
# Example: MLCC_BUILD_VERBOSE=1 compiler/build.sh
#
# Default: reuse compiler/out/mlcc when no compiler/**/*.mlc is newer than the binary.
# Cold start / stale: Ruby ModularCompiler, or mlcc codegen + build_bin.sh when mlcc exists.
# Force Ruby: MLCC_FORCE_RUBY=1

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

MAIN="$COMPILER_DIR/main.mlc"
BIN_OUT="$OUT_DIR/mlcc"

echo "[mlcc build] modular compile → ${BIN_OUT} (MAIN=${MAIN})" >&2

_verbose_lowercase="$(printf '%s' "${MLCC_BUILD_VERBOSE:-}" | tr '[:upper:]' '[:lower:]')"
if [[ -n "${MLCC_BUILD_VERBOSE:-}" ]] && [[ "${_verbose_lowercase}" != "0" ]] && [[ "${_verbose_lowercase}" != "false" ]] && [[ "${_verbose_lowercase}" != "no" ]]; then
  echo "compiler/build.sh: MLCC_BUILD_VERBOSE=${MLCC_BUILD_VERBOSE} (progress on stderr)" >&2
fi
unset _verbose_lowercase

mlcc_binary_is_fresh() {
  [ -x "$BIN_OUT" ] || return 1
  ! find "$COMPILER_DIR" -name '*.mlc' -newer "$BIN_OUT" -print -quit | grep -q .
}

build_via_ruby() {
  echo "[mlcc build] Ruby ModularCompiler" >&2
  bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
require "mlc/common/modular_compilation/modular_compiler"

compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: ARGV[0],
  out_dir: ARGV[1],
  root_dir: File.dirname(ARGV[0]),
  binary_name: "mlcc"
)
result = compiler.build
puts "Built: #{result[:binary]}"
' "$MAIN" "$OUT_DIR"
}

build_via_mlcc() {
  echo "[mlcc build] mlcc codegen + build_bin.sh" >&2
  "$BIN_OUT" "$MAIN" -o "$OUT_DIR"
  "$COMPILER_DIR/build_bin.sh" "$OUT_DIR" "$BIN_OUT"
}

if mlcc_binary_is_fresh; then
  echo "[mlcc build] ${BIN_OUT} up to date (skip)" >&2
elif [ "${MLCC_FORCE_RUBY:-0}" = "1" ] || [ ! -x "$BIN_OUT" ]; then
  build_via_ruby
elif [ "${MLCC_BOOTSTRAP:-0}" = "1" ]; then
  echo "[mlcc build] ${BIN_OUT} stale; skip main rebuild (bootstrap only)" >&2
else
  build_via_mlcc
fi

echo "[mlcc build] binary ready: ${BIN_OUT}" >&2

if [ "${MLCC_BOOTSTRAP:-0}" = "1" ]; then
  BS_DIR="$OUT_DIR/bootstrap"
  BS_BIN="$BS_DIR/mlcc_bootstrap"
  mkdir -p "$BS_DIR"
  echo "[mlcc build] bootstrap: mlcc codegen → ${BS_DIR}" >&2
  if ! "$BIN_OUT" "$MAIN" -o "$BS_DIR"; then
    echo "[mlcc build] bootstrap: mlcc codegen failed" >&2
    exit 1
  fi
  if ! ls "$BS_DIR"/*.cpp 1>/dev/null 2>&1; then
    echo "[mlcc build] bootstrap: no .cpp in ${BS_DIR}" >&2
    exit 1
  fi
  if ! "$COMPILER_DIR/build_bin.sh" "$BS_DIR" "$BS_BIN"; then
    echo "[mlcc build] bootstrap: build_bin.sh failed" >&2
    exit 1
  fi
  echo "[mlcc build] bootstrap: built ${BS_BIN}" >&2
fi

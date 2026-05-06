#!/usr/bin/env bash
set -e

# Progress during modular compile/link (semantic → codegen → g++): stderr lines prefixed with "[mlc build]".
# Example: MLCC_BUILD_VERBOSE=1 compiler/build.sh

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

echo "[mlcc build] binary ready: ${BIN_OUT}" >&2

if [ "${MLCC_BOOTSTRAP:-0}" = "1" ]; then
  BS_DIR="$OUT_DIR/bootstrap"
  mkdir -p "$BS_DIR"
  if (cd "$COMPILER_DIR" && "$BIN_OUT" main.mlc -o "$BS_DIR" 2>/dev/null); then
    if [ -d "$BS_DIR" ] && ls "$BS_DIR"/*.cpp 1>/dev/null 2>&1; then
      echo "Bootstrap: mlcc produced C++ output"
      RT_INC="$ROOT_DIR/runtime/include"
      RT_CPP="$ROOT_DIR/runtime/src/core/string.cpp $ROOT_DIR/runtime/src/io/io.cpp"
      OBJ_DIR="$BS_DIR/obj"
      mkdir -p "$OBJ_DIR"
      if (for cpp in "$BS_DIR"/*.cpp; do [ -f "$cpp" ] || continue; g++ -std=c++20 -O2 -I "$BS_DIR" -I "$RT_INC" -c "$cpp" -o "$OBJ_DIR/$(basename "$cpp" .cpp).o"; done && \
         for rcpp in $RT_CPP; do [ -f "$rcpp" ] && g++ -std=c++20 -O2 -I "$RT_INC" -c "$rcpp" -o "$OBJ_DIR/runtime_$(basename "$rcpp" .cpp).o"; done && \
         g++ -std=c++20 -O2 -o "$BS_DIR/mlcc_bootstrap" "$OBJ_DIR"/*.o); then
        echo "Bootstrap: built $BS_DIR/mlcc_bootstrap"
      else
        echo "Bootstrap: C++ compile failed"
      fi
    fi
  fi
fi

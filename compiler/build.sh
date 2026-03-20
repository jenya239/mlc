#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

MAIN="$COMPILER_DIR/main.mlc"
BIN_OUT="$OUT_DIR/mlcc"

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

BIN_OUT="$OUT_DIR/mlcc"

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

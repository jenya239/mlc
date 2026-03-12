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
  (cd "$COMPILER_DIR" && "$BIN_OUT" main.mlc 2>/dev/null) || true
  if [ -d "$OUT_DIR" ] && ls "$OUT_DIR"/*.cpp 1>/dev/null 2>&1; then
    echo "Bootstrap: mlcc produced C++ output"
  fi
fi

#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

COMBINED="$OUT_DIR/compiler.mlc"
CPP_OUT="$OUT_DIR/compiler.cpp"
BIN_OUT="$OUT_DIR/mlcc"

ruby -e '
files = %w[
  ast.mlc
  lexer.mlc
  parser/preds.mlc
  parser/types.mlc
  parser/exprs.mlc
  parser/stmts.mlc
  parser/decls.mlc
  checker/names.mlc
  checker/registry.mlc
  checker/infer.mlc
  codegen.mlc
  cpp_lexer.mlc
  main.mlc
]
src = files.map { |f| File.read(File.join(ARGV[0], f)) }.join("\n\n")
File.write(ARGV[1], src)
puts "Combined: #{src.lines.count} lines"
' "$COMPILER_DIR" "$COMBINED"

bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
src = File.read(ARGV[0])
cpp = MLC.to_cpp(src)
File.write(ARGV[1], cpp)
puts "Generated: #{cpp.lines.count} lines C++"
' "$COMBINED" "$CPP_OUT"

g++ -std=c++20 -I"$ROOT_DIR/runtime/include" -O2 -o "$BIN_OUT" "$CPP_OUT" \
  "$ROOT_DIR/runtime/src/core/string.cpp" \
  "$ROOT_DIR/runtime/src/io/io.cpp"
echo "Built: $BIN_OUT"

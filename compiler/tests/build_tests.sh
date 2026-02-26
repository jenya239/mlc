#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

COMBINED="$OUT_DIR/tests.mlc"
CPP_OUT="$OUT_DIR/tests.cpp"
BIN_OUT="$OUT_DIR/run_tests"

ruby -e '
files = %w[
  ast.mlc
  lexer.mlc
  cpp/ast.mlc
  cpp/lexer.mlc
  cpp/predicates.mlc
  cpp/parser.mlc
  parser/preds.mlc
  parser/types.mlc
  parser/exprs.mlc
  parser/stmts.mlc
  parser/decls.mlc
  checker/names.mlc
  checker/registry.mlc
  checker/infer.mlc
  codegen.mlc
  tests/test_runner.mlc
  tests/test_lexer.mlc
  tests/test_parser.mlc
  tests/test_checker.mlc
  tests/test_codegen.mlc
  tests/test_pipe_and_record_update.mlc
  tests/test_cpp_lexer.mlc
  tests/test_cpp_ast.mlc
  tests/tests_main.mlc
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

g++ -std=c++20 -I"$ROOT_DIR/runtime/include" -O2 -o "$BIN_OUT" "$CPP_OUT" "$ROOT_DIR/runtime/src/core/string.cpp" "$ROOT_DIR/runtime/src/io/io.cpp"
echo "Built: $BIN_OUT"
echo "Running tests..."
"$BIN_OUT"

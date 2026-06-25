#!/usr/bin/env bash
# Differential: Ruby CppAst.parse statement count vs mlcc cpp_parse declaration count.
set -e

COMPILER_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
FIXTURE_DIR="$COMPILER_DIR/tests/fixtures/cpp_parser"
TOOL_SOURCE="$COMPILER_DIR/tests/tools/cpp_parse_count.mlc"
TOOL_BUILD="$COMPILER_DIR/out/cpp_parse_count_build"
COUNT_BINARY="$TOOL_BUILD/cpp_parse_count"

mkdir -p "$TOOL_BUILD"
if [ ! -x "$COUNT_BINARY" ]; then
  bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: ARGV[0],
  out_dir: ARGV[1],
  root_dir: File.dirname(ARGV[0]),
  binary_name: "cpp_parse_count"
)
compiler.build
' "$TOOL_SOURCE" "$TOOL_BUILD"
fi

mismatch_count=0
fixture_count=0

for fixture_path in "$FIXTURE_DIR"/*.h; do
  [ -f "$fixture_path" ] || continue
  fixture_count=$((fixture_count + 1))
  ruby_count=$(bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "cpp_ast/index"
source_text = File.read(ARGV[0])
program = CppAst.parse(source_text)
statement_count = program.statements.count { |statement| !statement.is_a?(CppAst::Nodes::ErrorStatement) }
puts statement_count
' "$fixture_path")
  mlcc_count=$("$COUNT_BINARY" "$fixture_path")
  if [ "$ruby_count" != "$mlcc_count" ]; then
    echo "[cpp parser differential] mismatch $(basename "$fixture_path"): ruby=$ruby_count mlcc=$mlcc_count" >&2
    mismatch_count=$((mismatch_count + 1))
  fi
done

if [ "$fixture_count" -eq 0 ]; then
  echo "[cpp parser differential] FAIL: no fixtures in $FIXTURE_DIR" >&2
  exit 1
fi

if [ "$mismatch_count" -gt 0 ]; then
  echo "[cpp parser differential] FAIL: $mismatch_count mismatch(es)" >&2
  exit 1
fi

echo "[cpp parser differential] ok ($fixture_count fixtures)" >&2

#!/usr/bin/env bash
set -e

COMPILER_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$COMPILER_DIR/.." && pwd)"
OUT_DIR="${1:-$COMPILER_DIR/out}"
mkdir -p "$OUT_DIR"

MAIN="$COMPILER_DIR/main.mlc"
COMBINED="$OUT_DIR/compiler.mlc"
CPP_OUT="$OUT_DIR/compiler.cpp"
BIN_OUT="$OUT_DIR/mlcc"

# Resolve import { X } from "./file" starting from main.mlc,
# then compile to C++ and save the combined source for inspection.
bundle exec ruby -I"$ROOT_DIR/lib" -e '
require "mlc/common/index"
require "set"

main_file = ARGV[0]
combined_out = ARGV[1]
cpp_out = ARGV[2]

main_src = File.read(main_file)
main_dir = File.dirname(File.expand_path(main_file))
loaded   = Set.new([File.expand_path(main_file)])

combined = MLC.send(:inline_source_imports, main_src, main_dir, loaded)
File.write(combined_out, combined)
puts "Combined: #{combined.lines.count} lines"

cpp = MLC.to_cpp(combined)
File.write(cpp_out, cpp)
puts "Generated: #{cpp.lines.count} lines C++"
' "$MAIN" "$COMBINED" "$CPP_OUT"

g++ -std=c++20 -I"$ROOT_DIR/runtime/include" -O2 -o "$BIN_OUT" "$CPP_OUT" \
  "$ROOT_DIR/runtime/src/core/string.cpp" \
  "$ROOT_DIR/runtime/src/io/io.cpp"
echo "Built: $BIN_OUT"

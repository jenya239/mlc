#!/usr/bin/env bash
# Quick modular build test: minimal.mlc (1 module, ~5s)
set -e

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
bundle exec ruby -I"$ROOT_DIR/lib" -e "
require 'mlc/common/index'
require 'mlc/common/modular_compilation/modular_compiler'

compiler = MLC::Common::ModularCompilation::ModularCompiler.new(
  entry_path: 'misc/examples/minimal.mlc',
  out_dir: 'misc/examples/out_minimal',
  root_dir: 'misc/examples',
  binary_name: 'minimal'
)
result = compiler.build
system(result[:binary])
raise 'Expected exit 42' unless \$?.exitstatus == 42
puts 'OK: minimal build and run'
"

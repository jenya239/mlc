#!/usr/bin/env ruby
# frozen_string_literal: true

# Ruby checker path for fuzz differential: parse + semantic transform (no codegen).
# Exit 0 = accept, 1 = checker/parse error, 2 = unexpected failure.

require "mlc/common/index"

path = ARGV.fetch(0)
source = File.read(path)

begin
  abstract_syntax_tree = MLC.parse(source, filename: path)
  MLC.transform_to_core(abstract_syntax_tree)
  exit 0
rescue MLC::CompileError, MLC::ParseError
  exit 1
rescue StandardError => error
  warn "unexpected: #{error.class}: #{error.message}"
  exit 2
end

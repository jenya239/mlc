#!/usr/bin/env ruby
# frozen_string_literal: true

# Ruby checker error count for fuzz differential (parse + transform, no codegen).
# Prints error count to stdout: 0 = ok, N = checker errors.
# Exit 0 always on success path; exit 2 on parse error; exit 3 on unexpected failure.

require "mlc/common/index"

path = ARGV.fetch(0)
source = File.read(path)

begin
  abstract_syntax_tree = MLC.parse(source, filename: path)
  MLC.transform_to_core(abstract_syntax_tree)
  print 0
rescue MLC::ParseError
  warn "parse error"
  exit 2
rescue MLC::CompileError
  print 1
rescue StandardError => error
  warn "unexpected: #{error.class}: #{error.message}"
  exit 3
end

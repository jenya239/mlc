#!/usr/bin/env ruby
# frozen_string_literal: true

# Wrap AST module in Source module

file = 'lib/mlc/source/ast/nodes.rb'
content = File.read(file)
lines = content.split("\n")

output = []
in_ast_module = false
ast_module_line = nil

lines.each_with_index do |line, idx|
  if line.match?(/^\s*module AST/)
    # Add Source module before AST
    output << "  module Source"
    in_ast_module = true
    ast_module_line = idx
    # Add AST line with extra indentation
    output << "  " + line
  elsif in_ast_module && idx > ast_module_line
    # Inside AST module - add 2 spaces
    if line.match?(/^\s*end\s*$/) && line.start_with?('  end')
      # This is the closing end for AST
      output << "  " + line
      # Add closing end for Source
      output << "  end"
      in_ast_module = false
    else
      # Regular content - add 2 spaces
      output << (line.empty? ? line : "  " + line)
    end
  else
    # Outside AST module
    output << line
  end
end

File.write(file, output.join("\n") + "\n")
puts "✅ Wrapped module AST in module Source"

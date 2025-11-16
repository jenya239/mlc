#!/usr/bin/env ruby
# Fix engine_test.rb namespace and indentation

file = "test/mlc/representations/semantic/gen/engine_test.rb"
lines = File.readlines(file)
total = lines.length

output = []
lines.each_with_index do |line, idx|
  # Lines 0-5: unchanged (comments, requires)
  if idx < 6
    output << line
  # Line 6-7: replace with new namespace
  elsif idx == 6
    output << "module MLC\n"
    output << "  module Representations\n"
    output << "    module Semantic\n"
    output << "      module Gen\n"
  elsif idx == 7
    # skip old "module SemanticGen"
    next
  # Lines 8 to total-3: add 4 spaces
  elsif idx >= 8 && idx < total - 2
    if line.strip.empty? || line.start_with?("#")
      output << line
    else
      output << ("    " + line)
    end
  # Last 2 lines: replace closing ends
  elsif idx == total - 2
    output << "      end\n"
    output << "    end\n"
    output << "  end\n"
    output << "end\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Fixed #{file}"

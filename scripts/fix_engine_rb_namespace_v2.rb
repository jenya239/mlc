#!/usr/bin/env ruby
# Fix engine.rb namespace: MLC::SemanticGen → MLC::Representations::Semantic::Gen (v2)

file = "lib/mlc/representations/semantic/gen/engine.rb"
lines = File.readlines(file)
total = lines.length

output = []
lines.each_with_index do |line, idx|
  # Lines 0-37: unchanged (comments, requires)
  if idx < 38
    output << line
  # Lines 38-39: replace namespace declaration
  elsif idx == 38
    output << "module MLC\n"
    output << "  module Representations\n"
    output << "    module Semantic\n"
    output << "      module Gen\n"
  elsif idx == 39
    # skip old "module SemanticGen"
    next
  # Lines 40 to total-4: add 4 spaces indentation
  elsif idx >= 40 && idx < total - 3
    if line.strip.empty?
      output << line
    elsif line.lstrip.start_with?("#")
      # Comments: add 4 spaces
      output << ("    " + line)
    else
      # Regular code: add 4 spaces
      output << ("    " + line)
    end
  # Last 3 lines (127, 128, 129): replace with 5 closing ends
  elsif idx == total - 3
    output << "        end # class Engine\n"
    output << "      end   # module Gen\n"
    output << "    end     # module Semantic\n"
    output << "  end       # module Representations\n"
    output << "end         # module MLC\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Fixed #{file} - updated namespace and indentation (v2)"

#!/usr/bin/env ruby
# frozen_string_literal: true

# Add 4-space indentation to engine.rb content after namespace change

file = "lib/mlc/representations/semantic/gen/engine.rb"
lines = File.readlines(file)

output = []
lines.each_with_index do |line, idx|
  # Lines 0-43: keep as-is (already has correct namespace)
  if idx < 44
    output << line
  # Lines 44 to length-3: add 4 spaces to non-empty lines
  elsif idx < lines.length - 2
    output << if line.strip.empty?
                line
              else
                ("    " + line)
              end
  # Last 2 lines: replace with 4 closing ends
  elsif idx == lines.length - 2
    output << "        end   # class Engine\n"
    output << "      end     # module Gen\n"
    output << "    end       # module Semantic\n"
    output << "  end         # module Representations\n"
    output << "end           # module MLC\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Added indentation to #{file}"

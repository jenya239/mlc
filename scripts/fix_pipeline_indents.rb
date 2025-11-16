#!/usr/bin/env ruby
# Add 4-space indentation to pipeline.rb after namespace change

file = "lib/mlc/representations/semantic/gen/pipeline.rb"
lines = File.readlines(file)

output = []
lines.each_with_index do |line, idx|
  # Lines 0-18: keep as-is (requires + namespace start)
  if idx < 19
    output << line
  # Lines 19 to length-5: add 4 spaces to non-empty lines
  elsif idx < lines.length - 4
    if line.strip.empty?
      output << line
    else
      output << ("    " + line)
    end
  # Last 4 lines: replace with 4 closing ends
  elsif idx == lines.length - 4
    output << "        end # class Pipeline\n"
    output << "      end   # module Gen\n"
    output << "    end     # module Semantic\n"
    output << "  end       # module Representations\n"
    output << "end         # module MLC\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Added indentation to #{file}"

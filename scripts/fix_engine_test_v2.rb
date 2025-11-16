#!/usr/bin/env ruby
# Fix engine_test.rb - add indentation to content (v2, careful version)

file = "test/mlc/representations/semantic/gen/engine_test.rb"
lines = File.readlines(file)
total = lines.length

output = []
lines.each_with_index do |line, idx|
  # Lines 0-10: already fixed, keep as is
  if idx < 10
    output << line
  # Lines 10 to total-3: add 2 spaces to non-empty, non-comment lines
  elsif idx >= 10 && idx < total - 2
    if line.strip.empty?
      output << line
    elsif line.lstrip.start_with?("#")
      # Comment: add 2 spaces before the comment
      leading = line[/^\s*/]
      output << ("  " + line)
    else
      # Regular code: add 2 spaces
      output << ("  " + line)
    end
  # Last 2 lines: replace with 4 closing ends
  elsif idx == total - 2
    output << "        end\n"
    output << "      end\n"
    output << "    end\n"
    output << "  end\n"
    output << "end\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Fixed #{file} - added indentation to #{total - 12} lines"

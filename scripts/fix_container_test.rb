#!/usr/bin/env ruby
# Fix container_integration_test.rb namespace and indentation

file = "test/mlc/representations/semantic/gen/services/container_integration_test.rb"
lines = File.readlines(file)

output = []
lines.each_with_index do |line, idx|
  # Lines 0-10 already correct (including new namespace declaration)
  if idx < 11
    output << line
  # Lines 11-102 need 4 additional spaces
  elsif idx >= 11 && idx < 103
    if line.strip.empty? || line.start_with?("#")
      output << line
    else
      output << ("    " + line)
    end
  # Lines 103-106: replace closing ends
  elsif idx == 103
    output << "          end\n"
    output << "        end\n"
    output << "      end\n"
    output << "    end\n"
    output << "  end\n"
    output << "end\n"
    break
  end
end

File.write(file, output.join)
puts "✓ Fixed #{file}"

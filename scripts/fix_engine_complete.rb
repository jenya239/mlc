#!/usr/bin/env ruby
# Complete fix for engine.rb: namespace + internal references

file = "lib/mlc/representations/semantic/gen/engine.rb"
content = File.read(file)

# Step 1: Replace namespace declaration (lines 38-39)
content.gsub!(/^module MLC\n  module SemanticGen\n    # Engine/, <<~MODULES.chomp
module MLC
  module Representations
    module Semantic
      module Gen
        # Engine
MODULES
)

# Step 2: Replace all MLC::SemanticGen::Rules::* with Rules::*
content.gsub!(/MLC::SemanticGen::Rules::/, 'Rules::')

# Step 3: Add indentation (4 spaces) to class content
# Split by lines and process
lines = content.split("\n")
output = []
in_class = false
class_indent_added = false

lines.each_with_index do |line, idx|
  # Detect when we enter the class Engine
  if line.match(/^\s{8}class Engine/)
    in_class = true
    output << line
    next
  end

  # Add indentation to class content (after "class Engine" line)
  if in_class && !class_indent_added
    # Check if this line is already indented properly
    if line.match(/^\s{10}attr_reader/)
      # Line is already indented, class_indent was already added
      class_indent_added = true
      output << line
    elsif line.strip.empty?
      output << line
    else
      # Add 4 spaces
      output << "    #{line}"
    end
  elsif in_class && class_indent_added
    # Continue with existing indentation
    output << line
  else
    output << line
  end

  # Detect end of class
  if in_class && line.match(/^\s{8}end\s*$/) && !line.match(/^end/)
    in_class = false
  end
end

content = output.join("\n")

# Step 4: Fix closing ends (replace last 2 lines)
lines = content.split("\n")
# Remove last 2 lines (old closing ends)
lines = lines[0..-3]
# Add 4 new closing ends for 4 modules
lines << "      end # class Engine"
lines << "    end   # module Gen"
lines << "  end     # module Semantic"
lines << "end       # module Representations"

content = lines.join("\n") + "\n"

# Write back
File.write(file, content)

puts "✓ Fixed #{file}"
puts "  - Updated namespace: MLC::SemanticGen → MLC::Representations::Semantic::Gen"
puts "  - Replaced internal references: MLC::SemanticGen::Rules::* → Rules::*"
puts "  - Updated indentation and closing ends"

#!/usr/bin/env ruby
# frozen_string_literal: true

# Script to migrate MLC::Rules namespace to proper path-matching namespace
# File: lib/mlc/representations/semantic/gen/services/rule_engine.rb
# Old: module MLC\n  module Rules
# New: module MLC\n  module Representations\n    module Semantic\n      module Gen\n        module Services

rule_engine_file = "lib/mlc/representations/semantic/gen/services/rule_engine.rb"

# Read file
lines = File.readlines(rule_engine_file)

# Replace namespace
output_lines = []
i = 0
while i < lines.length
  line = lines[i]

  # Check for module MLC at line 3
  if i == 2 && line.strip == "module MLC"
    # Replace the old 2-level namespace with 5-level namespace
    output_lines << "module MLC\n"
    output_lines << "  module Representations\n"
    output_lines << "    module Semantic\n"
    output_lines << "      module Gen\n"
    output_lines << "        module Services\n"

    # Skip the next line (module Rules)
    i += 2
    next
  elsif i >= 3 && i < lines.length - 2
    # Add 6 spaces to content lines (3 to 45)
    if line.strip.empty?
      output_lines << line
    elsif line.start_with?("#")
      output_lines << "      " + line
    else
      output_lines << "      " + line
    end
    i += 1
  elsif i == lines.length - 2
    # Replace closing ends
    output_lines << "        end\n"
    output_lines << "      end\n"
    output_lines << "    end\n"
    output_lines << "  end\n"
    output_lines << "end\n"
    i += 2
  else
    output_lines << line
    i += 1
  end
end

# Write back
File.write(rule_engine_file, output_lines.join)

puts "✓ Updated #{rule_engine_file}"
puts "  module MLC::Rules → module MLC::Representations::Semantic::Gen::Services"

# Now update references in other files
reference_files = [
  "lib/mlc/representations/semantic/gen/engine.rb",
  "test/mlc/rules_rule_engine_test.rb"
]

reference_files.each do |file|
  next unless File.exist?(file)

  content = File.read(file)
  updated = content.gsub(/MLC::Rules::RuleEngine/,
                         "MLC::Representations::Semantic::Gen::Services::RuleEngine")

  if content != updated
    File.write(file, updated)
    puts "✓ Updated references in #{file}"
  end
end

puts "\nDone! Files processed:"
puts "  - rule_engine.rb (namespace changed)"
puts "  - engine.rb (references updated)"
puts "  - rules_rule_engine_test.rb (references updated)"

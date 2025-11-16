#!/usr/bin/env ruby
# Fix namespace for all Rules files:
# MLC::SemanticGen::Rules → MLC::Representations::Semantic::Gen::Rules

require 'fileutils'

# Find all rule files
files = Dir.glob("lib/mlc/representations/semantic/gen/rules/**/*_rule.rb")

puts "Found #{files.size} rule files"

files.each do |file|
  content = File.read(file)
  original = content.dup

  # For files in statements/ subdirectory (has 4 module levels)
  if file.include?('/statements/')
    # Replace namespace (4 modules deep)
    content.gsub!(
      /module MLC\n  module SemanticGen\n    module Rules\n      module Statements\n/,
      "module MLC\n  module Representations\n    module Semantic\n      module Gen\n        module Rules\n          module Statements\n"
    )

    # Update closing ends (from 4 to 6)
    content.gsub!(
      /^      end\n    end\n  end\nend\n\z/,
      "          end\n        end\n      end\n    end\n  end\nend\n"
    )

    # Add 4 spaces to content between namespace and closing
    lines = content.split("\n")
    output = []
    in_namespace = false
    namespace_line = 0

    lines.each_with_index do |line, idx|
      if line.match?(/^\s*module Statements/)
        in_namespace = true
        namespace_line = idx
        output << line
      elsif in_namespace && idx > namespace_line && !line.match?(/^\s{10,}end/)
        # Add 4 spaces if not already indented enough
        if line.strip.empty?
          output << line
        else
          output << ("    " + line)
        end
      else
        output << line
      end
    end
    content = output.join("\n")

  else
    # For regular rule files (3 modules deep)
    # Replace namespace
    content.gsub!(
      /module MLC\n  module SemanticGen\n    module Rules\n/,
      "module MLC\n  module Representations\n    module Semantic\n      module Gen\n        module Rules\n"
    )

    # Update closing ends (from 3 to 5)
    content.gsub!(
      /^    end\n  end\nend\n\z/,
      "        end\n      end\n    end\n  end\nend\n"
    )

    # Add 4 spaces to content between namespace and closing
    lines = content.split("\n")
    output = []
    in_namespace = false
    namespace_line = 0

    lines.each_with_index do |line, idx|
      if line.match?(/^\s*module Rules/)
        in_namespace = true
        namespace_line = idx
        output << line
      elsif in_namespace && idx > namespace_line && !line.match?(/^\s{8,}end/)
        # Add 4 spaces if not already indented enough
        if line.strip.empty?
          output << line
        else
          output << ("    " + line)
        end
      else
        output << line
      end
    end
    content = output.join("\n")
  end

  if content != original
    File.write(file, content)
    puts "✓ Updated #{file}"
  else
    puts "  Skipped #{file} (no changes)"
  end
end

puts "\nDone!"

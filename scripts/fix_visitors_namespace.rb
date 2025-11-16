#!/usr/bin/env ruby
# Fix Visitors namespace: MLC::SemanticGen → MLC::Representations::Semantic::Gen

files = Dir.glob("lib/mlc/representations/semantic/gen/visitors/**/*.rb")

puts "Found #{files.size} visitor files"

files.each do |file|
  content = File.read(file)
  original = content.dup

  # Replace namespace (3 modules to 5 modules)
  content.gsub!(
    /module MLC\n  module SemanticGen\n    module Visitors\n/,
    "module MLC\n  module Representations\n    module Semantic\n      module Gen\n        module Visitors\n"
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
    if line.match?(/^\s*module Visitors/)
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

  if content != original
    File.write(file, content)
    puts "✓ Updated #{file}"
  else
    puts "  Skipped #{file} (no changes)"
  end
end

puts "\nDone!"

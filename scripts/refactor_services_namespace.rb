#!/usr/bin/env ruby
# frozen_string_literal: true

# Надёжный refactoring MLC::Services → MLC::Representations::Semantic::Gen::Services
# Корректно обрабатывает отступы для всех patterns

class ServicesNamespaceRefactor
  PATTERN1_OLD = ["module MLC", "  module Services"]
  PATTERN1_NEW = [
    "module MLC",
    "  module Representations",
    "    module Semantic",
    "      module Gen",
    "        module Services"
  ]

  PATTERN2_OLD = ["module MLC", "  module SemanticGen", "    module Services"]
  PATTERN2_NEW = [
    "module MLC",
    "  module Representations",
    "    module Semantic",
      "      module Gen",
    "        module Services"
  ]

  def initialize(file_path, dry_run: false)
    @file_path = file_path
    @dry_run = dry_run
    @lines = File.readlines(file_path)
  end

  def process!
    pattern = detect_pattern
    return nil unless pattern

    case pattern
    when :pattern1
      process_pattern1
    when :pattern2
      process_pattern2
    end

    write_result unless @dry_run
    pattern
  end

  private

  def detect_pattern
    # Check for Pattern 2 first (more specific): module MLC\n  module SemanticGen\n    module Services
    if @lines[0..50].join.include?("module MLC\n") &&
       @lines[0..50].join.include?("  module SemanticGen\n")
      return :pattern2
    end

    # Check for Pattern 1: module MLC\n  module Services
    if @lines[0..50].join.include?("module MLC\n") &&
       @lines[0..50].join.include?("  module Services\n")
      return :pattern1
    end

    nil
  end

  def process_pattern1
    result = []
    i = 0
    indent_added = 0

    while i < @lines.size
      line = @lines[i]

      if line == "module MLC\n" && @lines[i + 1] == "  module Services\n"
        # Replace with new namespace
        PATTERN1_NEW.each { |l| result << "#{l}\n" }
        i += 2
        indent_added = 6  # Added 3 extra module levels = 6 spaces
      elsif line == "  end\n" && @lines[i + 1] == "end\n" && i + 1 == @lines.size - 1
        # This is the end for old module Services - skip it (we'll add proper ends at the last end)
        i += 1
      elsif line == "end\n" && i == @lines.size - 1
        # Last end - add extra closing ends
        result << "        end\n"
        result << "      end\n"
        result << "    end\n"
        result << "  end\n"
        result << "end\n"
        i += 1
      else
        # Regular line - add indent if needed
        if indent_added > 0 && !line.strip.empty? && line !~ /^#/
          result << (" " * indent_added + line)
        else
          result << line
        end
        i += 1
      end
    end

    @lines = result
  end

  def process_pattern2
    result = []
    i = 0
    indent_added = 0

    while i < @lines.size
      line = @lines[i]

      if line == "module MLC\n" && @lines[i + 1] == "  module SemanticGen\n"
        # Replace with new namespace
        PATTERN2_NEW.each { |l| result << "#{l}\n" }
        i += 3  # Skip MLC, SemanticGen, Services
        indent_added = 4  # Added 2 extra module levels (Representations, Semantic) = 4 spaces
      elsif line == "    end\n" && @lines[i + 1] == "  end\n" && @lines[i + 2] == "end\n" && i + 2 == @lines.size - 1
        # These are closing ends for Services, SemanticGen - skip them (we'll add proper ends at last end)
        i += 1
      elsif line == "  end\n" && @lines[i + 1] == "end\n" && i + 1 == @lines.size - 1
        # This is end for SemanticGen - skip it
        i += 1
      elsif line == "end\n" && i == @lines.size - 1
        # Last end - add closing ends for all modules
        result << "        end\n"   # Services
        result << "      end\n"     # Gen
        result << "    end\n"       # Semantic
        result << "  end\n"         # Representations
        result << "end\n"           # MLC
        i += 1
      else
        # Regular line - add indent if needed
        if indent_added > 0 && !line.strip.empty? && line !~ /^#/
          result << (" " * indent_added + line)
        else
          result << line
        end
        i += 1
      end
    end

    @lines = result
  end

  def write_result
    File.write(@file_path, @lines.join)
  end
end

# Main execution
if __FILE__ == $PROGRAM_NAME
  dry_run = ARGV.include?('--dry-run')
  files = ARGV.grep_v(/^--/)

  if files.empty?
    # Process all service files
    files = Dir.glob('lib/mlc/representations/semantic/gen/services/**/*.rb')
  end

  results = { pattern1: [], pattern2: [], skipped: [] }

  files.each do |file|
    refactor = ServicesNamespaceRefactor.new(file, dry_run: dry_run)
    pattern = refactor.process!

    if pattern
      results[pattern] << file
      puts "#{dry_run ? '[DRY RUN] ' : ''}Processed (#{pattern}): #{file}"
    else
      results[:skipped] << file
      puts "Skipped (no pattern): #{file}"
    end
  end

  puts "\n=== Summary ==="
  puts "Pattern 1 (direct Services): #{results[:pattern1].size}"
  puts "Pattern 2 (SemanticGen): #{results[:pattern2].size}"
  puts "Skipped: #{results[:skipped].size}"
  puts "\nRun with --dry-run to preview changes"
end

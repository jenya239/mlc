#!/usr/bin/env ruby
# frozen_string_literal: true

# Universal namespace refactoring tool using Prism AST
# Usage: ruby namespace_refactor.rb "MLC::AST" "MLC::Source::AST"

require 'prism'
require 'fileutils'

class NamespaceRefactor
  def initialize(old_namespace, new_namespace, dry_run: false)
    @old_namespace = old_namespace
    @new_namespace = new_namespace
    @old_parts = old_namespace.split('::')
    @new_parts = new_namespace.split('::')
    @dry_run = dry_run
    @stats = Hash.new(0)
  end

  def run
    puts "🔍 Searching for '#{@old_namespace}' references..."

    # Find all Ruby files
    files = Dir.glob('lib/**/*.rb') + Dir.glob('test/**/*.rb')

    files.each do |file|
      process_file(file)
    end

    print_stats
  end

  private

  def process_file(file)
    content = File.read(file)
    result = Prism.parse(content)

    # Collect all replacements with their locations
    replacements = []

    # Visit all nodes to find constant references
    visit_node(result.value, content, replacements)

    return if replacements.empty?

    # Apply replacements in reverse order (to preserve offsets)
    new_content = content.dup
    replacements.sort_by { |r| -r[:offset] }.each do |repl|
      new_content[repl[:offset], repl[:length]] = repl[:new_text]
    end

    if new_content != content
      @stats[:files_modified] += 1
      @stats[:replacements] += replacements.size

      if @dry_run
        puts "📝 Would modify #{file} (#{replacements.size} replacements)"
      else
        File.write(file, new_content)
        puts "✅ Modified #{file} (#{replacements.size} replacements)"
      end
    end
  end

  def visit_node(node, source, replacements)
    return unless node

    case node
    when Prism::ConstantReadNode
      # Simple constant like AST
      check_constant_read(node, source, replacements)
    when Prism::ConstantPathNode
      # Path like MLC::AST::Program
      check_constant_path(node, source, replacements)
    when Prism::ModuleNode, Prism::ClassNode
      # Module/class definitions
      check_definition(node, source, replacements)
    end

    # Recursively visit child nodes
    node.compact_child_nodes.each do |child|
      visit_node(child, source, replacements)
    end
  end

  def check_constant_read(node, source, replacements)
    # Check if this is the first part of our old namespace
    return unless node.name.to_s == @old_parts.first

    # Need to check if this is actually our namespace
    # This is conservative - only match if it's exactly our namespace
  end

  def check_constant_path(node, source, replacements)
    # Extract the full constant path
    path = extract_constant_path(node)
    return unless path

    # Check if it starts with our old namespace
    if path == @old_namespace
      # Exact match - replace entire thing
      location = node.location
      replacements << {
        offset: location.start_offset,
        length: location.length,
        new_text: @new_namespace
      }
    elsif path.start_with?(@old_namespace + '::')
      # Path starts with old namespace - replace prefix
      rest = path[@old_namespace.length..]
      location = node.location
      replacements << {
        offset: location.start_offset,
        length: location.length,
        new_text: @new_namespace + rest
      }
    end
  end

  def check_definition(node, source, replacements)
    # For module/class definitions, check constant_path
    if node.constant_path
      check_constant_path(node.constant_path, source, replacements)
    end
  end

  def extract_constant_path(node)
    parts = []
    current = node

    loop do
      case current
      when Prism::ConstantPathNode
        if current.child.is_a?(Prism::ConstantReadNode)
          parts.unshift(current.child.name.to_s)
        end
        current = current.parent
      when Prism::ConstantReadNode
        parts.unshift(current.name.to_s)
        break
      else
        break
      end
    end

    parts.empty? ? nil : parts.join('::')
  end

  def print_stats
    puts "\n" + "=" * 60
    puts "📊 Refactoring Statistics"
    puts "=" * 60
    puts "Old namespace: #{@old_namespace}"
    puts "New namespace: #{@new_namespace}"
    puts "Files modified: #{@stats[:files_modified]}"
    puts "Total replacements: #{@stats[:replacements]}"
    puts "Mode: #{@dry_run ? 'DRY RUN (no changes made)' : 'APPLIED'}"
    puts "=" * 60
  end
end

# CLI
if ARGV.size < 2
  puts "Usage: #{$0} OLD_NAMESPACE NEW_NAMESPACE [--dry-run]"
  puts "Example: #{$0} 'MLC::AST' 'MLC::Source::AST'"
  exit 1
end

old_ns = ARGV[0]
new_ns = ARGV[1]
dry_run = ARGV.include?('--dry-run')

refactor = NamespaceRefactor.new(old_ns, new_ns, dry_run: dry_run)
refactor.run

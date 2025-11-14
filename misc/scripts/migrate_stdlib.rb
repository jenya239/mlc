#!/usr/bin/env ruby
# frozen_string_literal: true

# Script to migrate stdlib files to new categorized structure with metadata

STDLIB_MIGRATION = {
  # Core - pure MLC
  'result.mlc' => { category: 'core', type: 'pure', module: 'std/core/result' },

  # Collections
  'array.mlc' => { category: 'collections', type: 'mixed', module: 'std/collections/array', namespace: 'mlc::collections' },

  # Text
  'string.mlc' => { category: 'text', type: 'extern', module: 'std/text/string', namespace: 'mlc' },
  'conv.mlc' => { category: 'text', type: 'extern', module: 'std/text/conv', namespace: 'mlc' },

  # IO
  'io.mlc' => { category: 'io', type: 'extern', module: 'std/io/io', namespace: 'mlc::io' },
  'file.mlc' => { category: 'io', type: 'extern', module: 'std/io/file', namespace: 'mlc::io' },

  # Data
  'json.mlc' => { category: 'data', type: 'extern', module: 'std/data/json', namespace: 'mlc::json' },

  # Math
  'math.mlc' => { category: 'math', type: 'extern', module: 'std/math/math', namespace: 'mlc::math' },

  # Graphics
  'graphics.mlc' => { category: 'graphics', type: 'extern', module: 'std/graphics/graphics', namespace: 'mlc::graphics' }
}

OLD_STDLIB_DIR = File.expand_path('../lib/mlc/stdlib', __dir__)
NEW_STDLIB_DIR = File.expand_path('../lib/mlc/common/stdlib', __dir__)

def add_metadata(content, info)
  metadata = "//! @module #{info[:module]}\n"
  metadata += "//! @category #{info[:category]}\n"
  metadata += "//! @type #{info[:type]}\n"
  metadata += "//! @namespace #{info[:namespace]}\n" if info[:namespace]
  metadata += "\n#{content}"
  metadata
end

STDLIB_MIGRATION.each do |filename, info|
  old_path = File.join(OLD_STDLIB_DIR, filename)
  new_path = File.join(NEW_STDLIB_DIR, info[:category], filename)

  unless File.exist?(old_path)
    puts "⚠️  Skip #{filename}: source file not found"
    next
  end

  if File.exist?(new_path)
    puts "⏭️  Skip #{filename}: already exists in new location"
    next
  end

  content = File.read(old_path)
  new_content = add_metadata(content, info)

  File.write(new_path, new_content)
  puts "✅ #{filename} → #{info[:category]}/#{filename}"
end

puts "\n✨ Migration complete!"

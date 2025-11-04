#!/usr/bin/env ruby
# frozen_string_literal: true

require_relative "lib/mlc"
require "json"

# Test Phase 24-A: Metadata generation

source = <<~MLC
  export fn add(a: i32, b: i32) -> i32 = a + b
  export fn multiply(a: i32, b: i32) -> i32 = a * b

  export type Vec2 = { x: f32, y: f32 }
MLC

puts "=== Testing Metadata Generation ==="
puts

begin
  result = MLC.to_hpp_cpp(source)

  puts "✅ Compilation successful!"
  puts
  puts "--- Generated Metadata ---"
  puts JSON.pretty_generate(result[:metadata])
  puts
  puts "--- Header (first 30 lines) ---"
  puts result[:header].lines.first(30).join

rescue => e
  puts "❌ Error: #{e.message}"
  puts e.backtrace.first(10).join("\n")
  exit 1
end

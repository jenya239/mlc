#!/usr/bin/env ruby
# frozen_string_literal: true

require_relative "lib/mlc"

source = <<~MLC
  export fn add(a: i32, b: i32) -> i32 = a + b
  export fn multiply(a: i32, b: i32) -> i32 = a * b

  export type Vec2 = { x: f32, y: f32 }
MLC

puts "=== Debugging HighIR Items ==="
puts

begin
  ast = MLC.parse(source)
  core_ir, type_registry, function_registry = MLC.transform_to_core_with_registry(ast)

  puts "Module name: #{core_ir.name.inspect}"
  puts "Number of items: #{core_ir.items.length}"
  puts

  core_ir.items.each_with_index do |item, idx|
    puts "Item #{idx}: #{item.class.name}"
    if item.is_a?(MLC::HighIR::Func)
      puts "  Name: #{item.name}"
      puts "  Exported: #{item.exported.inspect}"
      puts "  External: #{item.external.inspect}"
    elsif item.is_a?(MLC::HighIR::TypeDecl)
      puts "  Name: #{item.name}"
      puts "  Exported: #{item.exported.inspect}"
      puts "  Type: #{item.type.class.name}"
    end
    puts
  end

rescue => e
  puts "❌ Error: #{e.message}"
  puts e.backtrace.first(10).join("\n")
  exit 1
end

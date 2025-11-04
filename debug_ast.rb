#!/usr/bin/env ruby
# frozen_string_literal: true

require_relative "lib/mlc"

source = <<~MLC
  export fn add(a: i32, b: i32) -> i32 = a + b
  export fn multiply(a: i32, b: i32) -> i32 = a * b

  export type Vec2 = { x: f32, y: f32 }
MLC

puts "=== Debugging AST ==="
puts

begin
  ast = MLC.parse(source)

  puts "AST class: #{ast.class.name}"
  puts "Number of declarations: #{ast.declarations.length}"
  puts

  ast.declarations.each_with_index do |decl, idx|
    puts "Declaration #{idx}: #{decl.class.name}"
    if decl.is_a?(MLC::AST::FuncDecl)
      puts "  Name: #{decl.name}"
      puts "  Exported: #{decl.exported.inspect}"
      puts "  External: #{decl.external.inspect}"
    elsif decl.is_a?(MLC::AST::TypeDecl)
      puts "  Name: #{decl.name}"
      puts "  Exported: #{decl.exported.inspect}"
    end
    puts
  end

rescue => e
  puts "❌ Error: #{e.message}"
  puts e.backtrace.first(10).join("\n")
  exit 1
end

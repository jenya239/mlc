#!/usr/bin/env ruby
# frozen_string_literal: true

# MLC Language Demo - For Loops
# Demonstrates for loop support in MLC (2025-10-17)

require_relative "../lib/mlc"

puts "=" * 70
puts "  MLC LANGUAGE DEMO - For Loops"
puts "=" * 70
puts

# Example 1: Simple for loop over array literal
puts "Example 1: Simple for loop over array literal"
puts "-" * 70

mlc_source_1 = <<~MLC
  fn test_simple() -> i32 =
    for x in [1, 2, 3] do
      x
MLC

puts "MLC Source:"
puts mlc_source_1
puts

begin
  cpp_code = MLC.to_cpp(mlc_source_1)
  puts "Generated C++:"
  puts cpp_code
  puts
  puts "Note: Generates C++11 range-based for loop"
  puts "✅ SUCCESS!"
rescue => e
  puts "❌ ERROR: #{e.message}"
end

puts
puts "=" * 70
puts

# Example 2: For loop with let-bound array
puts "Example 2: For loop with let-bound array"
puts "-" * 70

mlc_source_2 = <<~MLC
  fn test_with_var() -> i32 =
    let arr = [10, 20, 30];
    for x in arr do
      x
MLC

puts "MLC Source:"
puts mlc_source_2
puts

begin
  cpp_code = MLC.to_cpp(mlc_source_2)
  puts "Generated C++:"
  puts cpp_code
  puts
  puts "✅ SUCCESS!"
rescue => e
  puts "❌ ERROR: #{e.message}"
end

puts
puts "=" * 70
puts

# Example 3: For loop with expression in body
puts "Example 3: For loop with expression in body"
puts "-" * 70

mlc_source_3 = <<~MLC
  fn test_expr() -> i32 =
    for i in [1, 2, 3, 4, 5] do
      i + i
MLC

puts "MLC Source:"
puts mlc_source_3
puts

begin
  cpp_code = MLC.to_cpp(mlc_source_3)
  puts "Generated C++:"
  puts cpp_code
  puts
  puts "✅ SUCCESS!"
rescue => e
  puts "❌ ERROR: #{e.message}"
end

puts
puts "=" * 70
puts

# Example 4: For loop with larger array
puts "Example 4: For loop with larger array"
puts "-" * 70

mlc_source_4 = <<~MLC
  fn process_array() -> i32 =
    for num in [5, 10, 15, 20, 25, 30] do
      num
MLC

puts "MLC Source:"
puts mlc_source_4
puts

begin
  cpp_code = MLC.to_cpp(mlc_source_4)
  puts "Generated C++:"
  puts cpp_code
  puts
  puts "✅ SUCCESS!"
rescue => e
  puts "❌ ERROR: #{e.message}"
end

puts
puts "=" * 70
puts
puts "HOW IT WORKS:"
puts "=" * 70
puts
puts "MLC For Loop Translation:"
puts "  MLC:  for x in array do body"
puts "  C++:     for (type x : container) { body; }"
puts
puts "Implementation Details:"
puts "1. Parser: parse_for_loop() already existed"
puts "2. AST: ForLoop node with var_name, iterable, body"
puts "3. CoreIR: ForStmt with type inference for loop variable"
puts "4. Type Tracking: Loop variable type saved in @var_types"
puts "5. C++ Lowering: Generates RangeForStatement (C++11 range-based for) без лямбда-IIFE"
puts "6. Variable Type: Element type extracted from iterable ArrayType"
puts
puts "Type Inference:"
puts "- Iterable type is analyzed (e.g., ArrayType with element type i32)"
puts "- Loop variable gets element type (i32 → int in C++)"
puts "- Loop variable type saved for body transformation"
puts "- Body can reference loop variable with correct type"
puts
puts "C++ Code Generation:"
puts "- Creates ForLoopVariable struct with to_source method"
puts "- Generates 'int x' for loop variable declaration"
puts "- Wraps body expression in BlockStatement"
puts "- Uses RangeForStatement for C++11 range-based for"
puts
puts "Features:"
puts "✅ Iterate over array literals: for x in [1,2,3] do x"
puts "✅ Iterate over variables: for x in arr do x"
puts "✅ Type inference for loop variable"
puts "✅ Expression evaluation in body"
puts "✅ C++11 range-based for syntax"
puts "✅ Type-safe through CoreIR"
puts
puts "LIMITATIONS (current):"
puts "- Body is single expression (no multiple statements yet)"
puts "- No range literals (0..10) yet"
puts "- No for-else construct"
puts "- For loop as expression (in return position)"
puts
puts "NEXT STEPS:"
puts "⏳ Support for range expressions: for i in 0..10"
puts "⏳ Multiple statements in body"
puts "⏳ List comprehensions using for loops"
puts "⏳ Break and continue statements"
puts
puts "=" * 70
puts "Demo completed!"
puts "=" * 70

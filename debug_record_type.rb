#!/usr/bin/env ruby
# Debug script to trace record type lowering error

require_relative "lib/mlc"

aurora_code = <<~AURORA
  type Point = { x: f32, y: f32 }
  fn make_point(x: f32, y: f32) -> Point = { x: x, y: y }
AURORA

begin
  result = MLC.to_cpp(aurora_code)
  puts "SUCCESS: Generated C++ code"
  puts result
rescue => e
  puts "ERROR: #{e.class}"
  puts "Message: #{e.message}"
  puts "\nBacktrace:"
  puts e.backtrace[0..15].join("\n")
end

#!/usr/bin/env ruby
# Generates synthetic MLC programs for benchmarking.
# Usage: ruby gen.rb <suite> <scale> <out_dir>
#   suite: functions | types | modules | match | strings
#   scale: integer (number of units)
#   out_dir: directory to write .mlc files

suite    = ARGV[0] || "functions"
scale    = (ARGV[1] || "100").to_i
out_dir  = ARGV[2] || "/tmp/bench_out"
Dir.mkdir(out_dir) unless Dir.exist?(out_dir)

def write(path, content)
  File.write(path, content)
end

case suite

when "functions"
  # Single module with N pure functions (arithmetic on i32)
  lines = []
  (1..scale).each do |i|
    lines << "fn func_#{i}(x: i32, y: i32) -> i32 = x * #{i} + y - #{i / 2 + 1}"
  end
  lines << "fn entry() -> i32 = func_1(1, 2)"
  write("#{out_dir}/main.mlc", lines.join("\n") + "\n")

when "types"
  # N sum types with 4 variants each, plus functions that match on them
  lines = []
  (1..scale).each do |i|
    lines << "type Shape#{i} = Circle#{i}(i32) | Rect#{i}(i32, i32) | Triangle#{i}(i32) | Point#{i}"
  end
  (1..scale).each do |i|
    lines << <<~MLC
      fn area_#{i}(s: Shape#{i}) -> i32 =
        match s {
          Circle#{i}(r) => r * r,
          Rect#{i}(w, h) => w * h,
          Triangle#{i}(b) => b,
          Point#{i} => 0
        }
    MLC
  end
  lines << "fn entry() -> i32 = area_1(Circle1(5))"
  write("#{out_dir}/main.mlc", lines.join("\n") + "\n")

when "modules"
  # N modules each exporting one function, main imports all
  (1..scale).each do |i|
    write("#{out_dir}/mod_#{i}.mlc",
      "export fn compute_#{i}(x: i32) -> i32 = x + #{i}\n")
  end
  imports = (1..scale).map { |i| "import { compute_#{i} } from './mod_#{i}'" }.join("\n")
  calls   = (1..scale).map { |i| "compute_#{i}(#{i})" }.join(" + ")
  write("#{out_dir}/main.mlc", "#{imports}\nfn entry() -> i32 = #{calls}\n")

when "match"
  # N functions each with a large match expression (K arms)
  arms_per_fn = 10
  fns = scale
  lines = ["type Color = " + (1..(arms_per_fn)).map { |i| "Color#{i}(i32)" }.join(" | ")]
  (1..fns).each do |f|
    arms = (1..arms_per_fn).map { |i| "Color#{i}(v) => v + #{f * i}" }.join(",\n    ")
    lines << "fn classify_#{f}(c: Color) -> i32 =\n  match c {\n    #{arms}\n  }"
  end
  lines << "fn entry() -> i32 = classify_1(Color1(1))"
  write("#{out_dir}/main.mlc", lines.join("\n") + "\n")

when "strings"
  # N functions doing string concatenation and comparison
  lines = []
  (1..scale).each do |i|
    lines << "fn label_#{i}(prefix: string) -> string = prefix + \"_item_#{i}\""
  end
  (1..scale).each do |i|
    lines << "fn check_#{i}(s: string) -> bool = s == \"hello_#{i}\""
  end
  lines << "fn entry() -> string = label_1(\"test\")"
  write("#{out_dir}/main.mlc", lines.join("\n") + "\n")

else
  abort "unknown suite: #{suite}"
end

puts "Generated #{suite}/#{scale} → #{out_dir}/main.mlc"

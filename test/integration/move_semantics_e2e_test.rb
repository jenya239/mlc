# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class MoveSemanticsE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  # ===========================================
  # Copy types: values are copied, original remains usable
  # ===========================================

  def test_copy_i32_used_after_bind
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let a = 10
        let b = a
        a + b
      end
    MLC
      assert_equal 20, status.exitstatus
    end
  end

  def test_copy_bool_used_after_bind
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let a = true
        let b = a
        if a then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_copy_i32_passed_to_multiple_functions
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn double(x: i32) -> i32 = x * 2
      fn triple(x: i32) -> i32 = x * 3

      fn main() -> i32 = do
        let n = 5
        let a = double(n)
        let b = triple(n)
        a + b
      end
    MLC
      assert_equal 25, status.exitstatus
    end
  end

  # ===========================================
  # Move semantics: std::move in generated C++
  # ===========================================

  def test_move_array_on_bind_compiles
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn sum_first(arr: i32[]) -> i32 = arr[0]

      fn main() -> i32 = do
        let a = [10, 20, 30]
        let b = a
        sum_first(b)
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  def test_move_generates_correct_result
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let a = [42, 0, 0]
        let b = a
        b[0]
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # ===========================================
  # Use-after-move: compiler rejects
  # ===========================================

  def test_use_after_move_array_rejected
    source = <<~MLC
      fn main() -> i32 = do
        let a = [1, 2, 3]
        let b = a
        a[0]
      end
    MLC

    Dir.mktmpdir do |dir|
      file = File.join(dir, "test.mlc")
      File.write(file, source)
      _stdout, stderr, status = Open3.capture3(CLI, file)
      refute status.success?, "Expected compilation to fail for use-after-move"
      assert_match(/moved/, stderr)
    end
  end

  def test_use_after_move_string_rejected
    source = <<~MLC
      fn main() -> i32 = do
        let a = "hello"
        let b = a
        println(a)
        0
      end
    MLC

    Dir.mktmpdir do |dir|
      file = File.join(dir, "test.mlc")
      File.write(file, source)
      _stdout, stderr, status = Open3.capture3(CLI, file)
      refute status.success?, "Expected compilation to fail for use-after-move"
      assert_match(/moved/, stderr)
    end
  end

  # ===========================================
  # Mutable reassignment resets moved state
  # ===========================================

  def test_mutable_reassignment_allows_reuse
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [1, 2, 3]
        let _b = a
        a = [4, 5, 6]
        a[0]
      end
    MLC
      assert_equal 4, status.exitstatus
    end
  end

  # ===========================================
  # Shared<T>: not moved on bind (RC)
  # ===========================================

  def test_shared_multiple_binds_ok
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let node = Shared.new(Node { value: 7 })
        let a = node
        let b = node
        a.value + b.value + node.value
      end
    MLC
      assert_equal 21, status.exitstatus
    end
  end

  # ===========================================
  # Complex programs with move semantics
  # ===========================================

  def test_fibonacci_with_copy_types
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn fib(n: i32) -> i32 =
        if n <= 1 then n
        else fib(n - 1) + fib(n - 2)

      fn main() -> i32 = do
        let a = fib(7)
        let b = a
        let c = a
        b + c - a
      end
    MLC
      assert_equal 13, status.exitstatus
    end
  end

  def test_pattern_match_with_copy_values
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Shape = Circle(i32) | Rect(i32, i32) | Point

      fn area(s: Shape) -> i32 =
        match s
          | Circle(r) => r * r
          | Rect(w, h) => w * h
          | Point => 0

      fn main() -> i32 = do
        let c = Circle(5)
        let result = area(c)
        result
      end
    MLC
      assert_equal 25, status.exitstatus
    end
  end

  def test_record_spread_with_move
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Point = { x: i32, y: i32, z: i32 }

      fn main() -> i32 = do
        let base = Point { x: 1, y: 2, z: 0 }
        let extended = Point { ...base, z: 3 }
        extended.x + extended.y + extended.z
      end
    MLC
      assert_equal 6, status.exitstatus
    end
  end
end

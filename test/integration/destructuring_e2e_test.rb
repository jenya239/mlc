# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class DestructuringE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, "-o#{dir}", source)
      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  def test_tuple_destructuring_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let (a, b, c) = (10, 20, 30)
        a + b + c
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  def test_record_destructuring_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Point = { x: i32, y: i32 }
      fn main() -> i32 = do
        let p = Point { x: 100, y: 200 }
        let { x, y } = p
        x + y
      end
    MLC
      assert_equal 44, status.exitstatus  # 300 % 256 = 44
    end
  end

  def test_multiple_destructuring_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let (a, b) = (5, 10)
        let (c, d) = (15, 20)
        a + b + c + d
      end
    MLC
      assert_equal 50, status.exitstatus
    end
  end

  def test_generic_pair_swap_with_tuple_parameter_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Pair<T, U> = { first: T, second: U }

      fn swap_pair<T, U>((left, right): Pair<T, U>) -> Pair<U, T> =
        Pair { first: right, second: left }

      fn main() -> i32 =
        let result = swap_pair(Pair { first: 4_i32, second: 8_i32 })
        result.first + result.second
    MLC
      assert_equal 12, status.exitstatus
    end
  end

  def test_tuple_parameter_destructuring_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn sum_pair((left, right): (i32, i32)) -> i32 = left + right

      fn main() -> i32 = sum_pair((3, 9))
    MLC
      assert_equal 12, status.exitstatus
    end
  end

  def test_record_parameter_destructuring_e2e
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Vec2 = { x: i32, y: i32 }

      fn sum_components({ x, y }: Vec2) -> i32 = x + y

      fn main() -> i32 = sum_components(Vec2 { x: 30, y: 12 })
    MLC
      assert_equal 42, status.exitstatus
    end
  end
end

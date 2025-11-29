# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class ControlFlowE2ETest < Minitest::Test
  tag :slow  # Requires C++ compilation

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code, &block)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)

      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  # === For Loops (using fold instead of mutation) ===

  def test_for_loop_with_println
    run_mlc(<<~MLC) do |stdout, stderr, status|
      import { to_string_i32 } from "Conv"

      fn main() -> i32 = do
        for i in [1, 2, 3] do
          println(to_string_i32(i))
        end
        0
      end
    MLC
      assert_match(/1/, stdout)
      assert_match(/2/, stdout)
      assert_match(/3/, stdout)
    end
  end

  # === If/Else Expressions ===

  def test_if_expression_simple
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = 10
        if x > 5 then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_if_expression_false_branch
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = 3
        if x > 5 then 100 else 50
      end
    MLC
      assert_equal 50, status.exitstatus
    end
  end

  # === Lambdas ===

  def test_lambda_simple
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let double = x => x * 2
        double(21)
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_lambda_with_types
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let add = (x: i32, y: i32) => x + y
        add(30, 12)
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_lambda_immediate_call
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        (x => x + 1)(41)
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # === Pipe Operator ===

  def test_pipe_operator
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn double(x: i32) -> i32 = x * 2
      fn add_one(x: i32) -> i32 = x + 1

      fn main() -> i32 = do
        10 |> double |> add_one
      end
    MLC
      assert_equal 21, status.exitstatus  # (10 * 2) + 1
    end
  end

  # === Blocks ===

  def test_do_block
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let a = 10
        let b = 20
        let c = do
          let x = a + b
          x * 2
        end
        c
      end
    MLC
      assert_equal 60, status.exitstatus  # (10 + 20) * 2
    end
  end

  # === Binary Operations ===

  def test_arithmetic_operations
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let a = 10 + 5
        let b = 20 - 8
        let c = 3 * 4
        let d = 15 / 3
        let e = 17 % 5

        a + b + c + d + e
      end
    MLC
      # 15 + 12 + 12 + 5 + 2 = 46
      assert_equal 46, status.exitstatus
    end
  end

  # === Known Limitations (skipped) ===

  def test_for_loop_sum
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5]
        let sum = 0
        for x in nums do
          let sum = sum + x
        end
        sum
      end
    MLC
      assert_equal 15, status.exitstatus
    end
  end

  def test_for_loop_nested
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let sum = 0
        for i in [1, 2] do
          for j in [10, 20] do
            let sum = sum + i + j
          end
        end
        sum
      end
    MLC
      assert_equal 66, status.exitstatus  # (0+1+10) + (11+1+20) + (32+2+10) + (44+2+20) = 66
    end
  end

  def test_if_expression_nested
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = 10
        if x > 5 then
          (if x > 15 then 3 else 2)
        else
          (if x > 0 then 1 else 0)
      end
    MLC
      assert_equal 2, status.exitstatus  # 10 > 5 is true, 10 > 15 is false -> 2
    end
  end

  def test_if_with_complex_condition
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = 10
        let y = 5
        if x > 5 && y < 10 then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus  # 10 > 5 && 5 < 10 = true -> 1
    end
  end

  def test_unary_negation
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn negate(x: i32) -> i32 = -(-x)

      fn main() -> i32 = negate(42)
    MLC
      assert_equal 42, status.exitstatus  # -(-42) = 42
    end
  end

  def test_logical_not
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = false
        let y = !x
        if y then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus  # !false = true -> 1
    end
  end

  def test_comparison_operations
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn to_int(b: bool) -> i32 = if b then 1 else 0

      fn main() -> i32 = do
        let a = 10
        let b = 5

        # Test all comparison operators
        let eq = to_int(a == a)    # true -> 1
        let ne = to_int(a != b)    # true -> 1
        let gt = to_int(a > b)     # true -> 1
        let lt = to_int(a < b)     # false -> 0
        let ge = to_int(a >= b)    # true -> 1
        let le = to_int(a <= b)    # false -> 0

        eq + ne + gt + ge  # 1 + 1 + 1 + 1 = 4
      end
    MLC
      assert_equal 4, status.exitstatus
    end
  end
end

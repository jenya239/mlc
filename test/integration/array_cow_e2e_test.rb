# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class ArrayCowE2ETest < Minitest::Test
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

  # =========================================================================
  # COW semantics: copy is independent
  # =========================================================================

  def test_cow_copy_is_independent
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [10, 20, 30]
        let b = a
        a.push(40)
        a.length() * 10 + b.length()
      end
    MLC
      # a.length() = 4, b.length() = 3 -> 4*10 + 3 = 43
      assert_equal 43, status.exitstatus
    end
  end

  def test_cow_set_does_not_affect_copy
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [1, 2, 3]
        let b = a
        a.set(0, 99)
        a[0] + b[0]
      end
    MLC
      # a[0] = 99, b[0] = 1 -> 100
      assert_equal 100, status.exitstatus
    end
  end

  def test_cow_multiple_copies
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [1, 2, 3]
        let b = a
        let c = a
        a.push(4)
        a.length() + b.length() + c.length()
      end
    MLC
      # 4 + 3 + 3 = 10
      assert_equal 10, status.exitstatus
    end
  end

  # =========================================================================
  # COW with function arguments
  # =========================================================================

  def test_array_passed_to_function_is_copied
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn get_len(arr: i32[]) -> i32 = arr.length()

      fn main() -> i32 = do
        let mut a = [1, 2, 3]
        let len_before = get_len(a)
        a.push(4)
        let len_after = a.length()
        len_before * 10 + len_after
      end
    MLC
      # 3 * 10 + 4 = 34
      assert_equal 34, status.exitstatus
    end
  end

  # =========================================================================
  # COW with HOFs
  # =========================================================================

  def test_cow_with_map
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [1, 2, 3]
        a.push(4)
        let b = a.map(x => x * 2)
        b[3]
      end
    MLC
      # b[3] = 4 * 2 = 8
      assert_equal 8, status.exitstatus
    end
  end

  def test_cow_with_filter
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut a = [1, 2, 3, 4, 5, 6]
        a.push(7)
        a.push(8)
        let evens = a.filter(x => x % 2 == 0)
        evens.length()
      end
    MLC
      # evens: [2, 4, 6, 8] -> length = 4
      assert_equal 4, status.exitstatus
    end
  end

  # =========================================================================
  # Mutation in loops
  # =========================================================================

  def test_push_in_while_loop
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut arr: i32[] = []
        let mut i = 0
        while i < 10 do
          arr.push(i)
          i = i + 1
        end
        arr.length()
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  def test_accumulate_sum_via_set
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let mut arr = [0, 0, 0]
        arr.set(0, 10)
        arr.set(1, 20)
        arr.set(2, 30)
        arr[0] + arr[1] + arr[2]
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end
end

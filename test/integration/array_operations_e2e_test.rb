# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class ArrayOperationsE2ETest < Minitest::Test
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

  # === Array Literals ===

  def test_array_literal_creation
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [10, 20, 30]
        nums[0] + nums[1] + nums[2]
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  def test_empty_array_with_type
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let empty: i32[] = []
        0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  # === Array Indexing ===

  def test_array_index_access
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30, 40, 50]
        arr[2]
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  def test_array_index_with_variable
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [5, 10, 15, 20]
        let idx = 3
        arr[idx]
      end
    MLC
      assert_equal 20, status.exitstatus
    end
  end

  def test_array_index_with_expression
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3, 4, 5]
        let base = 2
        arr[base + 1]
      end
    MLC
      assert_equal 4, status.exitstatus
    end
  end

  def test_array_first_and_last
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn get_first(arr: i32[]) -> i32 = arr[0]
      fn get_at(arr: i32[], idx: i32) -> i32 = arr[idx]

      fn main() -> i32 = do
        let nums = [7, 14, 21, 28]
        let first = get_first(nums)
        let last = get_at(nums, 3)
        first + last
      end
    MLC
      assert_equal 35, status.exitstatus  # 7 + 28
    end
  end

  # === Array Map ===

  def test_array_map_simple
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3]
        let doubled = nums.map(x => x * 2)
        doubled[0] + doubled[1] + doubled[2]
      end
    MLC
      assert_equal 12, status.exitstatus  # 2 + 4 + 6
    end
  end

  def test_array_map_with_addition
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [10, 20, 30]
        let incremented = nums.map(x => x + 5)
        incremented[1]
      end
    MLC
      assert_equal 25, status.exitstatus  # 20 + 5
    end
  end

  # === Array Filter ===

  def test_array_filter_evens
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5, 6]
        let evens = nums.filter(x => x % 2 == 0)
        evens[0] + evens[1] + evens[2]
      end
    MLC
      assert_equal 12, status.exitstatus  # 2 + 4 + 6
    end
  end

  def test_array_filter_greater_than
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [5, 15, 25, 35, 45]
        let big = nums.filter(x => x > 20)
        big[0] + big[1] + big[2]
      end
    MLC
      assert_equal 105, status.exitstatus  # 25 + 35 + 45
    end
  end

  # === Array Fold ===

  def test_array_fold_sum
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5]
        nums.fold(0, (acc, x) => acc + x)
      end
    MLC
      assert_equal 15, status.exitstatus
    end
  end

  def test_array_fold_product
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4]
        nums.fold(1, (acc, x) => acc * x)
      end
    MLC
      assert_equal 24, status.exitstatus  # 1 * 2 * 3 * 4
    end
  end

  def test_array_fold_with_initial
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [10, 20, 30]
        nums.fold(100, (acc, x) => acc + x)
      end
    MLC
      assert_equal 160, status.exitstatus  # 100 + 10 + 20 + 30
    end
  end

  # === Method Chaining ===

  def test_array_filter_then_map
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5, 6]
        let result = nums.filter(x => x % 2 == 0).map(y => y * 10)
        result[0] + result[1] + result[2]
      end
    MLC
      assert_equal 120, status.exitstatus  # 20 + 40 + 60
    end
  end

  def test_array_map_then_fold
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3]
        nums.map(x => x * x).fold(0, (acc, y) => acc + y)
      end
    MLC
      assert_equal 14, status.exitstatus  # 1 + 4 + 9
    end
  end

  def test_array_full_chain
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        nums
          .filter(x => x % 2 == 0)
          .map(y => y * y)
          .fold(0, (acc, z) => acc + z)
      end
    MLC
      # evens: [2,4,6,8,10] -> squares: [4,16,36,64,100] -> sum: 220
      assert_equal 220, status.exitstatus
    end
  end

  # === Nested Arrays ===

  def test_array_of_arrays_access
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
        let row = matrix[1]
        row[1]
      end
    MLC
      assert_equal 5, status.exitstatus  # matrix[1][1]
    end
  end

  # === Array with Functions ===

  def test_array_passed_to_function
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn sum_array(arr: i32[]) -> i32 =
        arr.fold(0, (acc, x) => acc + x)

      fn main() -> i32 = do
        let nums = [3, 5, 7, 9]
        sum_array(nums)
      end
    MLC
      assert_equal 24, status.exitstatus
    end
  end

  def test_array_transform_function
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn double_all(arr: i32[]) -> i32[] =
        arr.map(x => x * 2)

      fn main() -> i32 = do
        let nums = [5, 10, 15]
        let doubled = double_all(nums)
        doubled[0] + doubled[1] + doubled[2]
      end
    MLC
      assert_equal 60, status.exitstatus  # 10 + 20 + 30
    end
  end

  # === Known Limitations (skipped) ===

  def test_array_iteration_sum
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [5, 10, 15, 20]
        let sum = 0
        for x in nums do
          let sum = sum + x
        end
        sum
      end
    MLC
      assert_equal 50, status.exitstatus  # 5 + 10 + 15 + 20
    end
  end

  def test_array_iteration_count
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        let count = 0
        for x in nums do
          let count = if x % 2 == 0 then count + 1 else count
        end
        count
      end
    MLC
      assert_equal 5, status.exitstatus  # 5 even numbers
    end
  end

  def test_array_returned_from_function
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn get_nums() -> i32[] = [10, 20, 30]

      fn main() -> i32 = do
        let nums = get_nums()
        nums[0] + nums[1] + nums[2]
      end
    MLC
      assert_equal 60, status.exitstatus  # 10 + 20 + 30
    end
  end
end

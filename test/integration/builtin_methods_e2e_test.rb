# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class BuiltinMethodsE2ETest < Minitest::Test
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

  # === Numeric Methods ===

  def test_numeric_abs_positive
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = 42
        x.abs()
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_numeric_abs_negative
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let x = -42
        x.abs()
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # === String Methods ===

  def test_string_length
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello"
        s.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_string_is_empty_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello"
        if s.is_empty() then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_string_is_empty_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = ""
        if s.is_empty() then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_string_contains_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        if s.contains("world") then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_string_contains_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        if s.contains("foo") then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_string_starts_with_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        if s.starts_with("hello") then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_string_ends_with_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        if s.ends_with("world") then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_string_index_of_found
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        s.index_of("world") + 1
      end
    MLC
      assert_equal 7, status.exitstatus  # index 6 + 1
    end
  end

  def test_string_index_of_not_found
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let s = "hello world"
        let idx = s.index_of("foo")
        if idx < 0 then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus  # -1 returned
    end
  end

  # === Array Methods - length/is_empty ===

  def test_array_length
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3, 4, 5]
        arr.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_array_is_empty_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        if arr.is_empty() then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_array_is_empty_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr: i32[] = []
        if arr.is_empty() then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  # === Array Methods - any/all/none ===

  def test_array_any_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3, 4, 5]
        if arr.any(x => x > 3) then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_array_any_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        if arr.any(x => x > 10) then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_array_all_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [2, 4, 6, 8]
        if arr.all(x => x % 2 == 0) then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_array_all_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [2, 4, 5, 8]
        if arr.all(x => x % 2 == 0) then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_array_none_true
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        if arr.none(x => x > 10) then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_array_none_false
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 30]
        if arr.none(x => x > 10) then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  # === Array Methods - find/find_index/index_of ===

  def test_array_find
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 30, 4, 5]
        arr.find(x => x > 10)
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  def test_array_find_index
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30, 40]
        arr.find_index(x => x > 25)
      end
    MLC
      assert_equal 2, status.exitstatus  # index of 30
    end
  end

  def test_array_index_of
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30, 40]
        arr.index_of(30)
      end
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  def test_array_index_of_not_found
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        let idx = arr.index_of(100)
        if idx < 0 then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus  # -1 returned
    end
  end

  # === Array Methods - min/max ===

  def test_array_min
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [50, 20, 80, 10, 30]
        arr.min()
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  def test_array_max
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [50, 20, 80, 10, 30]
        arr.max()
      end
    MLC
      assert_equal 80, status.exitstatus
    end
  end

  # === Array Methods - concat ===

  def test_array_concat
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let a = [1, 2, 3]
        let b = [4, 5, 6]
        let c = a.concat(b)
        c[0] + c[3] + c[5]
      end
    MLC
      assert_equal 11, status.exitstatus  # 1 + 4 + 6
    end
  end

  # === Array Methods - slice ===

  def test_array_slice
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30, 40, 50]
        let sub = arr.slice(1, 4)
        sub[0] + sub[1] + sub[2]
      end
    MLC
      assert_equal 90, status.exitstatus  # 20 + 30 + 40
    end
  end

  # === Combined Tests ===

  def test_chained_array_methods
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        let result = nums
          .filter(x => x % 2 == 0)
          .map(y => y * 2)
        result.fold(0, (acc, z) => acc + z)
      end
    MLC
      # evens: [2,4,6,8,10] -> doubled: [4,8,12,16,20] -> sum: 60
      assert_equal 60, status.exitstatus
    end
  end

  def test_array_operations_combined
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn main() -> i32 = do
        let arr = [5, 10, 15, 20, 25]
        let len = arr.length()
        let has_big = if arr.any(x => x > 20) then 1 else 0
        let all_pos = if arr.all(x => x > 0) then 1 else 0
        len + has_big + all_pos
      end
    MLC
      assert_equal 7, status.exitstatus  # 5 + 1 + 1
    end
  end
end

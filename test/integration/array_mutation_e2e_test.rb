# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class ArrayMutationE2ETest < Minitest::Test
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
  # push
  # =========================================================================

  def test_push_increases_length
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.push(40)
        arr.push(50)
        arr.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_push_preserves_elements
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20]
        arr.push(30)
        arr[0] + arr[1] + arr[2]
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  def test_push_empty_array
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr: i32[] = []
        arr.push(42)
        arr[0]
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # =========================================================================
  # pop
  # =========================================================================

  def test_pop_decreases_length
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.pop()
        arr.length()
      end
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  # =========================================================================
  # set
  # =========================================================================

  def test_set_modifies_element
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.set(1, 99)
        arr[1]
      end
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  def test_set_first_element
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [0, 0, 0]
        arr.set(0, 42)
        arr[0]
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # =========================================================================
  # get
  # =========================================================================

  def test_get_element
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.get(2)
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  # =========================================================================
  # Combined operations
  # =========================================================================

  def test_push_set_get_combined
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr: i32[] = []
        arr.push(10)
        arr.push(20)
        arr.push(30)
        arr.set(1, 50)
        arr.get(0) + arr.get(1) + arr.get(2)
      end
    MLC
      assert_equal 90, status.exitstatus  # 10 + 50 + 30
    end
  end

  def test_build_array_in_loop
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let arr: i32[] = []
        let i = 0
        while i < 5 do
          arr.push(i * 10)
          i = i + 1
        end
        arr.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_push_with_println
    run_mlc(<<~MLC) do |stdout, _stderr, status|
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        arr.push(4)
        arr.push(5)
        println(arr.length().to_string())
        arr.length()
      end
    MLC
      assert_equal "5\n", stdout
      assert_equal 5, status.exitstatus
    end
  end
end

# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class TuplesE2ETest < Minitest::Test
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

  def test_tuple_swap
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn swap(a: i32, b: i32) -> (i32, i32) = (b, a)

      fn main() -> i32 = do
        let t = swap(10, 20)
        t.0
      end
    MLC
      assert_equal 20, status.exitstatus
    end
  end

  def test_tuple_second_element
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn swap(a: i32, b: i32) -> (i32, i32) = (b, a)

      fn main() -> i32 = do
        let t = swap(10, 20)
        t.1
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  def test_tuple_three_elements
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn make_triple(x: i32) -> (i32, i32, i32) = (x, x * 2, x * 3)

      fn main() -> i32 = do
        let t = make_triple(5)
        t.0 + t.1 + t.2
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  def test_tuple_inline_access
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = (7, 8).0
    MLC
      assert_equal 7, status.exitstatus
    end
  end

  def test_tuple_in_let_binding
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let pair = (15, 25)
        pair.0 + pair.1
      end
    MLC
      assert_equal 40, status.exitstatus
    end
  end

  def test_type_alias_compiles
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type UserId = i32

      fn get_id() -> UserId = 42

      fn main() -> i32 = get_id()
    MLC
      assert_equal 42, status.exitstatus
    end
  end
end

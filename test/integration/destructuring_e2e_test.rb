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
      stdout, stderr, status = Open3.capture3(CLI, source)
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
end

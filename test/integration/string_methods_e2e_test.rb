# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class StringMethodsE2ETest < Minitest::Test
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

  def test_length
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello".length()
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_is_empty_false
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let s = "hi"
        if s.is_empty() then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_is_empty_true
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let s = ""
        if s.is_empty() then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_upper_lower
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello".upper().length()
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_trim
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "  hi  ".trim().length()
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  def test_contains_true
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = if "hello world".contains("world") then 1 else 0
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_contains_false
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = if "hello".contains("xyz") then 1 else 0
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_starts_with
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = if "hello".starts_with("he") then 1 else 0
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_ends_with
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = if "hello".ends_with("lo") then 1 else 0
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_split_length
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "a,b,c".split(",").length()
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  def test_split_empty_delimiter
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "abc".split("").length()
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  def test_substring
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello world".substring(6, 5).length()
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_replace
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello".replace("l", "r").length()
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_repeat
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "ab".repeat(3).length()
    MLC
      assert_equal 6, status.exitstatus
    end
  end

  def test_reverse
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello".reverse().length()
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_index_of_found
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "hello".index_of("ll")
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  def test_index_of_not_found
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let idx = "hello".index_of("xyz")
        if idx < 0 then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_string_in_function
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn count_words(s: string) -> i32 = s.split(" ").length()

      fn main() -> i32 = count_words("one two three")
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  def test_chained_methods
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = "  Hello World  ".trim().lower().length()
    MLC
      assert_equal 11, status.exitstatus
    end
  end
end

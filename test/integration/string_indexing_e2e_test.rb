# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class StringIndexingE2ETest < Minitest::Test
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

  def test_string_indexing_first_char
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let s = "hello"
        let c = s[0]
        s.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  def test_string_indexing_last_char
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let s = "world"
        let last = s[4]
        s.length()
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end
end

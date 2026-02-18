# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class FileIoE2ETest < Minitest::Test
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

  def test_file_exists_true
    Dir.mktmpdir do |dir|
      path = File.join(dir, "hello.txt")
      File.write(path, "hi")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = if File.exists("#{path}") then 1 else 0
      MLC
        assert_equal 1, status.exitstatus
      end
    end
  end

  def test_file_exists_false
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = if File.exists("/tmp/no_such_file_mlc_test_xyz123") then 1 else 0
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_file_write_and_exists
    Dir.mktmpdir do |dir|
      path = File.join(dir, "out.txt")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = do
          File.write("#{path}", "hello")
          if File.exists("#{path}") then 1 else 0
        end
      MLC
        assert_equal 1, status.exitstatus
      end
    end
  end

  def test_file_read_length
    Dir.mktmpdir do |dir|
      path = File.join(dir, "data.txt")
      File.write(path, "hello")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = File.read("#{path}").length()
      MLC
        assert_equal 5, status.exitstatus
      end
    end
  end

  def test_file_write_read_roundtrip
    Dir.mktmpdir do |dir|
      path = File.join(dir, "rtrip.txt")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = do
          File.write("#{path}", "world")
          let s = File.read("#{path}")
          s.length()
        end
      MLC
        assert_equal 5, status.exitstatus
      end
    end
  end

  def test_file_read_lines_count
    Dir.mktmpdir do |dir|
      path = File.join(dir, "lines.txt")
      File.write(path, "one\ntwo\nthree\n")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = File.read_lines("#{path}").length()
      MLC
        assert_equal 3, status.exitstatus
      end
    end
  end

  def test_file_append
    Dir.mktmpdir do |dir|
      path = File.join(dir, "append.txt")
      File.write(path, "hello")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn main() -> i32 = do
          File.append("#{path}", " world")
          File.read("#{path}").length()
        end
      MLC
        assert_equal 11, status.exitstatus
      end
    end
  end

  def test_file_in_function
    Dir.mktmpdir do |dir|
      path = File.join(dir, "fn.txt")
      File.write(path, "test data")
      run_mlc(<<~MLC) do |_stdout, _stderr, status|
        fn file_size(path: string) -> i32 = File.read(path).length()

        fn main() -> i32 = file_size("#{path}")
      MLC
        assert_equal 9, status.exitstatus
      end
    end
  end
end

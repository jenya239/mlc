# frozen_string_literal: true

require_relative "../test_helper"

class FileIoTest < Minitest::Test
  def test_file_read_codegen
    cpp = MLC.to_cpp('fn main() -> i32 = do let s = File.read("/tmp/x") 0 end')
    assert_includes cpp, "mlc::file::read_to_string"
    assert_includes cpp, "mlc/io/file.hpp"
  end

  def test_file_write_codegen
    cpp = MLC.to_cpp('fn main() -> i32 = do File.write("/tmp/x", "data") 0 end')
    assert_includes cpp, "mlc::file::write_string"
  end

  def test_file_exists_codegen
    cpp = MLC.to_cpp('fn main() -> i32 = do let b = File.exists("/tmp/x") 0 end')
    assert_includes cpp, "mlc::file::exists"
    assert_includes cpp, "bool b"
  end

  def test_file_append_codegen
    cpp = MLC.to_cpp('fn main() -> i32 = do File.append("/tmp/x", "more") 0 end')
    assert_includes cpp, "mlc::file::append_string"
  end

  def test_file_read_lines_codegen
    cpp = MLC.to_cpp('fn main() -> i32 = do let lines = File.read_lines("/tmp/x") lines.length() end')
    assert_includes cpp, "mlc::file::read_lines"
    assert_includes cpp, "mlc::Array<mlc::String>"
  end

  def test_file_read_return_type
    cpp = MLC.to_cpp('fn main() -> i32 = do let s = File.read("/tmp/x") s.length() end')
    assert_includes cpp, "mlc::String s = mlc::file::read_to_string"
  end

  def test_file_exists_in_condition
    cpp = MLC.to_cpp('fn main() -> i32 = if File.exists("/tmp/x") then 1 else 0')
    assert_includes cpp, "mlc::file::exists"
  end

  def test_file_read_lines_length
    cpp = MLC.to_cpp(<<~MLC)
      fn count_lines(path: string) -> i32 = File.read_lines(path).length()
      fn main() -> i32 = count_lines("/tmp/test.txt")
    MLC
    assert_includes cpp, "mlc::file::read_lines"
    assert_includes cpp, ".size()"
  end
end

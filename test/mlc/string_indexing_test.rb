# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class StringIndexingTest < Minitest::Test
  def test_string_index_access_compiles
    source = <<~MLC
      fn main() -> i32 = do
        let s = "hello"
        let c = s[0]
        42
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "char c = s[0]"
  end

  def test_string_index_in_expression
    source = <<~MLC
      fn main() -> i32 = do
        let s = "test"
        s.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "s.length()"
  end

  def test_string_multiple_indexes
    source = <<~MLC
      fn main() -> i32 = do
        let s = "abc"
        let a = s[0]
        let b = s[1]
        let c = s[2]
        s.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "char a = s[0]"
    assert_includes cpp, "char b = s[1]"
    assert_includes cpp, "char c = s[2]"
  end
end

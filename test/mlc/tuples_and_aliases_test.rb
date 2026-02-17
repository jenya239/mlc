# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class TuplesAndAliasesTest < Minitest::Test
  # ===========================================
  # Tuple literals and access
  # ===========================================

  def test_tuple_literal_codegen
    source = <<~MLC
      fn foo() -> (i32, i32) = (1, 2)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::tuple<int, int>"
    assert_includes cpp, "std::make_tuple(1, 2)"
  end

  def test_tuple_access_codegen
    source = <<~MLC
      fn foo() -> i32 = do
        let t = (10, 20)
        t.0
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::get<0>(t)"
  end

  def test_tuple_second_element
    source = <<~MLC
      fn foo() -> i32 = do
        let t = (10, 20)
        t.1
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::get<1>(t)"
  end

  def test_tuple_three_elements
    source = <<~MLC
      fn foo() -> (i32, i32, i32) = (1, 2, 3)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::tuple<int, int, int>"
    assert_includes cpp, "std::make_tuple(1, 2, 3)"
  end

  def test_tuple_return_type_in_signature
    source = <<~MLC
      fn swap(a: i32, b: i32) -> (i32, i32) = (b, a)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::tuple<int, int> swap"
  end

  def test_tuple_parameter_type
    source = <<~MLC
      fn first(t: (i32, i32)) -> i32 = t.0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::tuple<int, int>"
    assert_includes cpp, "std::get<0>(t)"
  end

  # ===========================================
  # Type aliases
  # ===========================================

  def test_type_alias_primitive
    source = <<~MLC
      type UserId = i32
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "using UserId = int;"
  end

  def test_type_alias_array
    source = <<~MLC
      type Numbers = i32[]
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "using Numbers = mlc::Array<int>;"
  end

  def test_type_alias_tuple
    source = <<~MLC
      type Pair = (i32, str)
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "using Pair = std::tuple<int, mlc::String>;"
  end

  def test_type_alias_string
    source = <<~MLC
      type Name = str
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "using Name = mlc::String;"
  end

  def test_type_alias_generic
    source = <<~MLC
      type OptionalInt = Option<i32>
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "using OptionalInt ="
  end

  def test_record_still_generates_struct
    source = <<~MLC
      type Point = { x: i32, y: i32 }
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "struct Point"
    refute_includes cpp, "using Point"
  end
end

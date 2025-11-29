# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCStringLenAliasTest < Minitest::Test
  # ========== String .len() Tests ==========

  def test_string_len_method
    source = <<~MLC
      fn get_len(s: str) -> i32 = s.len()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "s.length()", ".len() should map to .length() in C++"
  end

  def test_string_length_method_still_works
    source = <<~MLC
      fn get_length(s: str) -> i32 = s.length()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "s.length()"
  end

  def test_string_len_chained
    source = <<~MLC
      fn process(s: str) -> i32 = s.trim().len()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".trim()"
    assert_includes cpp, ".length()", "chained .len() should map to .length()"
  end

  # ========== Array .len() Tests ==========

  def test_array_len_type_inference
    source = <<~MLC
      fn arr_len(arr: [i32]) -> i32 = arr.len()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.size()", ".len() on array should map to .size() in C++"
  end

  def test_array_type_prefix_syntax_parsing
    source = <<~MLC
      fn sum(arr: [i32]) -> i32 = arr[0]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::ArrayType, param_type
    assert_equal "i32", param_type.element_type.name
  end

  def test_array_type_prefix_syntax_nested
    source = <<~MLC
      fn matrix(m: [[i32]]) -> i32 = m[0][0]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    param_type = func.params.first.type

    # [[i32]] => ArrayType(ArrayType(i32))
    assert_instance_of MLC::Source::AST::ArrayType, param_type
    assert_instance_of MLC::Source::AST::ArrayType, param_type.element_type
    assert_equal "i32", param_type.element_type.element_type.name
  end
end

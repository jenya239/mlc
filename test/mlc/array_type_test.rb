# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCArrayTypeTest < Minitest::Test
  def test_array_parameter_maps_to_std_vector
    mlc_source = <<~MLC
      fn count(items: str[]) -> i32 =
        if items.is_empty() then
          0
        else
          items.length()
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::Array<mlc::String>"
    assert_includes cpp, "items.length"
    assert_includes cpp, "items.empty"
  end

  def test_empty_array_in_if_takes_other_branch_element_type
    mlc_source = <<~MLC
      fn names() -> [string] = ["a"]
      fn pick(flag: bool) -> [string] =
        if flag then [] else names()
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::Array<mlc::String> pick"
    refute_includes cpp, "mlc::Array<int>"
  end

  def test_empty_array_literal_takes_annotated_element_type
    mlc_source = <<~MLC
      fn none() -> [string] = []
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::Array<mlc::String> none"
    refute_includes cpp, "mlc::Array<int>"
  end

  def test_empty_array_match_arm_takes_other_arm_element_type
    mlc_source = <<~MLC
      fn labels(kind: i32) -> [string] =
        match kind {
          0 => ["a"],
          _ => []
        }
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::Array<mlc::String>"
    refute_includes cpp, "mlc::Array<int>"
  end

  def test_empty_array_record_field_takes_field_element_type
    mlc_source = <<~MLC
      type Parser = { errors: [string], position: i32 }
      fn parser_new() -> Parser = Parser { errors: [], position: 0 }
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::Array<mlc::String> errors"
    refute_includes cpp, "mlc::Array<int>"
  end

  def test_array_branches_with_different_elements_are_rejected
    mlc_source = <<~MLC
      fn bad(flag: bool) -> [string] =
        if flag then [1] else ["a"]
    MLC

    error = assert_raises(MLC::CompileError) { MLC.to_cpp(mlc_source) }
    assert_includes error.message, "if expression branches"
    assert_includes error.message, "i32"
    assert_includes error.message, "string"
  end
end

# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

# TRACK_LANG_ERROR_UNION STEP=2 — Ruby T!E → Result<T,E> desugar.
class ErrorUnionSugarTest < Minitest::Test
  def parse(source)
    MLC::Source::Parser::Parser.new(source).parse
  end

  def test_return_type_desugars_to_result_generic
    program = parse("fn divide(a: i32, b: i32) -> i32!string = Ok(0)")
    function_declaration = program.declarations.first
    return_type = function_declaration.ret_type
    assert_instance_of MLC::Source::AST::GenericType, return_type
    assert_equal "Result", return_type.base_type.name
    assert_equal 2, return_type.type_params.length
    assert_equal "i32", return_type.type_params[0].name
    assert_equal "string", return_type.type_params[1].name
  end

  def test_field_type_desugars_to_result_generic
    program = parse("type Box = { value: i32!string }\nfn main() -> i32 = 0")
    type_declaration = program.declarations.first
    field_type = type_declaration.type.fields.first[:type]
    assert_instance_of MLC::Source::AST::GenericType, field_type
    assert_equal "Result", field_type.base_type.name
    assert_equal "i32", field_type.type_params[0].name
    assert_equal "string", field_type.type_params[1].name
  end

  def test_explicit_result_unchanged
    program = parse("fn f() -> Result<i32, string> = Ok(0)")
    return_type = program.declarations.first.ret_type
    assert_instance_of MLC::Source::AST::GenericType, return_type
    assert_equal "Result", return_type.base_type.name
    assert_equal 2, return_type.type_params.length
  end

  def test_bang_without_error_type_raises
    assert_raises(StandardError) do
      parse("fn f() -> i32! = Ok(0)")
    end
  end
end

# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCLambdaTest < Minitest::Test
  def test_parse_simple_lambda
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let f = x => x + 1;
        f(5)
    MLCORA

    ast = MLC.parse(mlc_source)
    func = ast.declarations.first

    # Body should be a block with variable declaration and usage
    assert_instance_of MLC::Source::AST::Block, func.body
    assert_equal 2, func.body.stmts.size
    assert_instance_of MLC::Source::AST::VariableDecl, func.body.stmts.first

    # Value should be a lambda
    lambda_expr = func.body.stmts.first.value
    assert_instance_of MLC::Source::AST::Lambda, lambda_expr
    assert_equal 1, lambda_expr.params.length
    assert_instance_of MLC::Source::AST::LambdaParam, lambda_expr.params[0]
    assert_equal "x", lambda_expr.params[0].name
  end

  def test_parse_lambda_with_multiple_params
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let add = (x, y) => x + y;
        add(2, 3)
    MLCORA

    ast = MLC.parse(mlc_source)
    func = ast.declarations.first
    block = func.body
    assert_instance_of MLC::Source::AST::Block, block
    lambda_expr = block.stmts.first.value

    assert_instance_of MLC::Source::AST::Lambda, lambda_expr
    assert_equal 2, lambda_expr.params.length
    assert_equal ["x", "y"], lambda_expr.params.map(&:name)
  end

  def test_parse_lambda_with_types
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let f = (x: i32) => x + 1;
        f(5)
    MLCORA

    ast = MLC.parse(mlc_source)
    func = ast.declarations.first
    block = func.body
    lambda_expr = block.stmts.first.value

    assert_equal "i32", lambda_expr.params[0].type.name
  end

  def test_lambda_lowering_to_cpp
    # Test direct lambda expression (not in let binding)
    mlc_source = <<~MLCORA
      fn apply() -> i32 =
        (x => x + 1)(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Should generate C++ lambda
    assert_includes cpp_code, "[]"
    assert_includes cpp_code, "return"
    assert_includes cpp_code, "int x"
  end

  def test_lambda_in_function_call
    mlc_source = <<~MLCORA
      fn apply(x: i32) -> i32 =
        let f = y => y + 1;
        f(x)
    MLCORA

    ast = MLC.parse(mlc_source)
    refute_nil ast
  end
end

# frozen_string_literal: true

require_relative "../test_helper"

class ParameterDestructuringParserTest < Minitest::Test
  def parse_program(code)
    MLC::Source::Parser::Parser.new(code).parse
  end

  def test_record_pattern_parameter_on_parse_tree
    ast = parse_program(<<~MLC)
      type Vec2 = { x: i32, y: i32 }
      fn sum_xy({ x, y }: Vec2) -> i32 = x + y

      fn main() -> i32 = 0
    MLC

    function_declaration = ast.declarations.find do |declaration|
      declaration.is_a?(MLC::Source::AST::FuncDecl) && declaration.name == "sum_xy"
    end
    parameter = function_declaration.params.first
    assert_equal "__mlc_ds0", parameter.name
    assert_equal :record, parameter.destructure_pattern.kind
    assert_equal %w[x y], parameter.destructure_pattern.data[:bindings]
    assert_nil parameter.destructure_pattern.data[:rest]
  end

  def test_expand_rewrites_body_and_strips_pattern_metadata
    inner_ast = parse_program(<<~MLC)
      type Vec2 = { x: i32, y: i32 }
      fn sum_xy({ x, y }: Vec2) -> i32 = x + y

      fn main() -> i32 = 0
    MLC

    function_declaration = inner_ast.declarations.find do |declaration|
      declaration.is_a?(MLC::Source::AST::FuncDecl) && declaration.name == "sum_xy"
    end

    expanded = MLC::Source::Transforms::ParameterDestructuringExpand.expand(function_declaration)

    assert_nil expanded.params.first.destructure_pattern
    assert_kind_of MLC::Source::AST::BlockExpr, expanded.body
    assert_kind_of MLC::Source::AST::DestructuringDecl, expanded.body.statements.first
    assert_equal "__mlc_ds0", expanded.body.statements.first.value.name
    refute_nil expanded.body.result_expr
  end

  def test_tuple_pattern_parameter_on_parse_tree
    ast = parse_program(<<~MLC)
      fn add_pair((first, second): (i32, i32)) -> i32 = first + second

      fn main() -> i32 = 0
    MLC

    function_declaration = ast.declarations.find do |declaration|
      declaration.is_a?(MLC::Source::AST::FuncDecl) && declaration.name == "add_pair"
    end
    parameter = function_declaration.params.first
    assert_equal "__mlc_ds0", parameter.name
    assert_equal :tuple, parameter.destructure_pattern.kind
    elements = parameter.destructure_pattern.data[:elements]
    assert_equal :var, elements[0].kind
    assert_equal "first", elements[0].data[:name]
    assert_equal :var, elements[1].kind
    assert_equal "second", elements[1].data[:name]
  end

  def test_single_tuple_parameter_requires_trailing_comma
    error = assert_raises(MLC::CompileError) do
      parse_program(<<~MLC)
        fn bad((value): (i32,)) -> i32 = value

        fn main() -> i32 = 0
      MLC
    end

    assert_match(/trailing comma/, error.message)
  end

  def test_empty_record_parameter_pattern_rejected
    assert_raises(MLC::CompileError) do
      parse_program(<<~MLC)
        fn bad({ }: i32) -> i32 = 0

        fn main() -> i32 = 0
      MLC
    end
  end

  def test_duplicate_record_parameter_field_rejected
    error = assert_raises(MLC::CompileError) do
      parse_program(<<~MLC)
        type Vec2 = { x: i32, y: i32 }
        fn bad({ x, x }: Vec2) -> i32 = x

        fn main() -> i32 = 0
      MLC
    end

    assert_match(/duplicate field/, error.message)
  end

  def test_record_rest_pattern_parameter_on_parse_tree
    ast = parse_program(<<~MLC)
      type Triple = { a: i32, b: i32, c: i32 }
      fn take_ab({ a, b, ...tail }: Triple) -> i32 = a + b

      fn main() -> i32 = 0
    MLC

    function_declaration = ast.declarations.find do |declaration|
      declaration.is_a?(MLC::Source::AST::FuncDecl) && declaration.name == "take_ab"
    end
    parameter = function_declaration.params.first
    assert_equal :record, parameter.destructure_pattern.kind
    assert_equal %w[a b], parameter.destructure_pattern.data[:bindings]
    assert_equal "tail", parameter.destructure_pattern.data[:rest]
  end

  def test_extern_with_destructuring_parameter_rejected
    inner_ast = parse_program(<<~MLC)
      type Vec2 = { x: i32, y: i32 }
      extern fn bad({ x, y }: Vec2) -> i32

      fn main() -> i32 = 0
    MLC

    function_declaration = inner_ast.declarations.find do |declaration|
      declaration.is_a?(MLC::Source::AST::FuncDecl) && declaration.name == "bad"
    end

    error = assert_raises(MLC::CompileError) do
      MLC::Source::Transforms::ParameterDestructuringExpand.expand(function_declaration)
    end

    assert_match(/extern/, error.message)
  end
end

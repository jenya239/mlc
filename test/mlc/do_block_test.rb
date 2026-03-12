# frozen_string_literal: true

require_relative "../test_helper"

class DoBlockTest < Minitest::Test
  def test_simple_do_block
    source = <<~MLCORA
      fn test() -> i32 = do
        42
      end
    MLCORA

    ast = MLC.parse(source)
    assert_equal 1, ast.declarations.length
    func = ast.declarations.first
    # Now returns BlockExpr instead of DoExpr
    assert func.body.is_a?(MLC::Source::AST::BlockExpr)
    # BlockExpr has no statements, just result_expr
    assert_equal 0, func.body.statements.length
    assert func.body.result_expr.is_a?(MLC::Source::AST::IntLit)
  end

  def test_do_block_multiple_expressions
    source = <<~MLCORA
      fn test() -> i32 = do
        let x = 10;
        let y = 20;
        x + y
      end
    MLCORA

    ast = MLC.parse(source)
    func = ast.declarations.first
    # Now returns BlockExpr
    assert func.body.is_a?(MLC::Source::AST::BlockExpr)
    # 2 statements (let x, let y), 1 result (x + y)
    assert_equal 2, func.body.statements.length
    assert func.body.result_expr.is_a?(MLC::Source::AST::BinaryOp)
  end

  def test_do_block_with_function_calls
    source = <<~MLCORA
      fn greet() -> i32 = do
        println("Hello")
        println("World")
        0
      end
    MLCORA

    ast = MLC.parse(source)
    func = ast.declarations.first
    # Now returns BlockExpr
    assert func.body.is_a?(MLC::Source::AST::BlockExpr)
    # 2 statements (println calls), 1 result (0)
    assert_equal 2, func.body.statements.length
    assert func.body.result_expr.is_a?(MLC::Source::AST::IntLit)
  end

  def test_do_block_compiles
    source = <<~MLCORA
      fn compute() -> i32 = do
        let x = 10
        let y = 20
        x + y
      end
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "compute"
    assert cpp.include?("10") || cpp.include?("20")
  end

  def test_do_block_with_while_and_result
    source = <<~MLCORA
      fn foo() -> i32 = do
        let x = 0
        while x < 3 do
          x = x + 1
        end
        x
      end
    MLCORA

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body
    assert body.is_a?(MLC::Source::AST::BlockExpr)
    # 2 statements: let x, while loop. result: x (VarRef)
    assert_equal 2, body.statements.length
    assert body.result_expr.is_a?(MLC::Source::AST::VarRef),
           "expected VarRef (x), got #{body.result_expr.class}"
  end

  def test_do_block_with_while_if_and_record_result
    # Mimics cpp_parse: do; let; let; while with if/else (nested if)/end; RecordLit; end
    source = <<~MLCORA
      type R = R { x: i32 }
      fn foo() -> R = do
        let x = 0
        let y = 0
        while y < 1 do
          if true then
            x = 1
          else
            if false then x = 2 end
            y = 1
          end
          y = y + 1
        end
        R { x: x }
      end
    MLCORA

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.respond_to?(:name) && d.name == "foo" }
    body = func.body
    assert body.is_a?(MLC::Source::AST::BlockExpr)
    assert_equal 3, body.statements.length, "expected 3 stmts: let, let, while"
    assert body.result_expr.is_a?(MLC::Source::AST::RecordLit),
           "expected RecordLit, got #{body.result_expr.class}"
  end

  def test_nested_do_blocks
    source = <<~MLCORA
      fn test() -> i32 = do
        let x = do
          10
        end
        x + 5
      end
    MLCORA

    ast = MLC.parse(source)
    func = ast.declarations.first
    # Now returns BlockExpr
    assert func.body.is_a?(MLC::Source::AST::BlockExpr)
    # Nested do-block also becomes BlockExpr
    assert func.body.statements.first.is_a?(MLC::Source::AST::VariableDecl)
    assert func.body.statements.first.value.is_a?(MLC::Source::AST::BlockExpr)
  end
end

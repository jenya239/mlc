# frozen_string_literal: true

require_relative "../test_helper"

class NestedPatternsTest < Minitest::Test
  def test_nested_pattern_parsing
    source = <<~MLC
      type Option<T> = Some(T) | None
      type Result<T, E> = Ok(T) | Err(E)

      fn extract(r: Result<Option<i32>, str>) -> i32 =
        match r
          | Ok(Some(v)) => v
          | Ok(None) => 0
          | Err(_) => -1
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "extract" }
    assert func, "Function extract not found"

    match_expr = func.body
    assert match_expr.is_a?(MLC::Source::AST::MatchExpr)

    # First arm should have nested pattern Ok(Some(v))
    first_arm = match_expr.arms[0]
    pattern = first_arm[:pattern]
    assert_equal :constructor, pattern.kind
    assert_equal "Ok", pattern.data[:name]

    # Check nested pattern
    fields = pattern.data[:fields]
    assert_equal 1, fields.length

    nested = fields[0]
    assert nested.is_a?(MLC::Source::AST::Pattern)
    assert_equal :constructor, nested.kind
    assert_equal "Some", nested.data[:name]
  end

  def test_nested_pattern_lowering
    source = <<~MLC
      type Option<T> = Some(T) | None
      type Result<T, E> = Ok(T) | Err(E)

      fn extract(r: Result<Option<i32>, str>) -> i32 =
        match r
          | Ok(Some(v)) => v
          | Ok(None) => 0
          | Err(_) => -1
    MLC

    cpp = MLC.to_cpp(source)

    # Should use IIFE approach (not std::visit) for nested patterns
    assert_includes cpp, "[&]()"

    # Should have std::holds_alternative checks
    assert_includes cpp, "std::holds_alternative<Ok>"
    assert_includes cpp, "std::holds_alternative<Some>"
  end

  def test_deeply_nested_pattern_parsing
    source = <<~MLC
      type A = X(B)
      type B = Y(i32)

      fn deep(a: A) -> i32 =
        match a
          | X(Y(n)) => n
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "deep" }
    match_expr = func.body

    pattern = match_expr.arms[0][:pattern]
    assert_equal "X", pattern.data[:name]

    nested = pattern.data[:fields][0]
    assert_equal "Y", nested.data[:name]
    assert_equal ["n"], nested.data[:fields]
  end

  def test_mixed_nested_and_simple_bindings
    source = <<~MLC
      type Pair<A, B> = Pair(A, B)
      type Option<T> = Some(T) | None

      fn handle(p: Pair<i32, Option<str>>) -> i32 =
        match p
          | Pair(n, Some(s)) => n
          | Pair(n, None) => 0
    MLC

    cpp = MLC.to_cpp(source)
    # Should compile without error
    assert_includes cpp, "std::holds_alternative"
  end

  def test_simple_pattern_still_uses_std_visit
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn unwrap(opt: Option<i32>) -> i32 =
        match opt
          | Some(x) => x
          | None => 0
    MLC

    cpp = MLC.to_cpp(source)
    # Without nested patterns, should use std::visit (overloaded)
    assert_includes cpp, "overloaded"
  end
end

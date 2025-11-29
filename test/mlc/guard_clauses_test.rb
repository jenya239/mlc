# frozen_string_literal: true

require_relative "../test_helper"

class GuardClausesTest < Minitest::Test
  def test_guard_clause_parsing
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn test_guard(opt: Option<i32>) -> i32 =
        match opt
          | Some(x) if x > 0 => x * 2
          | Some(x) => 0
          | None => -1
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "test_guard" }
    assert func, "Function test_guard not found"

    match_expr = func.body
    assert match_expr.is_a?(MLC::Source::AST::MatchExpr), "Body should be MatchExpr"

    # First arm should have a guard
    first_arm = match_expr.arms[0]
    assert first_arm[:guard], "First arm should have a guard"

    # Second arm should not have a guard
    second_arm = match_expr.arms[1]
    assert_nil second_arm[:guard], "Second arm should not have a guard"
  end

  def test_guard_clause_lowering_to_cpp
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn test_guard(opt: Option<i32>) -> i32 =
        match opt
          | Some(x) if x > 0 => x * 2
          | Some(x) => 0
          | None => -1
    MLC

    cpp = MLC.to_cpp(source)

    # Should use IIFE with if-else chain (not std::visit)
    assert_includes cpp, "[&]()"

    # Should have std::holds_alternative checks
    assert_includes cpp, "std::holds_alternative<Some>"
    assert_includes cpp, "std::holds_alternative<None>"

    # Should have guard condition
    assert_includes cpp, "x > 0"
  end

  def test_guard_with_wildcard
    source = <<~MLC
      fn classify(n: i32) -> str =
        match n
          | x if x > 100 => "big"
          | x if x > 10 => "medium"
          | _ => "small"
    MLC

    cpp = MLC.to_cpp(source)

    # Should have guard conditions
    assert_includes cpp, "x > 100"
    assert_includes cpp, "x > 10"
  end

  def test_multiple_guards_on_same_pattern
    source = <<~MLC
      type Result<T, E> = Ok(T) | Err(E)

      fn handle(r: Result<i32, str>) -> i32 =
        match r
          | Ok(v) if v > 100 => 3
          | Ok(v) if v > 0 => 2
          | Ok(v) => 1
          | Err(_) => 0
    MLC

    cpp = MLC.to_cpp(source)

    # Should have multiple guards for Ok
    assert_includes cpp, "v > 100"
    assert_includes cpp, "v > 0"
  end

  def test_guard_without_guards_uses_std_visit
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn unwrap_or(opt: Option<i32>, default: i32) -> i32 =
        match opt
          | Some(x) => x
          | None => default
    MLC

    cpp = MLC.to_cpp(source)

    # Without guards, should use std::visit (overloaded pattern)
    assert_includes cpp, "overloaded"
  end

  def test_guard_with_complex_condition
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn complex_guard(opt: Option<i32>) -> i32 =
        match opt
          | Some(x) if x > 0 => x
          | Some(x) => 0
          | None => -1
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::holds_alternative"
  end
end

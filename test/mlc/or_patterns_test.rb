# frozen_string_literal: true

require_relative "../test_helper"

class OrPatternsTest < Minitest::Test
  def test_or_pattern_parsing_brace_style
    source = <<~MLC
      type Color = Red | Green | Blue | Yellow

      fn is_primary(c: Color) -> bool =
        match c {
          Red | Green | Blue => true,
          _ => false
        }
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "is_primary" }
    assert func, "Function is_primary not found"

    match_expr = func.body
    first_arm = match_expr.arms[0]
    pattern = first_arm[:pattern]

    assert_equal :or, pattern.kind
    assert_equal 3, pattern.data[:alternatives].length
    assert_equal "Red", pattern.data[:alternatives][0].data[:name]
    assert_equal "Green", pattern.data[:alternatives][1].data[:name]
    assert_equal "Blue", pattern.data[:alternatives][2].data[:name]
  end

  def test_or_pattern_parsing_pipe_style
    source = <<~MLC
      type Color = Red | Green | Blue | Yellow

      fn is_warm(c: Color) -> bool =
        match c
          | Red | Yellow => true
          | _ => false
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "is_warm" }
    assert func, "Function is_warm not found"

    match_expr = func.body
    first_arm = match_expr.arms[0]
    pattern = first_arm[:pattern]

    assert_equal :or, pattern.kind
    assert_equal 2, pattern.data[:alternatives].length
  end

  def test_or_pattern_lowering
    source = <<~MLC
      type Color = Red | Green | Blue

      fn is_red_or_blue(c: Color) -> bool =
        match c
          | Red | Blue => true
          | _ => false
    MLC

    cpp = MLC.to_cpp(source)

    # Should have OR condition
    assert_includes cpp, "||"
    assert_includes cpp, "std::holds_alternative<Red>"
    assert_includes cpp, "std::holds_alternative<Blue>"
  end

  # Literal or-patterns (1 | 2 | 3) require separate implementation
  # for match on primitive types. Skipping for now.
  # def test_or_pattern_with_literals
  #   source = <<~MLC
  #     fn classify(n: i32) -> str =
  #       match n {
  #         1 | 2 | 3 => "small",
  #         _ => "other"
  #       }
  #   MLC
  #
  #   cpp = MLC.to_cpp(source)
  #   assert_includes cpp, "||"
  # end

  def test_multiple_or_patterns_in_same_match
    source = <<~MLC
      type Status = Ok | Pending | Failed

      fn is_done(s: Status) -> bool =
        match s
          | Ok | Failed => true
          | _ => false
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "std::holds_alternative<Ok>"
    assert_includes cpp, "std::holds_alternative<Failed>"
  end

  def test_simple_pattern_without_or
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn is_some(opt: Option<i32>) -> bool =
        match opt
          | Some(_) => true
          | None => false
    MLC

    # Should not be detected as or-pattern
    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "is_some" }
    match_expr = func.body

    first_arm = match_expr.arms[0]
    assert_equal :constructor, first_arm[:pattern].kind
    refute_equal :or, first_arm[:pattern].kind
  end
end

# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

# Pattern Matching E2E Tests
#
# Current status: All tests skipped due to compiler limitations
# The pattern matching generates std::visit code that has issues with:
# - Return type consistency (float vs double)
# - Nullary variants (need constructor syntax)
# - Integer matching (std::visit on non-variant)
# - Pattern guards (.value accessor)
#
# These tests document expected behavior for future implementation.
class PatternMatchingE2ETest < Minitest::Test
  tag :slow  # Requires C++ compilation

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code, &block)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)

      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  # === All tests skipped - pattern matching needs compiler fixes ===

  def test_match_simple_variant_with_data
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Shape = Circle(i32) | Square(i32)

      fn area(s: Shape) -> i32 = match s
        | Circle(r) => 3 * r * r
        | Square(side) => side * side

      fn main() -> i32 = do
        let c = Circle(2)
        let sq = Square(3)
        area(c) + area(sq)
      end
    MLC
      assert_equal 21, status.exitstatus  # 12 + 9
    end
  end

  def test_match_with_wildcard
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Option = Some(i32) | None

      fn get_value(opt: Option) -> i32 = match opt
        | Some(x) => x
        | _ => 0

      fn main() -> i32 = do
        let a = Some(42)
        let b = None
        get_value(a) + get_value(b)
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_match_literal_integers
    run_mlc(<<~MLC) do |stdout, stderr, status|
      fn classify(x: i32) -> i32 = match x
        | 0 => 10
        | 1 => 20
        | 2 => 30
        | _ => 5

      fn main() -> i32 = classify(0) + classify(1) + classify(7)
    MLC
      assert_equal 35, status.exitstatus  # 10 + 20 + 5
    end
  end

  def test_match_with_guard
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Option = Some(i32) | None

      fn get_positive(opt: Option) -> i32 = match opt
        | Some(x) if x > 0 => x
        | Some(x) => 0
        | None => -1

      fn main() -> i32 = do
        let a = Some(5)
        let b = Some(-3)
        let c = None
        get_positive(a) + get_positive(b) + get_positive(c)
      end
    MLC
      assert_equal 4, status.exitstatus  # 5 + 0 + (-1)
    end
  end

  def test_match_nested_option
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Option = Some(i32) | None
      type NestedOption = Nested(Option) | Empty

      fn unwrap_nested(x: NestedOption) -> i32 = match x
        | Nested(Some(v)) => v
        | Nested(None) => 0
        | Empty => -1

      fn main() -> i32 = do
        let a = Nested(Some(42))
        let b = Nested(None)
        let c = Empty
        unwrap_nested(a) + unwrap_nested(b) + unwrap_nested(c)
      end
    MLC
      assert_equal 41, status.exitstatus  # 42 + 0 + (-1)
    end
  end

  def test_match_or_pattern
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Color = Red | Green | Blue

      fn is_warm(c: Color) -> i32 = match c
        | Red | Green => 1
        | Blue => 0

      fn main() -> i32 = do
        let r = Red
        let g = Green
        let b = Blue
        is_warm(r) + is_warm(g) + is_warm(b)
      end
    MLC
      assert_equal 2, status.exitstatus  # 1 + 1 + 0
    end
  end

  def test_match_nullary_variants
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Color = Red | Green | Blue

      fn to_code(c: Color) -> i32 = match c
        | Red => 1
        | Green => 2
        | Blue => 3

      fn main() -> i32 = to_code(Red)
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_match_expression_result
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Status = Active | Inactive | Pending

      fn check_status(s: Status) -> i32 = do
        let result = match s
          | Active => 1
          | Inactive => 0
          | Pending => -1
        result * 10
      end

      fn main() -> i32 = check_status(Active) + check_status(Inactive)
    MLC
      assert_equal 10, status.exitstatus  # 1*10 + 0*10
    end
  end
end

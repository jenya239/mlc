# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

# Pattern Matching E2E Tests
class PatternMatchingE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  def test_match_simple_variant_with_data
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
      assert_equal 4, status.exitstatus # 5 + 0 + (-1)
    end
  end

  def test_match_nested_option
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
      assert_equal 41, status.exitstatus # 42 + 0 + (-1)
    end
  end

  def test_match_or_pattern
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
      assert_equal 2, status.exitstatus # 1 + 1 + 0
    end
  end

  def test_match_nullary_variants
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
      assert_equal 10, status.exitstatus # 1*10 + 0*10
    end
  end

  def test_match_float_literals
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn classify(x: f32) -> i32 = match x
        | 0.0 => 1
        | 1.5 => 2
        | 3.14 => 3
        | _ => 0

      fn main() -> i32 = classify(0.0) + classify(1.5) + classify(2.0)
    MLC
      assert_equal 3, status.exitstatus # 1 + 2 + 0
    end
  end

  def test_match_boolean_literals
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn classify(b: bool) -> i32 = match b
        | true => 10
        | false => 5

      fn main() -> i32 = classify(true) + classify(false)
    MLC
      assert_equal 15, status.exitstatus  # 10 + 5
    end
  end

  def test_match_mixed_literals
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn test_int(x: i32) -> i32 = match x
        | 0 => 1
        | 1 => 10
        | _ => 0

      fn test_float(x: f32) -> i32 = match x
        | 0.0 => 100
        | _ => 0

      fn test_bool(b: bool) -> i32 = match b
        | true => 1000
        | false => 0

      fn main() -> i32 = test_int(1) + test_float(0.0) + test_bool(true)
    MLC
      assert_equal 86, status.exitstatus  # 10 + 100 + 1000 = 1110, wraps to 86 (1110 % 256)
    end
  end

  def test_match_multi_field_variant
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Expr = Add(i32, i32) | Mul(i32, i32) | Lit(i32)

      fn eval(e: Expr) -> i32 = match e
        | Add(a, b) => a + b
        | Mul(a, b) => a * b
        | Lit(v) => v

      fn main() -> i32 = do
        let x = Add(3, 4)
        let y = Mul(2, 5)
        let z = Lit(1)
        eval(x) + eval(y) + eval(z)
      end
    MLC
      assert_equal 18, status.exitstatus  # 7 + 10 + 1
    end
  end

  def test_match_variant_binding_in_arithmetic
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Wrapped = Val(i32) | Zero

      fn double(w: Wrapped) -> i32 = match w
        | Val(x) => x * 2
        | Zero => 0

      fn main() -> i32 = do
        let a = Val(7)
        let b = Zero
        double(a) + double(b)
      end
    MLC
      assert_equal 14, status.exitstatus
    end
  end

  def test_match_multiple_sum_types
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Direction = North | South | East | West

      fn dx(d: Direction) -> i32 = match d
        | East => 1
        | West => -1
        | _ => 0

      fn dy(d: Direction) -> i32 = match d
        | North => 1
        | South => -1
        | _ => 0

      fn main() -> i32 = do
        let d1 = East
        let d2 = North
        dx(d1) + dy(d2)
      end
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  def test_match_chain_functions
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Opt = Some(i32) | None

      fn unwrap_or(o: Opt, default: i32) -> i32 = match o
        | Some(v) => v
        | None => default

      fn add_opt(a: Opt, b: Opt) -> i32 = unwrap_or(a, 0) + unwrap_or(b, 0)

      fn main() -> i32 = do
        let x = Some(10)
        let y = None
        add_opt(x, y)
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end
end

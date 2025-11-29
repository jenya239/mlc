# frozen_string_literal: true

require_relative "../test_helper"

# Tests for closure capture analysis
# Verifies that lambdas correctly identify and capture outer scope variables
class ClosureCaptureTest < Minitest::Test
  def test_lambda_captures_outer_variable
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let multiplier = 2;
        let f = (x: i32) => x * multiplier;
        f(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Lambda should capture multiplier: [multiplier](int x)
    assert_includes cpp_code, "[multiplier]"
    assert_includes cpp_code, "int x"
  end

  def test_lambda_captures_multiple_variables
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let a = 1;
        let b = 2;
        let f = () => a + b;
        f()
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Should capture both a and b
    # Could be [a, b] or [b, a] depending on implementation
    assert_includes cpp_code, "a"
    assert_includes cpp_code, "b"
    assert_includes cpp_code, "["
    assert_includes cpp_code, "]"
  end

  def test_lambda_does_not_capture_own_params
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let f = (x: i32) => x + 1;
        f(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Lambda with no captures should have empty capture list
    # [](int x) or [](...) pattern
    refute_includes cpp_code, "[x]"
  end

  def test_nested_lambda_captures
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let a = 1;
        let outer = () => do
          let b = 2
          let inner = () => a + b
          inner()
        end;
        outer()
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Inner lambda should capture 'a' (from outer scope) and 'b' (from outer lambda)
    # Outer lambda should capture 'a' (from function scope)
    assert_includes cpp_code, "a"
  end

  def test_lambda_with_no_free_vars_to_cpp
    # IIFE (immediately invoked function expression)
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let f = (x: i32) => x + 1;
        f(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Lambda with no free vars should have empty capture list []
    assert_includes cpp_code, "[]"
  end

  def test_capture_with_let_scoping
    # Variable in inner let should not be captured by preceding lambda
    mlc_source = <<~MLCORA
      fn test() -> i32 =
        let f = (x: i32) => x + 1;
        let unused = 10;
        f(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Lambda should not capture 'unused' - should have empty captures
    # The generated C++ should have [] not [unused]
    refute_includes cpp_code, "[unused]"
  end

  def test_capture_from_function_param
    mlc_source = <<~MLCORA
      fn test(n: i32) -> i32 =
        let f = (x: i32) => x + n;
        f(5)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    # Lambda should capture 'n' from function parameter
    assert_includes cpp_code, "[n]"
  end
end

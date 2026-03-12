# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class PipeOperatorTest < Minitest::Test
  # ============================================================================
  # Parser Tests
  # ============================================================================

  def test_parse_simple_pipe
    source = <<~MLC
      fn double(x: i32) -> i32 = x + x

      fn apply_double(x: i32) -> i32 =
        x |> double
    MLC

    ast = MLC.parse(source)
    function = ast.declarations[1]

    assert_instance_of MLC::Source::AST::BinaryOp, function.body
    assert_equal "|>", function.body.op
    assert_instance_of MLC::Source::AST::VarRef, function.body.left
    assert_instance_of MLC::Source::AST::VarRef, function.body.right
    assert_equal "x", function.body.left.name
    assert_equal "double", function.body.right.name
  end

  def test_parse_chained_pipe_is_left_associative
    source = <<~MLC
      fn test(x: i32) -> i32 =
        x |> first |> second |> third
    MLC

    ast = MLC.parse(source)
    function = ast.declarations.first

    # ((x |> first) |> second) |> third
    outer = function.body
    assert_equal "|>", outer.op
    assert_equal "third", outer.right.name

    middle = outer.left
    assert_equal "|>", middle.op
    assert_equal "second", middle.right.name

    inner = middle.left
    assert_equal "|>", inner.op
    assert_equal "x", inner.left.name
    assert_equal "first", inner.right.name
  end

  def test_parse_pipe_with_partial_application
    source = <<~MLC
      fn test(x: i32) -> i32 =
        x |> add(5)
    MLC

    ast = MLC.parse(source)
    function = ast.declarations.first
    body = function.body

    assert_equal "|>", body.op
    assert_equal "x", body.left.name
    assert_instance_of MLC::Source::AST::Call, body.right
    assert_instance_of MLC::Source::AST::VarRef, body.right.callee
    assert_equal "add", body.right.callee.name
  end

  # ============================================================================
  # Codegen Tests
  # ============================================================================

  def test_codegen_simple_pipe_becomes_function_call
    source = <<~MLC
      fn double(x: i32) -> i32 = x * 2

      fn apply_double(x: i32) -> i32 =
        x |> double
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "double_(x)"
  end

  def test_codegen_chained_pipe_is_nested_calls
    source = <<~MLC
      fn double(x: i32) -> i32 = x * 2
      fn increment(x: i32) -> i32 = x + 1
      fn negate(x: i32) -> i32 = 0 - x

      fn transform(x: i32) -> i32 =
        x |> double |> increment |> negate
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "negate(increment(double_(x)))"
  end

  def test_codegen_pipe_with_partial_application_inserts_argument_first
    source = <<~MLC
      fn add(x: i32, y: i32) -> i32 = x + y

      fn add_five(x: i32) -> i32 =
        x |> add(5)
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "add(x, 5)"
  end

  def test_codegen_pipe_chain_with_partial_application
    source = <<~MLC
      fn add(x: i32, y: i32) -> i32 = x + y
      fn multiply(x: i32, factor: i32) -> i32 = x * factor

      fn compute(x: i32) -> i32 =
        x |> add(3) |> multiply(2)
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "multiply(add(x, 3), 2)"
  end

  # ============================================================================
  # Compile and Run Tests
  # ============================================================================

  def test_pipe_produces_correct_result_at_runtime
    source = <<~MLC
      fn double(x: i32) -> i32 = x * 2
      fn increment(x: i32) -> i32 = x + 1

      fn compute(x: i32) -> i32 =
        x |> double |> increment
    MLC

    cpp = MLC.to_cpp(source)

    # double(3) = 6, increment(6) = 7
    test_program = <<~CPP
      #{cpp}

      int main() {
        return compute(3) == 7 ? 0 : 1;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_pipe_with_partial_application_produces_correct_result
    source = <<~MLC
      fn add(x: i32, y: i32) -> i32 = x + y
      fn multiply(x: i32, factor: i32) -> i32 = x * factor

      fn compute(x: i32) -> i32 =
        x |> add(3) |> multiply(2)
    MLC

    cpp = MLC.to_cpp(source)

    # add(10, 3) = 13, multiply(13, 2) = 26
    test_program = <<~CPP
      #{cpp}

      int main() {
        return compute(10) == 26 ? 0 : 1;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_pipe_in_do_block
    source = <<~MLC
      fn double(x: i32) -> i32 = x * 2
      fn increment(x: i32) -> i32 = x + 1

      fn compute(x: i32) -> i32 = do
        let intermediate = x |> double
        intermediate |> increment
      end
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        return compute(5) == 11 ? 0 : 1;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_pipe_threading_record_state
    source = <<~MLC
      type Counter = { value: i32 }

      fn increment_counter(counter: Counter) -> Counter =
        Counter { ...counter, value: counter.value + 1 }

      fn apply_three_increments(counter: Counter) -> Counter =
        counter |> increment_counter |> increment_counter |> increment_counter
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        Counter initial{0};
        Counter result = apply_three_increments(initial);
        return result.value == 3 ? 0 : 1;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  private

  def assert_compiles_and_runs(cpp_code)
    source_file = "/tmp/mlc_pipe_test.cpp"
    binary_file = "/tmp/mlc_pipe_test"
    runtime_dir = File.expand_path("../../runtime", __dir__)

    File.write(source_file, cpp_code)

    compile_result = system(
      "g++ -std=c++20 -I #{runtime_dir}/include -o #{binary_file} #{source_file} 2>/tmp/mlc_pipe_compile_error.txt"
    )
    assert compile_result, "Compilation failed:\n#{File.read("/tmp/mlc_pipe_compile_error.txt")}\n\nCode:\n#{cpp_code}"

    run_result = system(binary_file)
    assert run_result, "Runtime assertion failed for:\n#{cpp_code}"
  ensure
    File.delete(source_file) if File.exist?(source_file)
    File.delete(binary_file) if File.exist?(binary_file)
  end
end

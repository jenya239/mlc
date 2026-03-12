# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class RecordUpdateTest < Minitest::Test
  # ============================================================================
  # Parser Tests
  # ============================================================================

  def test_parse_record_update_has_spread
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn move_x(point: Point, delta: i32) -> Point =
        Point { ...point, x: point.x + delta }
    MLC

    ast = MLC.parse(source)
    function = ast.declarations[1]
    record = function.body

    assert_instance_of MLC::Source::AST::RecordLit, record
    assert_equal "Point", record.type_name
    assert_equal 1, record.spreads.size
    assert_equal 1, record.fields.size
    assert record.fields.key?("x")
  end

  def test_parse_record_update_spread_references_original
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn move_x(point: Point, delta: i32) -> Point =
        Point { ...point, x: point.x + delta }
    MLC

    ast = MLC.parse(source)
    function = ast.declarations[1]
    record = function.body

    spread_expr = record.spreads.first[:expr]
    assert_instance_of MLC::Source::AST::VarRef, spread_expr
    assert_equal "point", spread_expr.name
  end

  def test_parse_record_update_multiple_overrides
    source = <<~MLC
      type State = { x: i32, y: i32, z: i32 }

      fn reset_xy(state: State) -> State =
        State { ...state, x: 0, y: 0 }
    MLC

    ast = MLC.parse(source)
    function = ast.declarations[1]
    record = function.body

    assert_equal 1, record.spreads.size
    assert_equal 2, record.fields.size
    assert record.fields.key?("x")
    assert record.fields.key?("y")
    refute record.fields.key?("z")
  end

  # ============================================================================
  # Codegen Tests
  # ============================================================================

  def test_codegen_record_update_preserves_unchanged_fields
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn move_x(point: Point, delta: i32) -> Point =
        Point { ...point, x: point.x + delta }
    MLC

    cpp = MLC.to_cpp(source)

    # Must include the spread field (y is preserved from point)
    assert_includes cpp, "point.y"
  end

  def test_codegen_record_update_uses_override_value
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn move_x(point: Point, delta: i32) -> Point =
        Point { ...point, x: point.x + delta }
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "point.x + delta"
  end

  def test_codegen_record_update_with_all_fields_overridden
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn origin(point: Point) -> Point =
        Point { ...point, x: 0, y: 0 }
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "Point{0, 0}"
  end

  # ============================================================================
  # Compile and Run Tests
  # ============================================================================

  def test_record_update_preserves_unchanged_field_at_runtime
    source = <<~MLC
      type Point = { x: i32, y: i32 }

      fn move_x(point: Point, delta: i32) -> Point =
        Point { ...point, x: point.x + delta }
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        Point original{10, 20};
        Point moved = move_x(original, 5);
        if (moved.x != 15) return 1;
        if (moved.y != 20) return 2;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_record_update_with_multiple_overrides_at_runtime
    source = <<~MLC
      type State = { x: i32, y: i32, z: i32 }

      fn reset_xy(state: State) -> State =
        State { ...state, x: 0, y: 0 }
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        State original{10, 20, 30};
        State updated = reset_xy(original);
        if (updated.x != 0) return 1;
        if (updated.y != 0) return 2;
        if (updated.z != 30) return 3;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_chained_record_updates_at_runtime
    source = <<~MLC
      type Vector3 = { x: i32, y: i32, z: i32 }

      fn set_x(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, x: value }

      fn set_y(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, y: value }

      fn set_z(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, z: value }
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        Vector3 origin{0, 0, 0};
        Vector3 result = set_z(set_y(set_x(origin, 1), 2), 3);
        if (result.x != 1) return 1;
        if (result.y != 2) return 2;
        if (result.z != 3) return 3;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_record_update_combined_with_pipe_at_runtime
    source = <<~MLC
      type Vector3 = { x: i32, y: i32, z: i32 }

      fn set_x(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, x: value }

      fn set_y(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, y: value }

      fn set_z(vector: Vector3, value: i32) -> Vector3 =
        Vector3 { ...vector, z: value }

      fn build_vector(origin: Vector3) -> Vector3 =
        origin |> set_x(1) |> set_y(2) |> set_z(3)
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        Vector3 origin{0, 0, 0};
        Vector3 result = build_vector(origin);
        if (result.x != 1) return 1;
        if (result.y != 2) return 2;
        if (result.z != 3) return 3;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_record_update_in_do_block_at_runtime
    source = <<~MLC
      type Counter = { value: i32, step: i32 }

      fn advance(counter: Counter) -> Counter = do
        let new_value = counter.value + counter.step
        Counter { ...counter, value: new_value }
      end
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        Counter counter{0, 5};
        Counter advanced = advance(counter);
        if (advanced.value != 5) return 1;
        if (advanced.step != 5) return 2;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  def test_immutable_state_threading_pattern
    source = <<~MLC
      type LexerState = { source: string, position: i32, line: i32, column: i32 }

      fn advance_position(state: LexerState) -> LexerState =
        LexerState { ...state, position: state.position + 1, column: state.column + 1 }

      fn advance_line(state: LexerState) -> LexerState =
        LexerState { ...state, position: state.position + 1, line: state.line + 1, column: 0 }
    MLC

    cpp = MLC.to_cpp(source)

    test_program = <<~CPP
      #{cpp}

      int main() {
        LexerState initial{"hello", 0, 1, 0};
        LexerState after_advance = advance_position(initial);
        if (after_advance.position != 1) return 1;
        if (after_advance.column != 1) return 2;
        if (after_advance.line != 1) return 3;

        LexerState after_newline = advance_line(initial);
        if (after_newline.position != 1) return 4;
        if (after_newline.line != 2) return 5;
        if (after_newline.column != 0) return 6;
        return 0;
      }
    CPP

    assert_compiles_and_runs(test_program)
  end

  private

  def assert_compiles_and_runs(cpp_code)
    source_file = "/tmp/mlc_record_update_test.cpp"
    binary_file = "/tmp/mlc_record_update_test"
    runtime_dir = File.expand_path("../../runtime", __dir__)

    File.write(source_file, cpp_code)

    compile_result = system(
      "g++ -std=c++20 -I #{runtime_dir}/include -o #{binary_file} #{source_file} 2>/tmp/mlc_record_update_compile_error.txt"
    )
    assert compile_result, "Compilation failed:\n#{File.read("/tmp/mlc_record_update_compile_error.txt")}\n\nCode:\n#{cpp_code}"

    run_result = system(binary_file)
    assert run_result, "Runtime assertion failed"
  ensure
    File.delete(source_file) if File.exist?(source_file)
    File.delete(binary_file) if File.exist?(binary_file)
  end
end

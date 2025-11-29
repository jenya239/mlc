# frozen_string_literal: true

require_relative "../../test_helper"

class TypeConstraintSolverTest < Minitest::Test
  def setup
    @compatible_calls = []
    @type_errors = []

    @ensure_compatible = ->(actual, expected, context) {
      @compatible_calls << { actual: actual, expected: expected, context: context }
    }

    @type_error = ->(message) {
      @type_errors << message
      raise MLC::CompileError, message
    }
  end

  def test_solve_returns_instantiation_struct
    solver = create_solver(
      infer: ->(type_params, param_types, arg_types) { { "T" => "i32" } },
      substitute: ->(type, map) { map[type] || type }
    )

    info = MockFunctionInfo.new(
      type_params: [MockTypeParam.new("T")],
      param_types: ["T"],
      ret_type: "T"
    )

    arg = MockArg.new("i32")

    result = solver.solve(info, [arg], name: "identity")

    assert_instance_of MLC::Common::Typing::TypeConstraintSolver::Instantiation, result
    assert_equal({ "T" => "i32" }, result.type_map)
    assert_equal ["i32"], result.param_types
    assert_equal "i32", result.ret_type
  end

  def test_solve_checks_argument_count
    solver = create_solver(
      infer: ->(*) { {} },
      substitute: ->(type, _map) { type }
    )

    info = MockFunctionInfo.new(
      type_params: [],
      param_types: ["i32", "i32"],
      ret_type: "i32"
    )

    arg = MockArg.new("i32")

    assert_raises(MLC::CompileError) do
      solver.solve(info, [arg], name: "add")
    end

    assert_includes @type_errors.first, "expects 2 argument(s), got 1"
  end

  def test_solve_checks_type_compatibility
    solver = create_solver(
      infer: ->(*) { {} },
      substitute: ->(type, _map) { type }
    )

    info = MockFunctionInfo.new(
      type_params: [],
      param_types: ["i32"],
      ret_type: "i32"
    )

    arg = MockArg.new("str")

    solver.solve(info, [arg], name: "foo")

    assert_equal 1, @compatible_calls.size
    assert_equal "str", @compatible_calls.first[:actual]
    assert_equal "i32", @compatible_calls.first[:expected]
    assert_equal "argument 1 of 'foo'", @compatible_calls.first[:context]
  end

  def test_solve_substitutes_generic_types
    solver = create_solver(
      infer: ->(type_params, param_types, arg_types) { { "T" => "f32" } },
      substitute: ->(type, map) { map[type] || type }
    )

    info = MockFunctionInfo.new(
      type_params: [MockTypeParam.new("T")],
      param_types: ["T", "T"],
      ret_type: "T"
    )

    args = [MockArg.new("f32"), MockArg.new("f32")]

    result = solver.solve(info, args, name: "max")

    assert_equal ["f32", "f32"], result.param_types
    assert_equal "f32", result.ret_type
  end

  def test_solve_with_expected_return_type_bidirectional_inference
    type_variable = MLC::SemanticIR::TypeVariable.new(name: "T")

    solver = create_solver(
      infer: ->(type_params, param_types, arg_types) { {} }, # Returns empty - can't infer from args
      substitute: ->(type, map) {
        if type.is_a?(MLC::SemanticIR::TypeVariable)
          map[type.name] || type
        else
          type
        end
      }
    )

    info = MockFunctionInfo.new(
      type_params: [MockTypeParam.new("T")],
      param_types: [],
      ret_type: type_variable
    )

    result = solver.solve(info, [], name: "default", expected_ret_type: "i32")

    assert_equal({ "T" => "i32" }, result.type_map)
  end

  def test_solve_assigns_expression_types
    assigned_types = []
    assign_type = ->(expr, type) { assigned_types << { expr: expr, type: type } }

    solver = create_solver(
      infer: ->(*) { {} },
      substitute: ->(type, _map) { type },
      assign_type: assign_type
    )

    info = MockFunctionInfo.new(
      type_params: [],
      param_types: ["i32"],
      ret_type: "i32"
    )

    arg = MockArg.new("i32")

    solver.solve(info, [arg], name: "inc")

    assert_equal 1, assigned_types.size
    assert_equal arg, assigned_types.first[:expr]
    assert_equal "i32", assigned_types.first[:type]
  end

  def test_incomplete_type_map_returns_partially_solved_when_no_expected_type
    type_variable = MLC::SemanticIR::TypeVariable.new(name: "T")

    solver = create_solver(
      infer: ->(type_params, param_types, arg_types) {
        # Only partial inference - T remains a TypeVariable
        { "T" => type_variable }
      },
      substitute: ->(type, map) {
        if type.is_a?(MLC::SemanticIR::TypeVariable)
          map[type.name] || type
        else
          type
        end
      }
    )

    info = MockFunctionInfo.new(
      type_params: [MockTypeParam.new("T")],
      param_types: [],
      ret_type: type_variable
    )

    # Without expected return type, type_map["T"] remains as TypeVariable
    result = solver.solve(info, [], name: "default")

    # Type remains as TypeVariable when incomplete
    assert_instance_of MLC::SemanticIR::TypeVariable, result.type_map["T"]
  end

  private

  def create_solver(infer:, substitute:, assign_type: nil)
    MLC::Common::Typing::TypeConstraintSolver.new(
      infer_type_arguments: infer,
      substitute_type: substitute,
      ensure_compatible_type: @ensure_compatible,
      type_error: @type_error,
      assign_expression_type: assign_type
    )
  end

  # Mock helpers
  MockFunctionInfo = Struct.new(:type_params, :param_types, :ret_type, keyword_init: true)
  MockTypeParam = Struct.new(:name)
  MockArg = Struct.new(:type)
end

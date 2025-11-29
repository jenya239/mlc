# frozen_string_literal: true

require_relative "../../test_helper"

class GenericCallResolverTest < Minitest::Test
  def test_instantiate_delegates_to_constraint_solver
    # Track calls to the constraint solver
    solve_calls = []
    mock_solver = MockConstraintSolver.new { |args| solve_calls << args }

    resolver = MLC::Common::Typing::GenericCallResolver.new(
      constraint_solver: mock_solver
    )

    info = { type_params: ["T"], param_types: ["T"], ret_type: "T" }
    args = [{ type: "i32" }]

    resolver.instantiate(info, args, name: "identity")

    assert_equal 1, solve_calls.size
    assert_equal info, solve_calls.first[:info]
    assert_equal args, solve_calls.first[:args]
    assert_equal "identity", solve_calls.first[:name]
  end

  def test_instantiate_passes_expected_ret_type
    solve_calls = []
    mock_solver = MockConstraintSolver.new { |args| solve_calls << args }

    resolver = MLC::Common::Typing::GenericCallResolver.new(
      constraint_solver: mock_solver
    )

    info = { type_params: ["T"], param_types: [], ret_type: "T" }

    resolver.instantiate(info, [], name: "default", expected_ret_type: "i32")

    assert_equal 1, solve_calls.size
    assert_equal "i32", solve_calls.first[:expected_ret_type]
  end

  def test_instantiate_returns_solver_result
    expected_result = MockInstantiation.new(
      type_map: { "T" => "f64" },
      param_types: ["f64"],
      ret_type: "f64"
    )

    mock_solver = MockConstraintSolver.new { expected_result }

    resolver = MLC::Common::Typing::GenericCallResolver.new(
      constraint_solver: mock_solver
    )

    info = { type_params: ["T"], param_types: ["T"], ret_type: "T" }
    args = [{ type: "f64" }]

    result = resolver.instantiate(info, args, name: "identity")

    assert_equal expected_result, result
    assert_equal({ "T" => "f64" }, result.type_map)
    assert_equal ["f64"], result.param_types
    assert_equal "f64", result.ret_type
  end

  def test_instantiate_with_multiple_type_params
    solve_calls = []
    mock_solver = MockConstraintSolver.new { |args| solve_calls << args }

    resolver = MLC::Common::Typing::GenericCallResolver.new(
      constraint_solver: mock_solver
    )

    info = {
      type_params: ["T", "U"],
      param_types: ["T", "U"],
      ret_type: "Pair<T, U>"
    }
    args = [{ type: "i32" }, { type: "str" }]

    resolver.instantiate(info, args, name: "make_pair")

    assert_equal 1, solve_calls.size
    assert_equal info, solve_calls.first[:info]
    assert_equal args, solve_calls.first[:args]
    assert_equal "make_pair", solve_calls.first[:name]
  end

  def test_instantiate_with_nil_expected_ret_type
    solve_calls = []
    mock_solver = MockConstraintSolver.new { |args| solve_calls << args }

    resolver = MLC::Common::Typing::GenericCallResolver.new(
      constraint_solver: mock_solver
    )

    info = { type_params: ["T"], param_types: ["T"], ret_type: "T" }
    args = [{ type: "bool" }]

    resolver.instantiate(info, args, name: "id")

    assert_equal 1, solve_calls.size
    assert_nil solve_calls.first[:expected_ret_type]
  end

  private

  class MockConstraintSolver
    def initialize(&block)
      @block = block
    end

    def solve(info, args, name:, expected_ret_type: nil)
      @block.call({
        info: info,
        args: args,
        name: name,
        expected_ret_type: expected_ret_type
      })
    end
  end

  MockInstantiation = Struct.new(:type_map, :param_types, :ret_type, keyword_init: true)
end

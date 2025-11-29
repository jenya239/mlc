# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the service and its dependencies
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/inference/type_inference_service"
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/checkers/type_checker"

class TypeInferenceServiceTest < Minitest::Test
  def setup
    @var_type_registry = MockVarTypeRegistry.new
    @type_registry = MockTypeRegistry.new
    @function_registry = MockFunctionRegistry.new
    @type_decl_table = {}
    @generic_call_resolver = MockGenericCallResolver.new
    @type_checker = MockTypeChecker.new
    @scope_context = MockScopeContext.new

    @service = MLC::Representations::Semantic::Gen::Services::TypeInferenceService.new(
      var_type_registry: @var_type_registry,
      type_registry: @type_registry,
      function_registry: @function_registry,
      type_decl_table: @type_decl_table,
      generic_call_resolver: @generic_call_resolver,
      type_checker: @type_checker,
      scope_context: @scope_context
    )
  end

  # ========== infer_variable_type ==========

  def test_infer_variable_type_from_var_registry
    expected_type = MLC::SemanticIR::Builder.primitive_type("i32")
    @var_type_registry.set("x", expected_type)

    result = @service.infer_variable_type("x")

    assert_equal expected_type, result
  end

  def test_infer_variable_type_from_function_registry
    ret_type = MLC::SemanticIR::Builder.primitive_type("i32")
    info = MockFunctionInfo.new(
      type_params: [],
      param_types: [MLC::SemanticIR::Builder.primitive_type("i32")],
      ret_type: ret_type
    )
    @function_registry.register("add", info)

    result = @service.infer_variable_type("add")

    assert_instance_of MLC::SemanticIR::FunctionType, result
    assert_equal ret_type, result.ret_type
  end

  def test_infer_variable_type_true_false_returns_bool
    result_true = @service.infer_variable_type("true")
    result_false = @service.infer_variable_type("false")

    assert_equal "bool", result_true.name
    assert_equal "bool", result_false.name
  end

  def test_infer_variable_type_unknown_raises_error
    @type_checker.expect_type_error = true

    assert_raises(MLC::CompileError) do
      @service.infer_variable_type("unknown_var")
    end
  end

  # ========== infer_binary_type ==========

  def test_infer_binary_type_addition_numeric
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @service.infer_binary_type("+", i32, i32)

    assert_equal "i32", result.name
  end

  def test_infer_binary_type_addition_string
    str = MLC::SemanticIR::Builder.primitive_type("string")

    result = @service.infer_binary_type("+", str, str)

    assert_equal "string", result.name
  end

  def test_infer_binary_type_comparison_returns_bool
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    %w[== != < > <= >=].each do |op|
      result = @service.infer_binary_type(op, i32, i32)
      assert_equal "bool", result.name, "Expected bool for '#{op}'"
    end
  end

  def test_infer_binary_type_logical_returns_bool
    bool = MLC::SemanticIR::Builder.primitive_type("bool")

    %w[&& ||].each do |op|
      result = @service.infer_binary_type(op, bool, bool)
      assert_equal "bool", result.name, "Expected bool for '#{op}'"
    end
  end

  def test_infer_binary_type_division_float_result
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    f32 = MLC::SemanticIR::Builder.primitive_type("f32")

    result = @service.infer_binary_type("/", i32, f32)

    assert_equal "f32", result.name
  end

  # ========== infer_unary_type ==========

  def test_infer_unary_type_negation_preserves_type
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @service.infer_unary_type("-", i32)

    assert_equal i32, result
  end

  def test_infer_unary_type_not_returns_bool
    bool = MLC::SemanticIR::Builder.primitive_type("bool")

    result = @service.infer_unary_type("!", bool)

    assert_equal "bool", result.name
  end

  # ========== numeric_type? / string_type? / void_type? ==========

  def test_numeric_type_predicate
    assert @service.numeric_type?(MLC::SemanticIR::Builder.primitive_type("i32"))
    assert @service.numeric_type?(MLC::SemanticIR::Builder.primitive_type("f32"))
    refute @service.numeric_type?(MLC::SemanticIR::Builder.primitive_type("bool"))
    refute @service.numeric_type?(MLC::SemanticIR::Builder.primitive_type("string"))
  end

  def test_string_type_predicate
    assert @service.string_type?(MLC::SemanticIR::Builder.primitive_type("string"))
    assert @service.string_type?(MLC::SemanticIR::Builder.primitive_type("str"))
    refute @service.string_type?(MLC::SemanticIR::Builder.primitive_type("i32"))
  end

  def test_void_type_predicate
    assert @service.void_type?(MLC::SemanticIR::Builder.primitive_type("void"))
    assert @service.void_type?(MLC::SemanticIR::UnitType.new)
    refute @service.void_type?(MLC::SemanticIR::Builder.primitive_type("i32"))
  end

  # ========== Type unification ==========

  def test_unify_types_binds_type_variable
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    concrete = MLC::SemanticIR::Builder.primitive_type("i32")
    type_map = {}

    @service.unify_types(type_var, concrete, type_map)

    assert_equal concrete, type_map["T"]
  end

  def test_unify_types_array_element
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    pattern = MLC::SemanticIR::ArrayType.new(element_type: type_var)
    concrete_elem = MLC::SemanticIR::Builder.primitive_type("f32")
    concrete = MLC::SemanticIR::ArrayType.new(element_type: concrete_elem)
    type_map = {}

    @service.unify_types(pattern, concrete, type_map)

    assert_equal concrete_elem, type_map["T"]
  end

  def test_unify_types_generic_type
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    pattern = MLC::SemanticIR::GenericType.new(base_type: base, type_args: [type_var])

    concrete_arg = MLC::SemanticIR::Builder.primitive_type("i32")
    concrete = MLC::SemanticIR::GenericType.new(base_type: base, type_args: [concrete_arg])
    type_map = {}

    @service.unify_types(pattern, concrete, type_map)

    assert_equal concrete_arg, type_map["T"]
  end

  # ========== substitute_type ==========

  def test_substitute_type_variable
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    concrete = MLC::SemanticIR::Builder.primitive_type("i32")
    type_map = { "T" => concrete }

    result = @service.substitute_type(type_var, type_map)

    assert_equal concrete, result
  end

  def test_substitute_type_array
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: type_var)
    concrete = MLC::SemanticIR::Builder.primitive_type("f32")
    type_map = { "T" => concrete }

    result = @service.substitute_type(array_type, type_map)

    assert_instance_of MLC::SemanticIR::ArrayType, result
    assert_equal concrete, result.element_type
  end

  def test_substitute_type_no_change_when_not_in_map
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    type_map = {}

    result = @service.substitute_type(i32, type_map)

    assert_equal i32, result
  end

  # ========== infer_type_arguments ==========

  def test_infer_type_arguments_single_param
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    concrete = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @service.infer_type_arguments(["T"], [type_var], [concrete])

    assert_equal concrete, result["T"]
  end

  def test_infer_type_arguments_multiple_params
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    u_var = MLC::SemanticIR::TypeVariable.new(name: "U")
    t_concrete = MLC::SemanticIR::Builder.primitive_type("i32")
    u_concrete = MLC::SemanticIR::Builder.primitive_type("str")

    result = @service.infer_type_arguments(["T", "U"], [t_var, u_var], [t_concrete, u_concrete])

    assert_equal t_concrete, result["T"]
    assert_equal u_concrete, result["U"]
  end

  # ========== types_compatible? ==========

  def test_types_compatible_same_type
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    assert @service.types_compatible?(i32, i32)
  end

  def test_types_compatible_numeric_flexibility
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    f32 = MLC::SemanticIR::Builder.primitive_type("f32")

    assert @service.types_compatible?(i32, f32)
  end

  def test_types_not_compatible_different_types
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    str = MLC::SemanticIR::Builder.primitive_type("string")

    refute @service.types_compatible?(i32, str)
  end

  # ========== infer_iterable_type ==========

  def test_infer_iterable_type_array
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: element_type)
    mock_iterable = Struct.new(:type).new(array_type)

    result = @service.infer_iterable_type(mock_iterable)

    assert_equal element_type, result
  end

  def test_infer_iterable_type_non_array_raises_error
    @type_checker.expect_type_error = true
    non_array = Struct.new(:type).new(MLC::SemanticIR::Builder.primitive_type("i32"))

    assert_raises(MLC::CompileError) do
      @service.infer_iterable_type(non_array)
    end
  end

  # ========== expected_lambda_param_types ==========

  def test_expected_lambda_param_types_map
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: element_type)
    mock_object = Struct.new(:type).new(array_type)

    result = @service.expected_lambda_param_types(mock_object, "map", [], 0)

    assert_equal [element_type], result
  end

  def test_expected_lambda_param_types_filter
    element_type = MLC::SemanticIR::Builder.primitive_type("str")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: element_type)
    mock_object = Struct.new(:type).new(array_type)

    result = @service.expected_lambda_param_types(mock_object, "filter", [], 0)

    assert_equal [element_type], result
  end

  def test_expected_lambda_param_types_unknown_method
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: element_type)
    mock_object = Struct.new(:type).new(array_type)

    result = @service.expected_lambda_param_types(mock_object, "unknown", [], 0)

    assert_equal [], result
  end

  private

  # ========== Mock Classes ==========

  class MockVarTypeRegistry
    def initialize
      @vars = {}
    end

    def has?(name)
      @vars.key?(name)
    end

    def get(name)
      @vars[name]
    end

    def set(name, type)
      @vars[name] = type
    end

    def keys
      @vars.keys
    end
  end

  class MockTypeRegistry
    def initialize
      @types = {}
    end

    def has_type?(name)
      @types.key?(name)
    end

    def lookup(name)
      @types[name]
    end

    def resolve_member(type_name, member)
      nil
    end
  end

  class MockFunctionRegistry
    def initialize
      @functions = {}
    end

    def fetch(name)
      @functions[name]
    end

    def register(name, info)
      @functions[name] = info
    end
  end

  class MockGenericCallResolver
    def instantiate(info, args, name:, expected_ret_type: nil)
      MLC::Common::Typing::TypeConstraintSolver::Instantiation.new(
        type_map: {},
        param_types: info.param_types,
        ret_type: info.ret_type
      )
    end
  end

  class MockTypeChecker
    attr_accessor :expect_type_error

    IO_RETURN_TYPES = {}.freeze

    def initialize
      @expect_type_error = false
    end

    def type_error(message, node: nil)
      raise MLC::CompileError, message
    end

    def type_name(type)
      return type.name if type.respond_to?(:name)
      type.to_s
    end

    def describe_type(type)
      type_name(type)
    end

    def normalized_type_name(name)
      return nil if name.nil?
      case name
      when "str" then "string"
      else name
      end
    end

    def ensure_type!(type, message, node: nil)
      type_error(message, node: node) unless type
    end

    def ensure_numeric_type(type, context)
      # Accept any type in mock
    end

    def ensure_boolean_type(type, context)
      # Accept any type in mock
    end

    def ensure_compatible_type(actual, expected, context)
      # Accept any types in mock
    end

    def ensure_argument_count(method, args, expected)
      # Accept any count in mock
    end

    def io_return_type(name)
      MLC::SemanticIR::Builder.primitive_type("void")
    end

    def validate_function_call(info, args, name)
      # Accept any call in mock
    end

    def module_member_info(module_name, member)
      nil
    end
  end

  class MockScopeContext
    def current_type_params
      []
    end
  end

  MockFunctionInfo = Struct.new(:type_params, :param_types, :ret_type, keyword_init: true)
end

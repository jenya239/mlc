# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the checker
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/checkers/type_checker"

class TypeCheckerServiceTest < Minitest::Test
  def setup
    @function_registry = MockFunctionRegistry.new

    @checker = MLC::Representations::Semantic::Gen::Services::TypeChecker.new(
      function_registry: @function_registry
    )
  end

  # ========== Type name resolution ==========

  def test_type_name_primitive
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    assert_equal "i32", @checker.type_name(i32)
  end

  def test_type_name_array
    element = MLC::SemanticIR::Builder.primitive_type("i32")
    array = MLC::SemanticIR::ArrayType.new(element_type: element)

    # type_name returns simplified name "array" for ArrayType
    assert_equal "array", @checker.type_name(array)
  end

  def test_normalized_type_name_str_to_string
    assert_equal "string", @checker.normalized_type_name("str")
    assert_equal "string", @checker.normalized_type_name("string")
  end

  def test_normalized_type_name_nil
    assert_nil @checker.normalized_type_name(nil)
  end

  def test_describe_type
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    assert_equal "i32", @checker.describe_type(i32)
  end

  # ========== Type validation ==========

  def test_ensure_boolean_type_passes_for_bool
    bool = MLC::SemanticIR::Builder.primitive_type("bool")

    # Should not raise
    @checker.ensure_boolean_type(bool, "condition")
  end

  def test_ensure_boolean_type_fails_for_non_bool
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    assert_raises(MLC::CompileError) do
      @checker.ensure_boolean_type(i32, "condition")
    end
  end

  def test_ensure_numeric_type_passes_for_i32
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    # Should not raise
    @checker.ensure_numeric_type(i32, "operand")
  end

  def test_ensure_numeric_type_passes_for_f32
    f32 = MLC::SemanticIR::Builder.primitive_type("f32")

    # Should not raise
    @checker.ensure_numeric_type(f32, "operand")
  end

  def test_ensure_numeric_type_fails_for_string
    str = MLC::SemanticIR::Builder.primitive_type("string")

    assert_raises(MLC::CompileError) do
      @checker.ensure_numeric_type(str, "operand")
    end
  end

  def test_ensure_compatible_type_same_types
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    # Should not raise
    @checker.ensure_compatible_type(i32, i32, "assignment")
  end

  def test_ensure_compatible_type_different_types
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    str = MLC::SemanticIR::Builder.primitive_type("string")

    assert_raises(MLC::CompileError) do
      @checker.ensure_compatible_type(i32, str, "assignment")
    end
  end

  def test_ensure_compatible_type_skips_auto
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    auto = MLC::SemanticIR::Builder.primitive_type("auto")

    # Should not raise for auto
    @checker.ensure_compatible_type(i32, auto, "assignment")
    @checker.ensure_compatible_type(auto, i32, "assignment")
  end

  def test_ensure_compatible_type_skips_type_variable
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")

    # Should not raise for TypeVariable in expected
    @checker.ensure_compatible_type(i32, type_var, "assignment")
  end

  def test_ensure_compatible_type_skips_error_type
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    error_type = MLC::SemanticIR::ErrorType.new(error_message: "test")

    # Should not raise for ErrorType (prevents cascading errors)
    @checker.ensure_compatible_type(error_type, i32, "assignment")
    @checker.ensure_compatible_type(i32, error_type, "assignment")
  end

  def test_ensure_type_raises_for_nil
    assert_raises(MLC::CompileError) do
      @checker.ensure_type!(nil, "expected type")
    end
  end

  def test_ensure_type_passes_for_valid_type
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    # Should not raise
    @checker.ensure_type!(i32, "expected type")
  end

  # ========== Type predicates ==========

  def test_numeric_type_predicate
    assert @checker.numeric_type?(MLC::SemanticIR::Builder.primitive_type("i32"))
    assert @checker.numeric_type?(MLC::SemanticIR::Builder.primitive_type("f32"))
    assert @checker.numeric_type?(MLC::SemanticIR::Builder.primitive_type("i64"))
    refute @checker.numeric_type?(MLC::SemanticIR::Builder.primitive_type("bool"))
  end

  def test_error_type_predicate
    error = MLC::SemanticIR::ErrorType.new(error_message: "test")
    i32 = MLC::SemanticIR::Builder.primitive_type("i32")

    assert @checker.error_type?(error)
    refute @checker.error_type?(i32)
  end

  def test_generic_type_name_predicate
    assert @checker.generic_type_name?("T")
    assert @checker.generic_type_name?("U")
    refute @checker.generic_type_name?("i32")
    refute @checker.generic_type_name?("bool")
  end

  # ========== IO return types ==========

  def test_io_return_type_print
    result = @checker.io_return_type("print")

    assert_equal "i32", result.name
  end

  def test_io_return_type_read_line
    result = @checker.io_return_type("read_line")

    assert_equal "string", result.name
  end

  def test_io_return_type_args
    result = @checker.io_return_type("args")

    assert_instance_of MLC::SemanticIR::ArrayType, result
    assert_equal "string", result.element_type.name
  end

  # ========== Function validation ==========

  def test_validate_function_call_correct_args
    info = MockFunctionInfo.new(
      param_types: [MLC::SemanticIR::Builder.primitive_type("i32")],
      ret_type: MLC::SemanticIR::Builder.primitive_type("bool")
    )

    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    arg = MockExpr.new(i32)

    # Should not raise
    @checker.validate_function_call(info, [arg], "test_fn")
  end

  def test_validate_function_call_wrong_arg_count
    info = MockFunctionInfo.new(
      param_types: [
        MLC::SemanticIR::Builder.primitive_type("i32"),
        MLC::SemanticIR::Builder.primitive_type("i32")
      ],
      ret_type: MLC::SemanticIR::Builder.primitive_type("i32")
    )

    i32 = MLC::SemanticIR::Builder.primitive_type("i32")
    arg = MockExpr.new(i32)

    assert_raises(MLC::CompileError) do
      @checker.validate_function_call(info, [arg], "add")
    end
  end

  def test_ensure_argument_count_correct
    # Should not raise
    @checker.ensure_argument_count("method", [1, 2], 2)
  end

  def test_ensure_argument_count_wrong
    assert_raises(MLC::CompileError) do
      @checker.ensure_argument_count("method", [1], 2)
    end
  end

  # ========== Type constraints ==========

  def test_builtin_constraints_defined
    assert_includes MLC::Representations::Semantic::Gen::Services::TypeChecker::BUILTIN_CONSTRAINTS.keys, "Numeric"
    assert_includes MLC::Representations::Semantic::Gen::Services::TypeChecker::BUILTIN_CONSTRAINTS.keys, "Comparable"
    assert_includes MLC::Representations::Semantic::Gen::Services::TypeChecker::BUILTIN_CONSTRAINTS.keys, "Eq"
  end

  def test_validate_constraint_name_valid
    # Should not raise
    @checker.validate_constraint_name("Numeric")
    @checker.validate_constraint_name("Comparable")
    @checker.validate_constraint_name(nil)
    @checker.validate_constraint_name("")
  end

  def test_validate_constraint_name_invalid
    assert_raises(MLC::CompileError) do
      @checker.validate_constraint_name("InvalidConstraint")
    end
  end

  def test_type_satisfies_constraint
    assert @checker.type_satisfies_constraint?("Numeric", "i32")
    assert @checker.type_satisfies_constraint?("Numeric", "f32")
    refute @checker.type_satisfies_constraint?("Numeric", "string")
    refute @checker.type_satisfies_constraint?("Numeric", "bool")
  end

  # ========== Error recovery mode ==========

  def test_error_recovery_mode_disabled_by_default
    refute @checker.error_recovery_mode?
  end

  def test_error_recovery_mode_enabled_with_collector
    collector = MockErrorCollector.new

    checker = MLC::Representations::Semantic::Gen::Services::TypeChecker.new(
      function_registry: @function_registry,
      error_collector: collector
    )

    assert checker.error_recovery_mode?
  end

  def test_type_error_collects_when_in_recovery_mode
    collector = MockErrorCollector.new

    checker = MLC::Representations::Semantic::Gen::Services::TypeChecker.new(
      function_registry: @function_registry,
      error_collector: collector
    )

    result = checker.type_error("test error")

    assert_nil result
    assert_equal 1, collector.errors.size
    assert_equal "test error", collector.errors.first[:message]
  end

  def test_type_error_with_recovery_returns_error_type
    collector = MockErrorCollector.new

    checker = MLC::Representations::Semantic::Gen::Services::TypeChecker.new(
      function_registry: @function_registry,
      error_collector: collector
    )

    result = checker.type_error_with_recovery("test error")

    assert_instance_of MLC::SemanticIR::ErrorType, result
    assert_equal "test error", result.error_message
  end

  # ========== Type kind inference ==========

  def test_infer_type_kind_record
    record_type = MLC::SemanticIR::RecordType.new(
      name: "Point",
      fields: [{ name: "x", type: MLC::SemanticIR::Builder.primitive_type("i32") }]
    )
    ast_decl = MockTypeDecl.new("Point", MockRecordType.new)

    result = @checker.infer_type_kind(ast_decl, record_type)

    assert_equal :record, result
  end

  def test_infer_type_kind_sum
    sum_type = MLC::SemanticIR::SumType.new(
      name: "Option",
      variants: []
    )
    ast_decl = MockTypeDecl.new("Option", MockSumType.new)

    result = @checker.infer_type_kind(ast_decl, sum_type)

    assert_equal :sum, result
  end

  def test_infer_type_kind_primitive
    prim_type = MLC::SemanticIR::Builder.primitive_type("i32")
    ast_decl = MockTypeDecl.new("i32", MockPrimType.new("i32"))

    result = @checker.infer_type_kind(ast_decl, prim_type)

    assert_equal :primitive, result
  end

  private

  # ========== Mock Classes ==========

  class MockFunctionRegistry
    def initialize
      @functions = {}
    end

    def fetch(name)
      @functions[name]
    end

    def fetch_entry(name)
      nil
    end

    def register(name, info)
      @functions[name] = info
    end
  end

  class MockErrorCollector
    attr_reader :errors

    def initialize
      @errors = []
    end

    def add_error(message, origin: nil)
      @errors << { message: message, origin: origin }
    end
  end

  MockFunctionInfo = Struct.new(:param_types, :ret_type, keyword_init: true)

  class MockExpr
    attr_accessor :type

    def initialize(type)
      @type = type
    end
  end

  MockTypeDecl = Struct.new(:name, :type)
  MockRecordType = Struct.new(:x)
  MockSumType = Struct.new(:x)

  class MockPrimType
    attr_reader :name

    def initialize(name)
      @name = name
    end
  end
end

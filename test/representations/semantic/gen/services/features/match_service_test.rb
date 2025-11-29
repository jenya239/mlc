# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the service
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/features/match_service"
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/builders/ir_builder"

class MatchServiceTest < Minitest::Test
  def setup
    @ir_builder = ::MLC::Representations::Semantic::Gen::Services::IRBuilder.new
    @type_checker = MockTypeChecker.new
    @var_type_registry = MockVarTypeRegistry.new
    @ast_factory = MockAstFactory.new
    @ast_type_checker = MockAstTypeChecker.new
    @type_unification_service = MockTypeUnificationService.new
    @match_analyzer = MockMatchAnalyzer.new

    @service = ::MLC::Representations::Semantic::Gen::Services::MatchService.new(
      ir_builder: @ir_builder,
      type_checker: @type_checker,
      var_type_registry: @var_type_registry,
      ast_factory: @ast_factory,
      ast_type_checker: @ast_type_checker,
      type_unification_service: @type_unification_service,
      match_analyzer: @match_analyzer
    )
  end

  # ========== Pattern Transformation Tests ==========

  def test_transform_pattern_wildcard
    pattern = MockPattern.new(:wildcard, {})

    result = @service.send(:transform_pattern, pattern)

    assert_equal({ kind: :wildcard }, result)
  end

  def test_transform_pattern_literal
    pattern = MockPattern.new(:literal, { value: 42 })

    result = @service.send(:transform_pattern, pattern)

    assert_equal({ kind: :literal, value: 42 }, result)
  end

  def test_transform_pattern_var
    pattern = MockPattern.new(:var, { name: "x" })

    result = @service.send(:transform_pattern, pattern)

    assert_equal({ kind: :var, name: "x" }, result)
  end

  def test_transform_pattern_constructor_simple
    pattern = MockPattern.new(:constructor, { name: "Some", fields: ["x"] })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :constructor, result[:kind]
    assert_equal "Some", result[:name]
    assert_equal ["x"], result[:fields]
  end

  def test_transform_pattern_constructor_nested
    # Use real MLC::Source::AST::Pattern for recursive transformation
    inner_pattern = ::MLC::Source::AST::Pattern.new(kind: :var, data: { name: "value" })
    pattern = MockPattern.new(:constructor, { name: "Some", fields: [inner_pattern] })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :constructor, result[:kind]
    assert_equal "Some", result[:name]
    assert_equal 1, result[:fields].size
    assert_equal({ kind: :var, name: "value" }, result[:fields][0])
  end

  def test_transform_pattern_regex
    pattern = MockPattern.new(:regex, {
      pattern: "\\d+",
      flags: "i",
      bindings: ["match", "group1"]
    })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :regex, result[:kind]
    assert_equal "\\d+", result[:pattern]
    assert_equal "i", result[:flags]
    assert_equal ["match", "group1"], result[:bindings]
  end

  def test_transform_pattern_or
    alt1 = MockPattern.new(:literal, { value: 1 })
    alt2 = MockPattern.new(:literal, { value: 2 })
    pattern = MockPattern.new(:or, { alternatives: [alt1, alt2] })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :or, result[:kind]
    assert_equal 2, result[:alternatives].size
    assert_equal({ kind: :literal, value: 1 }, result[:alternatives][0])
    assert_equal({ kind: :literal, value: 2 }, result[:alternatives][1])
  end

  def test_transform_pattern_array
    # Use real MLC::Source::AST::Pattern for recursive transformation
    elem1 = ::MLC::Source::AST::Pattern.new(kind: :var, data: { name: "x" })
    elem2 = ::MLC::Source::AST::Pattern.new(kind: :var, data: { name: "y" })
    pattern = MockPattern.new(:array, { elements: [elem1, elem2] })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :array, result[:kind]
    assert_equal 2, result[:elements].size
    assert_equal({ kind: :var, name: "x" }, result[:elements][0])
    assert_equal({ kind: :var, name: "y" }, result[:elements][1])
  end

  def test_transform_pattern_tuple
    # Use real MLC::Source::AST::Pattern for recursive transformation
    elem1 = ::MLC::Source::AST::Pattern.new(kind: :var, data: { name: "a" })
    elem2 = ::MLC::Source::AST::Pattern.new(kind: :wildcard, data: {})
    pattern = MockPattern.new(:tuple, { elements: [elem1, elem2] })

    result = @service.send(:transform_pattern, pattern)

    assert_equal :tuple, result[:kind]
    assert_equal 2, result[:elements].size
    assert_equal({ kind: :var, name: "a" }, result[:elements][0])
    assert_equal({ kind: :wildcard }, result[:elements][1])
  end

  def test_transform_pattern_unknown_kind_raises
    pattern = MockPattern.new(:unknown, {})

    assert_raises(MLC::CompileError) do
      @service.send(:transform_pattern, pattern)
    end
  end

  # ========== Pattern Variable Binding Tests ==========

  def test_bind_pattern_variables_var_pattern
    pattern = { kind: :var, name: "x" }
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    @service.send(:bind_pattern_variables, pattern, type)

    assert_equal type, @var_type_registry.get("x")
  end

  def test_bind_pattern_variables_ignores_wildcard
    pattern = { kind: :wildcard }
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    @service.send(:bind_pattern_variables, pattern, type)

    assert_equal 0, @var_type_registry.size
  end

  def test_bind_pattern_variables_ignores_underscore
    pattern = { kind: :var, name: "_" }
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    @service.send(:bind_pattern_variables, pattern, type)

    assert_equal 0, @var_type_registry.size
  end

  def test_bind_pattern_variables_regex_bindings
    pattern = {
      kind: :regex,
      pattern: "\\d+",
      bindings: ["match", "_", "group1"]
    }
    type = MLC::SemanticIR::Builder.primitive_type("string")

    @service.send(:bind_pattern_variables, pattern, type)

    # "_" should be skipped, "match" and "group1" should be bound as strings
    assert @var_type_registry.has?("match")
    assert @var_type_registry.has?("group1")
    refute @var_type_registry.has?("_")
    assert_equal "string", @var_type_registry.get("match").name
  end

  def test_bind_pattern_variables_or_pattern_binds_first_alternative
    pattern = {
      kind: :or,
      alternatives: [
        { kind: :var, name: "x" },
        { kind: :var, name: "x" }
      ]
    }
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    @service.send(:bind_pattern_variables, pattern, type)

    assert @var_type_registry.has?("x")
    assert_equal type, @var_type_registry.get("x")
  end

  def test_bind_pattern_variables_array_pattern
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = MLC::SemanticIR::ArrayType.new(element_type: element_type)
    pattern = {
      kind: :array,
      elements: [
        { kind: :var, name: "first" },
        { kind: :var, name: "second" }
      ]
    }

    @service.send(:bind_pattern_variables, pattern, array_type)

    assert @var_type_registry.has?("first")
    assert @var_type_registry.has?("second")
    assert_equal element_type, @var_type_registry.get("first")
    assert_equal element_type, @var_type_registry.get("second")
  end

  def test_bind_pattern_variables_tuple_pattern
    type1 = MLC::SemanticIR::Builder.primitive_type("i32")
    type2 = MLC::SemanticIR::Builder.primitive_type("string")
    tuple_type = MLC::SemanticIR::TupleType.new(element_types: [type1, type2])
    pattern = {
      kind: :tuple,
      elements: [
        { kind: :var, name: "a" },
        { kind: :var, name: "b" }
      ]
    }

    @service.send(:bind_pattern_variables, pattern, tuple_type)

    assert @var_type_registry.has?("a")
    assert @var_type_registry.has?("b")
    assert_equal type1, @var_type_registry.get("a")
    assert_equal type2, @var_type_registry.get("b")
  end

  def test_bind_pattern_variables_constructor_simple
    field_type = MLC::SemanticIR::Builder.primitive_type("i32")
    @type_unification_service.set_constructor_info("Some", MockConstructorInfo.new([field_type]))

    scrutinee_type = MLC::SemanticIR::Builder.primitive_type("Option")
    pattern = {
      kind: :constructor,
      name: "Some",
      fields: ["value"]
    }

    @service.send(:bind_pattern_variables, pattern, scrutinee_type)

    assert @var_type_registry.has?("value")
    assert_equal field_type, @var_type_registry.get("value")
  end

  def test_bind_pattern_variables_constructor_nested
    inner_type = MLC::SemanticIR::Builder.primitive_type("i32")
    outer_type = MLC::SemanticIR::Builder.primitive_type("Option")

    @type_unification_service.set_constructor_info("Some", MockConstructorInfo.new([outer_type]))
    @type_unification_service.set_constructor_info("Inner", MockConstructorInfo.new([inner_type]))

    pattern = {
      kind: :constructor,
      name: "Some",
      fields: [
        { kind: :constructor, name: "Inner", fields: ["x"] }
      ]
    }

    # Note: This tests that nested patterns are recursively bound
    # The outer constructor gives us outer_type, then we bind the inner pattern
    @service.send(:bind_pattern_variables, pattern, outer_type)

    # The inner constructor "Inner" should be called with outer_type as scrutinee
    # then bind "x" to inner_type
    assert @var_type_registry.has?("x")
  end

  # ========== Helper Method Tests ==========

  def test_ignored_binding_nil
    assert @service.send(:ignored_binding?, nil)
  end

  def test_ignored_binding_underscore
    assert @service.send(:ignored_binding?, "_")
  end

  def test_ignored_binding_regular_name
    refute @service.send(:ignored_binding?, "x")
    refute @service.send(:ignored_binding?, "value")
  end

  def test_string_type_returns_string_primitive
    result = @service.send(:string_type)

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal "string", result.name
  end

  def test_unknown_type_returns_auto_primitive
    result = @service.send(:unknown_type)

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal "auto", result.name
  end

  def test_statement_safe_pattern_constructor
    pattern = MockPattern.new(:constructor, {})
    assert @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_wildcard
    pattern = MockPattern.new(:wildcard, {})
    assert @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_var
    pattern = MockPattern.new(:var, {})
    assert @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_tuple
    pattern = MockPattern.new(:tuple, {})
    assert @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_literal_not_safe
    pattern = MockPattern.new(:literal, {})
    refute @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_regex_not_safe
    pattern = MockPattern.new(:regex, {})
    refute @service.send(:statement_safe_pattern?, pattern)
  end

  def test_statement_safe_pattern_nil
    refute @service.send(:statement_safe_pattern?, nil)
  end

  private

  # ========== Mock Classes ==========

  class MockPattern
    attr_reader :kind, :data

    def initialize(kind, data)
      @kind = kind
      @data = data
    end
  end

  # Note: We use MockPattern instead of MLC::Source::AST::Pattern
  # to avoid namespace conflicts. The actual is_a? checks in match_service
  # test nested patterns which are covered by the MockPattern class.

  class MockTypeChecker
    def ensure_type!(type, message)
      raise ::MLC::CompileError, message unless type
    end
  end

  class MockVarTypeRegistry
    def initialize
      @vars = {}
      @snapshots = []
    end

    def set(name, type)
      @vars[name] = type
    end

    def get(name)
      @vars[name]
    end

    def has?(name)
      @vars.key?(name)
    end

    def size
      @vars.size
    end

    def snapshot
      @vars.dup
    end

    def restore(snapshot)
      @vars = snapshot
    end
  end

  class MockAstFactory
    def expr_stmt(expr:, origin:)
      { type: :expr_stmt, expr: expr, origin: origin }
    end
  end

  class MockAstTypeChecker
    def block_expr?(node)
      node.respond_to?(:type) && node.type == :block_expr
    end

    def block_statement?(node)
      node.respond_to?(:type) && node.type == :block_statement
    end

    def unit_literal?(node)
      node.respond_to?(:type) && node.type == :unit_literal
    end
  end

  class MockTypeUnificationService
    def initialize
      @constructor_infos = {}
    end

    def set_constructor_info(name, info)
      @constructor_infos[name] = info
    end

    def constructor_info_for(name, _scrutinee_type)
      @constructor_infos[name]
    end
  end

  MockConstructorInfo = Struct.new(:param_types)

  class MockMatchAnalyzer
    def analyze(scrutinee_type:, arms:, transform_arm:)
      transformed_arms = arms.map { |arm| transform_arm.call(scrutinee_type, arm) }
      result_type = transformed_arms.first&.dig(:body)&.type || MLC::SemanticIR::Builder.primitive_type("void")

      MockAnalysisResult.new(transformed_arms, result_type)
    end
  end

  MockAnalysisResult = Struct.new(:arms, :result_type)
end

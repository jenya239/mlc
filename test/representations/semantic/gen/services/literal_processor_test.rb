# frozen_string_literal: true

require_relative "../../../../test_helper"

# Explicitly require the service
require_relative "../../../../../lib/mlc/representations/semantic/gen/services/literal_processor"
require_relative "../../../../../lib/mlc/representations/semantic/gen/services/builders/ir_builder"

class LiteralProcessorTest < Minitest::Test
  def setup
    @ir_builder = MLC::Representations::Semantic::Gen::Services::IRBuilder.new
    @ast_type_checker = MockAstTypeChecker.new
    @processor = MLC::Representations::Semantic::Gen::Services::LiteralProcessor.new(
      ir_builder: @ir_builder
    )
  end

  # ========== supported? ==========

  def test_supported_int_literal
    node = MockIntLit.new(42)
    @ast_type_checker.set_literal_kind(node, :int)

    assert @processor.supported?(node, @ast_type_checker)
  end

  def test_supported_float_literal
    node = MockFloatLit.new(3.14)
    @ast_type_checker.set_literal_kind(node, :float)

    assert @processor.supported?(node, @ast_type_checker)
  end

  def test_supported_string_literal
    node = MockStringLit.new("hello")
    @ast_type_checker.set_literal_kind(node, :string)

    assert @processor.supported?(node, @ast_type_checker)
  end

  def test_supported_regex_literal
    node = MockRegexLit.new("\\d+", "i")
    @ast_type_checker.set_literal_kind(node, :regex)

    assert @processor.supported?(node, @ast_type_checker)
  end

  def test_supported_unit_literal
    node = MockUnitLit.new
    @ast_type_checker.set_literal_kind(node, :unit)

    assert @processor.supported?(node, @ast_type_checker)
  end

  def test_not_supported_unknown_node
    node = Object.new
    @ast_type_checker.set_literal_kind(node, nil)

    refute @processor.supported?(node, @ast_type_checker)
  end

  # ========== build - int ==========

  def test_build_int_literal
    node = MockIntLit.new(42)
    @ast_type_checker.set_literal_kind(node, :int)

    result = @processor.build(node, @ast_type_checker)

    assert_instance_of MLC::SemanticIR::LiteralExpr, result
    assert_equal 42, result.value
    assert_equal "i32", result.type.name
  end

  # ========== build - float ==========

  def test_build_float_literal
    node = MockFloatLit.new(3.14)
    @ast_type_checker.set_literal_kind(node, :float)

    result = @processor.build(node, @ast_type_checker)

    assert_instance_of MLC::SemanticIR::LiteralExpr, result
    assert_equal 3.14, result.value
    assert_equal "f32", result.type.name
  end

  # ========== build - string ==========

  def test_build_string_literal
    node = MockStringLit.new("hello world")
    @ast_type_checker.set_literal_kind(node, :string)

    result = @processor.build(node, @ast_type_checker)

    assert_instance_of MLC::SemanticIR::LiteralExpr, result
    assert_equal "hello world", result.value
    assert_equal "string", result.type.name
  end

  # ========== build - regex ==========

  def test_build_regex_literal
    node = MockRegexLit.new("\\d+", "gi")
    @ast_type_checker.set_literal_kind(node, :regex)

    result = @processor.build(node, @ast_type_checker)

    assert_instance_of MLC::SemanticIR::RegexExpr, result
    assert_equal "\\d+", result.pattern
    assert_equal "gi", result.flags
    assert_equal "regex", result.type.name
  end

  # ========== build - unit ==========

  def test_build_unit_literal
    node = MockUnitLit.new
    @ast_type_checker.set_literal_kind(node, :unit)

    result = @processor.build(node, @ast_type_checker)

    assert_instance_of MLC::SemanticIR::UnitLiteral, result
  end

  # ========== build - nil for unknown ==========

  def test_build_returns_nil_for_unknown_kind
    node = Object.new
    @ast_type_checker.set_literal_kind(node, nil)

    result = @processor.build(node, @ast_type_checker)

    assert_nil result
  end

  private

  # ========== Mock Classes ==========

  class MockAstTypeChecker
    def initialize
      @kinds = {}
    end

    def set_literal_kind(node, kind)
      @kinds[node.object_id] = kind
    end

    def literal_kind(node)
      @kinds[node.object_id]
    end
  end

  class MockIntLit
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end

  class MockFloatLit
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end

  class MockStringLit
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end

  class MockRegexLit
    attr_reader :pattern, :flags

    def initialize(pattern, flags)
      @pattern = pattern
      @flags = flags
    end
  end

  class MockUnitLit
    # No attributes needed
  end
end

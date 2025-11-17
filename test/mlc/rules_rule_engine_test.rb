# frozen_string_literal: true

require_relative "../test_helper"

class RulesRuleEngineTest < Minitest::Test
  class CaptureRule < MLC::Representations::Semantic::Gen::Rules::BaseRule
    def initialize(bucket)
      @bucket = bucket
    end

    private

    def matches?(node, _context = {})
      node.is_a?(MLC::SemanticIR::Func)
    end

    def produce(node, _context = {})
      @bucket << node.name
    end
  end

  def test_rule_engine_can_register_objects
    bucket = []
    engine = MLC::Representations::Semantic::Gen::Services::RuleEngine.new
    engine.register(:stage, CaptureRule.new(bucket))

    engine.apply(:stage, :node)

    assert_equal [], bucket
  end

  def test_rule_engine_is_invoked_during_transform
    bucket = []
    engine = MLC::Representations::Semantic::Gen::Services::RuleEngine.new
    engine.register(:core_ir_function, CaptureRule.new(bucket))

    source = <<~MLCORA
      fn id(x: i32) -> i32 = x
    MLCORA

    ast = MLC.parse(source)
    transformer = MLC::Representations::Semantic::Gen::Pipeline.new(rule_engine: engine)
    transformer.transform(ast)

    assert_includes bucket, "id"
  end

  def test_sum_type_constructors_are_registered
    source = <<~MLCORA
      type Option<T> = Some(T) | None
    MLCORA

    ast = MLC.parse(source)
    transformer = MLC::Representations::Semantic::Gen::Pipeline.new
    transformer.transform(ast)

    info = transformer.function_registry.fetch("Some")
    refute_nil info
    assert_equal "Option", transformer.type_registry.lookup("Option").name
  end

  def test_match_rule_builds_core_ir_match_expression
    source = <<~MLCORA
      type Option<T> = Some(T) | None

      fn unwrap(opt: Option<i32>) -> i32 =
        match opt
          | Some(x) => x
          | None => 0
    MLCORA

    ast = MLC.parse(source)
    transformer = MLC::Representations::Semantic::Gen::Pipeline.new
    core = transformer.transform(ast)
    func = core.items.find { |item| item.is_a?(MLC::SemanticIR::Func) && item.name == "unwrap" }

    assert_instance_of MLC::SemanticIR::MatchExpr, func.body
  end

  def test_function_effect_rule_sets_effects
    engine = MLC::Representations::Semantic::Gen::Services::RuleEngine.new
    engine.register(:core_ir_function, MLC::Representations::Semantic::Gen::Rules::FunctionEffectRule.new)

    source = <<~MLCORA
      fn identity(x: i32) -> i32 = x
    MLCORA

    ast = MLC.parse(source)
    transformer = MLC::Representations::Semantic::Gen::Pipeline.new(rule_engine: engine)
    core = transformer.transform(ast)
    func = core.items.find { |item| item.is_a?(MLC::SemanticIR::Func) && item.name == "identity" }

    assert_equal [:constexpr, :noexcept], func.effects
  end
end

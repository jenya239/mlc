# frozen_string_literal: true

require_relative '../function_registry'
require_relative '../type_registry'
require_relative '../rules/rule_engine'
require_relative 'services/container'
require_relative 'rules/literal_rule'
require_relative 'rules/var_ref_rule'
require_relative 'rules/let_rule'
require_relative 'rules/record_literal_rule'
require_relative 'rules/array_literal_rule'
require_relative 'rules/call_rule'
require_relative 'rules/unary_rule'
require_relative 'rules/binary_rule'
require_relative 'rules/module_member_rule'
require_relative 'visitors/expression_visitor'

module MLC
  module IRGenV2
    # Engine orchestrates IRGen v2 rule evaluation with shared services
    class Engine
      attr_reader :services, :rule_engine, :expression_visitor

      def initialize(function_registry: nil, type_registry: nil, services: nil, rule_engine: nil)
        @services = services || build_services(function_registry: function_registry, type_registry: type_registry)
        @rule_engine = rule_engine || build_rule_engine
        @expression_visitor = Visitors::ExpressionVisitor.new(engine: self)
      end

      def run(stage, node, extra_context = {})
        context = base_context.merge(extra_context)
        @rule_engine.apply(stage, node, context: context)
      end

      def run_expression(node, extra_context = {})
        @expression_visitor.visit(node, extra_context)
      end

      private

      def base_context
        { services: @services, engine: self, expression_visitor: @expression_visitor }
      end

      def build_services(function_registry:, type_registry:)
        registry = function_registry || MLC::FunctionRegistry.new
        types = type_registry || MLC::TypeRegistry.new
        Services::Container.new(function_registry: registry, type_registry: types)
      end

      def build_rule_engine
        engine = MLC::Rules::RuleEngine.new
        engine.register(:expression, MLC::IRGenV2::Rules::LiteralRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::VarRefRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::LetRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::RecordLiteralRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::ArrayLiteralRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::CallRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::UnaryRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::BinaryRule.new)
        engine.register(:expression, MLC::IRGenV2::Rules::ModuleMemberRule.new)
        engine
      end
    end
  end
end

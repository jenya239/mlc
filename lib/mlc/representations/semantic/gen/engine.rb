# frozen_string_literal: true

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::Representations::Semantic::Gen::Services::RuleEngine (line 57)
# - MLC::Representations::Semantic::Gen::Services::Container (line 53)
# - MLC::Representations::Semantic::Gen::Visitors::* (lines 21-22)
# - MLC::Representations::Semantic::Gen::Rules::* (lines 61-89)
# - MLC::Registries::* (lines 51-52)

module MLC
  module Representations
    module Semantic
      module Gen
        # Engine orchestrates SemanticGen rule evaluation with shared services
        class Engine
      attr_reader :services, :rule_engine, :expression_visitor, :statement_visitor

      def initialize(function_registry: nil, type_registry: nil, services: nil, rule_engine: nil)
        @services = services || build_services(function_registry: function_registry, type_registry: type_registry)
        @rule_engine = rule_engine || build_rule_engine
        ensure_required_rules(@rule_engine)
        @expression_visitor = MLC::Representations::Semantic::Gen::Visitors::ExpressionVisitor.new(engine: self)
        @statement_visitor = MLC::Representations::Semantic::Gen::Visitors::StatementVisitor.new(
          rule_engine: @rule_engine,
          services: @services,
          expression_visitor: @expression_visitor
        )
        @expression_visitor.statement_visitor = @statement_visitor
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
        {
          services: @services,
          engine: self,
          expression_visitor: @expression_visitor,
          statement_visitor: @statement_visitor
        }
      end

      def build_services(function_registry:, type_registry:)
        registry = function_registry || MLC::Registries::FunctionRegistry.new
        types = type_registry || MLC::Registries::TypeRegistry.new
        MLC::Representations::Semantic::Gen::Services::Container.new(function_registry: registry, type_registry: types)
      end

      def build_rule_engine
        MLC::Representations::Semantic::Gen::Services::RuleEngine.new
      end

      def ensure_required_rules(engine)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::LiteralRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::VarRefRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::LetRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::RecordLiteralRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::ArrayLiteralRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::IfRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::MatchRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::ForLoopRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::WhileLoopRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::LambdaRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::ListComprehensionRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::IndexAccessRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::CallRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::UnaryRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::BinaryRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::BlockRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::DoRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::ModuleMemberRule)
        register_rule(engine, :expression, MLC::Representations::Semantic::Gen::Rules::MemberRule)

        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::ExprStmtRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::VariableDeclRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::ReturnRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::AssignmentRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::BreakRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::ContinueRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::IfRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::ForRule)
        register_rule(engine, :statement, MLC::Representations::Semantic::Gen::Rules::Statements::WhileRule)
      end

      def register_rule(engine, stage, rule_class)
        stage = stage.to_sym
        rules = engine.registry[stage]
        already_registered = rules.any? do |rule|
          rule.is_a?(rule_class) || rule == rule_class || rule.class == rule_class
        end
        engine.register(stage, rule_class.new) unless already_registered
          end
        end
      end
    end
  end
end

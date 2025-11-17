# frozen_string_literal: true

require_relative '../../../registries/function_registry'
require_relative '../../../registries/type_registry'
require_relative 'services/rule_engine'
require_relative 'services/container'
require_relative 'rules/literal_rule'
require_relative 'rules/var_ref_rule'
require_relative 'rules/let_rule'
require_relative 'rules/record_literal_rule'
require_relative 'rules/array_literal_rule'
require_relative 'rules/if_rule'
require_relative 'rules/match_rule'
require_relative 'rules/for_loop_rule'
require_relative 'rules/while_loop_rule'
require_relative 'rules/lambda_rule'
require_relative 'rules/list_comprehension_rule'
require_relative 'rules/index_access_rule'
require_relative 'rules/call_rule'
require_relative 'rules/unary_rule'
require_relative 'rules/binary_rule'
require_relative 'rules/block_rule'
require_relative 'rules/do_rule'
require_relative 'rules/module_member_rule'
require_relative 'rules/member_rule'
require_relative 'rules/statements/expr_stmt_rule'
require_relative 'rules/statements/variable_decl_rule'
require_relative 'rules/statements/return_rule'
require_relative 'rules/statements/assignment_rule'
require_relative 'rules/statements/break_rule'
require_relative 'rules/statements/continue_rule'
require_relative 'rules/statements/if_rule'
require_relative 'rules/statements/for_rule'
require_relative 'rules/statements/while_rule'
require_relative 'visitors/expression_visitor'
require_relative 'visitors/statement_visitor'

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
        Services::Container.new(function_registry: registry, type_registry: types)
      end

      def build_rule_engine
        Services::RuleEngine.new
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

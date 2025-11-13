# frozen_string_literal: true

require_relative "../../test_helper"
require_relative "../../../lib/mlc/backends/cpp/orchestrator"
require_relative "../../../lib/mlc/backends/cpp/rules/expressions/unary_rule"
require_relative "../../../lib/mlc/backends/cpp/rules/expressions/literal_rule"
require_relative "../../../lib/mlc/common/registry/type_registry"
require_relative "../../../lib/mlc/common/registry/function_registry"

module MLC
  module Backends
    module Cpp
      class RuleEngineTest < Minitest::Test
        def test_rule_engine_integration_with_unary_rule
          # Setup: Create Orchestrator which initializes Container, Context, and RuleEngine
          type_registry = MLC::Core::TypeRegistry.new
          function_registry = MLC::Core::FunctionRegistry.new
          orchestrator = Orchestrator.new(
            type_registry: type_registry,
            function_registry: function_registry
          )

          # Register LiteralRule first (for operand)
          literal_rule = Rules::Expressions::LiteralRule.new(orchestrator.context)
          orchestrator.container.rule_engine.register_expression_rule(literal_rule)

          # Register UnaryRule
          unary_rule = Rules::Expressions::UnaryRule.new(orchestrator.context)
          orchestrator.container.rule_engine.register_expression_rule(unary_rule)

          # Create a simple SemanticIR::UnaryExpr for testing: !true
          bool_type = SemanticIR::Type.new(kind: :prim, name: "bool")
          operand = SemanticIR::LiteralExpr.new(value: true, type: bool_type)
          unary_expr = SemanticIR::UnaryExpr.new(
            op: "!",
            operand: operand,
            type: bool_type
          )

          # Lower the expression through the rule engine
          result = orchestrator.context.lower_expression(unary_expr)

          # Verify we get a CppAst::Nodes::UnaryExpression
          assert_instance_of CppAst::Nodes::UnaryExpression, result
          assert_equal "!", result.operator
          assert_equal true, result.prefix
        end

        def test_rule_engine_raises_when_no_rule_matches
          type_registry = MLC::Core::TypeRegistry.new
          function_registry = MLC::Core::FunctionRegistry.new
          orchestrator = Orchestrator.new(
            type_registry: type_registry,
            function_registry: function_registry
          )

          # Don't register any rules
          # Try to lower an expression
          bool_type = SemanticIR::Type.new(kind: :prim, name: "bool")
          operand = SemanticIR::LiteralExpr.new(value: true, type: bool_type)
          unary_expr = SemanticIR::UnaryExpr.new(
            op: "!",
            operand: operand,
            type: bool_type
          )

          # Should raise because no rule registered
          error = assert_raises(RuntimeError) do
            orchestrator.context.lower_expression(unary_expr)
          end

          assert_match(/No rule found for expression/, error.message)
        end
      end
    end
  end
end

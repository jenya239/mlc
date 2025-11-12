# frozen_string_literal: true

require_relative "../expression_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::UnaryExpr to CppAst::Nodes::UnaryExpression
          # Handles unary operators: !, -, +
          class UnaryRule < ExpressionRule
            def applies?(node)
              context.checker.unary_expr?(node)
            end

            def apply(node)
              # Recursively lower operand through context
              operand = lower_expression(node.operand)

              context.factory.unary_expression(
                operator: node.op,
                operand: operand,
                operator_suffix: node.op == "!" ? "" : "",
                prefix: true
              )
            end
          end
        end
      end
    end
  end
end

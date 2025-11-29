# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::UnaryExpr to CppAst::Nodes::UnaryExpression
          # Handles unary operators: !, -, +, ~ (bitwise NOT)
          class UnaryRule < ExpressionRule
            def applies?(node)
              context.checker.unary_expr?(node)
            end

            def apply(node)
              # Recursively lower operand through context
              operand = lower_expression(node.operand)

              # Prevent operator collision: -(-x) must generate -(-x), not --x
              # Same for +(+x) -> +(+x) and +(-x) -> +(-x), etc.
              if needs_parentheses?(node.op, operand)
                operand = context.factory.parenthesized(expression: operand)
              end

              context.factory.unary_expression(
                operator: node.op,
                operand: operand,
                operator_suffix: node.op == "!" ? "" : "",
                prefix: true
              )
            end

            private

            # Check if operand needs parentheses to prevent operator collision
            # e.g., -(-x) would become --x without parentheses
            def needs_parentheses?(op, operand)
              return false unless operand.is_a?(CppAst::Nodes::UnaryExpression)
              return false unless operand.prefix

              # These combinations would create C++ increment/decrement operators
              collision_pairs = [
                ["-", "-"], # --
                ["+", "+"], # ++
                ["-", "+"], # -+ (not critical but cleaner with parens)
                ["+", "-"]  # +- (not critical but cleaner with parens)
              ]

              collision_pairs.include?([op, operand.operator])
            end
          end
        end
      end
    end
  end
end

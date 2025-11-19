# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::IfExpr to C++ ternary operator
          # Handles: condition ? true_expr : false_expr
          # Note: Unit-type ifs should be lowered as statements, not expressions
          class IfRule < ExpressionRule
            def applies?(node)
              context.checker.if_expr?(node)
            end

            def apply(node)
              # Unit type if-expressions should be lowered as statements, not expressions
              if context.should_lower_as_statement?(node)
                raise "Internal error: Unit type if-expression should not be lowered as expression. " \
                      "Use lower_if_expr_as_statement instead."
              end

              # Recursively lower all parts
              condition = lower_expression(node.condition)
              then_branch = lower_expression(node.then_branch)

              # For value-producing expressions, use ternary operator
              # If no else branch, default to 0 (shouldn't happen in well-typed code)
              else_branch = if node.else_branch
                              lower_expression(node.else_branch)
                            else
                              context.factory.number_literal(value: "0")
                            end

              context.factory.ternary(
                condition: condition,
                true_expression: then_branch,
                false_expression: else_branch,
                question_prefix: " ",
                colon_prefix: " ",
                question_suffix: " ",
                colon_suffix: " "
              )
            end
          end
        end
      end
    end
  end
end

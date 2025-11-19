# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::BinaryExpr to C++ binary operators
          # Handles: +, -, *, /, %, ==, !=, <, >, <=, >=, &&, ||, etc.
          class BinaryRule < ExpressionRule
            def applies?(node)
              context.checker.binary_expr?(node)
            end

            def apply(node)
              # Recursively lower left and right child expressions
              left = lower_expression(node.left)
              right = lower_expression(node.right)

              context.factory.binary_expression(
                left: left,
                operator: node.op,
                right: right,
                operator_prefix: " ",
                operator_suffix: " "
              )
            end
          end
        end
      end
    end
  end
end

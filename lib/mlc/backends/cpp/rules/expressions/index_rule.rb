# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::IndexExpr to CppAst array subscript ([])
          # Handles array[index] syntax
          class IndexRule < ExpressionRule
            def applies?(node)
              context.checker.index_expr?(node)
            end

            def apply(node)
              # Recursively lower array and index expressions through context
              array = lower_expression(node.object)
              index = lower_expression(node.index)

              context.factory.array_subscript(
                array: array,
                index: index
              )
            end
          end
        end
      end
    end
  end
end

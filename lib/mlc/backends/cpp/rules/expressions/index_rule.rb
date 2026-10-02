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
              array = lower_expression(node.object)
              index = lower_expression(node.index)
              subscript = context.factory.array_subscript(array: array, index: index)
              return subscript unless cyclic_sum_array_element?(node.object)

              context.factory.unary_expression(operator: "*", operand: subscript)
            end

            def cyclic_sum_array_element?(object)
              element_type = object&.type
              return false unless element_type.is_a?(MLC::SemanticIR::ArrayType)

              context.named_cyclic_sum_type?(element_type.element_type)
            end
          end
        end
      end
    end
  end
end

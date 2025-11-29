# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::TupleExpr to C++ std::make_tuple
          # Handles: (1, 2, 3) -> std::make_tuple(1, 2, 3)
          class TupleRule < ExpressionRule
            def applies?(node)
              context.checker.tuple_expr?(node)
            end

            def apply(node)
              # Recursively lower each element
              elements = node.elements.map { |elem| lower_expression(elem) }

              # Generate: std::make_tuple(elem1, elem2, ...)
              context.factory.call(
                callee: context.factory.identifier("std::make_tuple"),
                arguments: elements,
                argument_separators: elements.size > 1 ? Array.new(elements.size - 1, ", ") : []
              )
            end
          end
        end
      end
    end
  end
end

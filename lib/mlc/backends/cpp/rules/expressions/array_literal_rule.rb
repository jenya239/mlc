# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::ArrayLiteralExpr to C++ std::vector initializer
          # Handles: [1, 2, 3] -> std::vector<int>{1, 2, 3}
          class ArrayLiteralRule < ExpressionRule
            def applies?(node)
              context.checker.array_literal_expr?(node)
            end

            def apply(node)
              # Get element type and map to C++
              element_type = context.map_type(node.type.element_type)

              # Recursively lower each element
              elements = node.elements.map { |elem| lower_expression(elem) }

              # Generate brace initializer: std::vector<int>{1, 2, 3}
              context.factory.brace_initializer(
                type: "std::vector<#{element_type}>",
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

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
              elements = node.elements.map { |elem| lower_expression(elem) }
              element_type = node.type&.element_type
              if context.named_cyclic_sum_type?(element_type)
                sum_name = element_type.name
                boxed_elements = elements.map do |element|
                  context.factory.raw_expression(
                    code: "std::make_shared<#{sum_name}>(#{element.to_source})"
                  )
                end
                return context.factory.brace_initializer(
                  type: "mlc::Array<std::shared_ptr<#{sum_name}>>",
                  arguments: boxed_elements,
                  argument_separators: boxed_elements.size > 1 ? Array.new(boxed_elements.size - 1, ", ") : []
                )
              end

              if elements.empty?
                # Empty array: emit {} and let C++ deduce the type from context.
                context.factory.raw_expression(code: "{}")
              else
                mapped_element_type = context.map_type(element_type)
                context.factory.brace_initializer(
                  type: "mlc::Array<#{mapped_element_type}>",
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
end

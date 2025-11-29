# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # TupleLiteralRule - builds SemanticIR tuple expressions
          class TupleLiteralRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.tuple_literal?(node)
            end

            def produce(node, context)
              svc = services(context)
              elements_ir = context[:elements_ir] || build_elements(node, context)

              # Build tuple type from element types
              element_types = elements_ir.map(&:type)
              tuple_type = svc.ir_builder.tuple_type(element_types: element_types)

              svc.ir_builder.tuple_expr(
                elements: elements_ir,
                type: tuple_type,
                origin: node.origin
              )
            end

            private

            def build_elements(node, context)
              visitor = context.fetch(:expression_visitor)
              node.elements.map { |element| visitor.visit(element) }
            end
          end
        end
      end
    end
  end
end

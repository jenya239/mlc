# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      # ArrayLiteralRule - builds SemanticIR array literals via services
      class ArrayLiteralRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.array_literal?(node)
        end

        def produce(node, context)
          svc = services(context)
          elements_ir = context[:elements_ir] || build_elements(node, context)

          svc.array_literal_builder.build(
            node: node,
            elements_ir: elements_ir
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

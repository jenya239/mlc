# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # RecordLiteralRule - builds SemanticIR record literals through services
          # Handles spread operator: { ...base, z: 3 } expands to { x: base.x, y: base.y, z: 3 }
          class RecordLiteralRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.record_literal?(node)
            end

            def produce(node, context)
              svc = services(context)
              fields_ir = context[:fields_ir] || build_simple_fields(node, context)
              expected_type = context[:expected_type] || svc.scope_context.current_function_return

              svc.record_literal_builder.build(
                node: node,
                fields_ir: fields_ir,
                expected_type: expected_type
              )
            end

            private

            # Simple field building (fallback, spreads handled in ExpressionVisitor)
            def build_simple_fields(node, context)
              visitor = context.fetch(:expression_visitor)
              node.fields.each_with_object({}) do |(name, expr), acc|
                acc[name.to_s] = visitor.visit(expr)
              end
            end
          end
        end
      end
    end
  end
end

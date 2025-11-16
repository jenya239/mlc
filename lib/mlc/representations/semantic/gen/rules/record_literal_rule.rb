# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # RecordLiteralRule - builds SemanticIR record literals through services
          class RecordLiteralRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.record_literal?(node)
        end

            def produce(node, context)
              svc = services(context)
              fields_ir = context[:fields_ir] || build_fields(node, context)

              svc.record_literal_builder.build(
                node: node,
                fields_ir: fields_ir
              )
        end

            private

            def build_fields(node, context)
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
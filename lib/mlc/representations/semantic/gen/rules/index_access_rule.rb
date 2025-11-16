# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # IndexAccessRule - handles array indexing expressions
          class IndexAccessRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.index_access?(node)
        end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)

              object_ir = visitor.visit(node.object)
              index_ir = visitor.visit(node.index)

              svc.index_access_service.build(
                object_ir: object_ir,
                index_ir: index_ir,
                ir_builder: svc.ir_builder,
                origin: node
              )
        end
          end
        end
          end
        end
      end
    end
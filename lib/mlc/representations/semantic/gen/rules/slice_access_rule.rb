# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # SliceAccessRule - handles array slicing expressions
          class SliceAccessRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.slice_access?(node)
            end

            def produce(node, context)
              svc = services(context)

              # Use pre-visited IR from context (provided by expression_visitor)
              object_ir = context.fetch(:object_ir)
              start_ir = context[:start_ir]
              end_ir = context[:end_ir]

              svc.slice_access_service.build(
                object_ir: object_ir,
                start_ir: start_ir,
                end_ir: end_ir,
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

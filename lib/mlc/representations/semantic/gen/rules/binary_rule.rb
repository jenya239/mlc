# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # BinaryRule - produces SemanticIR binary expressions
          class BinaryRule < BaseRule
            def matches?(node, context)
              return false if node.respond_to?(:op) && node.op == '|>'
              services(context).ast_type_checker.binary_op?(node)
            end

            def produce(node, context)
              svc = services(context)
              left_ir = context.fetch(:left_ir)
              right_ir = context.fetch(:right_ir)

              result_type = svc.type_inference_service.infer_binary_type(
                node.op,
                left_ir.type,
                right_ir.type
              )

              # Check for operator overloading via traits
              if svc.operator_trait_mapper.overloadable?(node.op, left_ir.type)
                resolution = svc.operator_trait_mapper.resolve(node.op, left_ir.type)
                if resolution
                  return svc.ir_builder.operator_call(
                    op: node.op,
                    left: left_ir,
                    right: right_ir,
                    method_name: resolution[:method_name],
                    trait_name: resolution[:trait_name],
                    type: result_type,
                    origin: node
                  )
                end
              end

              # Default: native binary operation
              svc.ir_builder.binary_op(
                op: node.op,
                left: left_ir,
                right: right_ir,
                type: result_type,
                origin: node
              )
            end
          end
        end
      end
    end
  end
end

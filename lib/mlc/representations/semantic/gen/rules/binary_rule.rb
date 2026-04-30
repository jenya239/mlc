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

              # Check for operator overloading via traits BEFORE type inference
              if svc.operator_trait_mapper.overloadable?(node.op, left_ir.type)
                resolution = svc.operator_trait_mapper.resolve(node.op, left_ir.type)
                if resolution
                  output_type = svc.operator_trait_mapper.output_type(node.op, left_ir.type, svc.type_builder) ||
                                left_ir.type
                  return svc.ir_builder.operator_call(
                    op: node.op,
                    left: left_ir,
                    right: right_ir,
                    method_name: resolution[:method_name],
                    trait_name: resolution[:trait_name],
                    type: output_type,
                    origin: node
                  )
                end
              end

              result_type = begin
                svc.type_inference_service.infer_binary_type(
                  node.op,
                  left_ir.type,
                  right_ir.type
                )
              rescue => e
                right_name = right_ir.respond_to?(:name) ? right_ir.name : right_ir.origin.inspect[0..80] rescue "?"
                left_val = left_ir.respond_to?(:value) ? left_ir.value : "?"
                raise "#{e.message} | left_val=#{left_val.inspect[0..50]} right_name=#{right_name}"
              end

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

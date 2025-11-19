# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # UnaryRule - produces SemanticIR unary expressions
          class UnaryRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.unary_op?(node)
        end

            def produce(node, context)
              svc = services(context)
              operand_ir = context.fetch(:operand_ir)

              result_type = svc.type_inference_service.infer_unary_type(
                node.op,
                operand_ir.type
              )

              svc.ir_builder.unary_op(
                op: node.op,
                operand: operand_ir,
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
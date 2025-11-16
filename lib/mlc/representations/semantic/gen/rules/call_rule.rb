# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # CallRule - Builds SemanticIR call expressions using services
          class CallRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.call?(node)
        end

            def produce(node, context)
              svc = services(context)
              callee_ir = context.fetch(:callee_ir)
              args_ir = context.fetch(:args_ir)

              result_type = svc.type_inference_service.infer_call_type(callee_ir, args_ir)

              svc.ir_builder.call(
                func: callee_ir,
                args: args_ir,
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
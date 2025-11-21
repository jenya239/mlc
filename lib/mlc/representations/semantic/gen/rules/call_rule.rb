# frozen_string_literal: true


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

              # Track move semantics - mark Owned<T> arguments as moved
              mark_moved_arguments(svc, node.args, args_ir)

              result_type = svc.type_inference_service.infer_call_type(callee_ir, args_ir)

              svc.ir_builder.call(
                func: callee_ir,
                args: args_ir,
                type: result_type,
                origin: node
              )
            end

            private

            def mark_moved_arguments(svc, ast_args, args_ir)
              ast_args.each_with_index do |ast_arg, index|
                next unless svc.ast_type_checker.var_ref?(ast_arg)

                arg_ir = args_ir[index]
                next unless arg_ir && Services::VarTypeRegistry.has_move_semantics?(arg_ir.type)

                svc.var_type_registry.mark_moved(ast_arg.name)
              end
            end
          end
        end
          end
        end
      end
    end
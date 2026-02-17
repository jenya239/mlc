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

              # Check for instance method call from extend blocks (obj.method(args) -> Type_method(obj, args))
              if callee_ir.is_a?(SemanticIR::MemberExpr)
                instance_result = try_instance_method_call(svc, node, callee_ir, args_ir)
                return instance_result if instance_result
              end

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

            def try_instance_method_call(svc, node, callee_ir, args_ir)
              trait_registry = svc.trait_registry
              return nil unless trait_registry

              object_type = callee_ir.object&.type
              return nil unless object_type

              type_name = svc.type_inference_service.send(:extract_base_type_name, object_type)
              return nil unless type_name

              method_name = callee_ir.member
              return nil unless trait_registry.resolve_instance_method(type_name, method_name)

              mangled_name = "#{type_name}_#{method_name}"
              func_info = svc.function_registry.fetch(mangled_name)
              return nil unless func_info

              # Prepend object (self) to args
              all_args = [callee_ir.object] + args_ir

              mark_moved_arguments(svc, node.args, args_ir)

              func_type = svc.type_inference_service.function_type_from_info(func_info)
              ret_type = svc.type_inference_service.infer_call_type(
                svc.ir_builder.var(name: mangled_name, type: func_type, origin: node),
                all_args
              )

              svc.ir_builder.call(
                func: svc.ir_builder.var(name: mangled_name, type: func_type, origin: node.callee),
                args: all_args,
                type: ret_type,
                origin: node
              )
            end

            def mark_moved_arguments(svc, ast_args, args_ir)
              ast_args.each_with_index do |ast_arg, index|
                next unless svc.ast_type_checker.var_ref?(ast_arg)

                arg_ir = args_ir[index]
                next unless arg_ir && Services::VarTypeRegistry.move_on_pass?(arg_ir.type)

                svc.var_type_registry.mark_moved(ast_arg.name)
              end
            end
          end
        end
      end
    end
  end
end

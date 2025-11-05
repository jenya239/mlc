# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # CallRule - Builds HighIR call expressions using services
      class CallRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.call?(node)
        end

        def produce(node, context)
          svc = services(context)
          callee_ir = context.fetch(:callee_ir)
          args_ir = context.fetch(:args_ir)

          result_type = infer_type(callee_ir, args_ir, context, svc)

          svc.ir_builder.call(
            func: callee_ir,
            args: args_ir,
            type: result_type,
            origin: node
          )
        end

        private

        def infer_type(callee_ir, args_ir, context, services)
          type_checker = services.type_checker

          classifier = services.high_ir_classifier

          if classifier.var_expr?(callee_ir)
            entry = services.function_registry.fetch_entry(callee_ir.name)
            if entry
              type_checker.validate_function_call(entry.info, args_ir, entry.name)
              return entry.info.ret_type
            end

            func_type = callee_ir.type
            return func_type.ret_type if func_type.respond_to?(:ret_type)

            return services.ir_builder.prim_type(name: 'auto')
          end

          if callee_ir.respond_to?(:type) && callee_ir.type.respond_to?(:ret_type)
            return callee_ir.type.ret_type
          end

          services.ir_builder.prim_type(name: 'auto')
        end
      end
    end
  end
end

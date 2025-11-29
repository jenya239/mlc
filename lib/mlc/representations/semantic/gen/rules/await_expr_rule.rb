# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # AwaitExprRule - handles await expressions
          # await expr - suspends coroutine until expr (a Future/Task) completes
          # Returns the inner value type from Future<T> or Task<T>
          class AwaitExprRule < BaseRule
            def matches?(node, context)
              svc = services(context)
              svc.ast_type_checker.await_expr?(node)
            end

            def produce(node, context)
              svc = services(context)

              # Use pre-visited operand_ir from context if available
              operand_ir = context[:operand_ir]
              unless operand_ir
                visitor = context.fetch(:expression_visitor)
                operand_ir = visitor.visit(node.operand)
              end

              # Extract the inner type from Future<T> or Task<T>
              inner_type = unwrap_future_type(operand_ir.type, svc)

              svc.ir_builder.await_expr(
                operand: operand_ir,
                type: inner_type,
                origin: node
              )
            end

            private

            # Extract T from Future<T>, Task<T>, or similar async wrapper types
            def unwrap_future_type(type, _services)
              return type unless type.is_a?(MLC::SemanticIR::GenericType)

              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil

              case base_name
              when "Future", "Task", "Awaitable", "Coroutine"
                # Future<T> -> T, Task<T> -> T
                return type.type_args.first if type.type_args.size >= 1
              end

              # If not a known async type, return as-is
              type
            end
          end
        end
      end
    end
  end
end

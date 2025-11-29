# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # TryExprRule - handles try expressions (error propagation)
          # expr? - unwraps Option<T> to T, returning early if None
          # Like Rust's ? operator
          class TryExprRule < BaseRule
            def matches?(node, context)
              svc = services(context)
              svc.ast_type_checker.try_expr?(node)
            end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)

              operand_ir = visitor.visit(node.operand)

              # Extract the inner type from Option<T> or Result<T, E>
              inner_type = unwrap_option_or_result_type(operand_ir.type, svc)

              svc.ir_builder.try_expr(
                operand: operand_ir,
                type: inner_type,
                origin: node
              )
            end

            private

            # Extract T from Option<T> or Ok type from Result<T, E>
            def unwrap_option_or_result_type(type, services)
              return type unless type.is_a?(MLC::SemanticIR::GenericType)

              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil

              case base_name
              when "Option"
                # Option<T> -> T
                return type.type_args.first if type.type_args.size == 1
              when "Result"
                # Result<T, E> -> T (the Ok type)
                return type.type_args.first if type.type_args.size >= 1
              end

              type
            end
          end
        end
      end
    end
  end
end

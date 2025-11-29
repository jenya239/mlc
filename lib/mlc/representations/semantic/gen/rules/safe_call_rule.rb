# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # SafeCallRule - handles safe call expressions (optional chaining)
          # obj?.method(args) - returns Option<T> where T is method's return type
          class SafeCallRule < BaseRule
            def matches?(node, context)
              svc = services(context)
              svc.ast_type_checker.safe_call?(node)
            end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)

              object_ir = visitor.visit(node.object)
              args_ir = node.args.map { |arg| visitor.visit(arg) }

              inner_type = unwrap_option_type(object_ir.type, svc)

              # Infer the method return type
              method_return_type = svc.type_inference_service.infer_method_call_type(
                inner_type,
                node.method_name,
                args_ir.map(&:type),
                node: node
              )

              # Result type is Option<method_return_type>
              result_type = wrap_in_option(method_return_type, svc)

              svc.ir_builder.safe_call(
                object: object_ir,
                method_name: node.method_name,
                args: args_ir,
                type: result_type,
                origin: node
              )
            end

            private

            # Extract T from Option<T>, or return the type as-is if not Option
            def unwrap_option_type(type, services)
              return type unless type.is_a?(MLC::SemanticIR::GenericType)
              return type unless type.base_type.respond_to?(:name) && type.base_type.name == "Option"
              return type unless type.type_args.size == 1

              type.type_args.first
            end

            # Wrap type in Option<T>
            def wrap_in_option(inner_type, services)
              option_base = MLC::SemanticIR::Builder.primitive_type("Option")
              MLC::SemanticIR::GenericType.new(base_type: option_base, type_args: [inner_type])
            end
          end
        end
      end
    end
  end
end

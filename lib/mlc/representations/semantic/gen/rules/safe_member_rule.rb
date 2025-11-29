# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # SafeMemberRule - handles safe member access expressions (optional chaining)
          # obj?.member - returns Option<T> where T is member's type
          class SafeMemberRule < BaseRule
            def matches?(node, context)
              svc = services(context)
              svc.ast_type_checker.safe_member_access?(node)
            end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)

              object_ir = visitor.visit(node.object)
              inner_type = unwrap_option_type(object_ir.type, svc)
              member_type = svc.type_inference_service.infer_member_type(inner_type, node.member, node: node)

              # Result type is Option<member_type>
              result_type = wrap_in_option(member_type, svc)

              svc.ir_builder.safe_member(
                object: object_ir,
                member: node.member,
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

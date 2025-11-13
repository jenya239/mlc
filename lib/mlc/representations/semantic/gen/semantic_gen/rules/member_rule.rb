# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      # MemberRule - handles non-module member access expressions
      class MemberRule < BaseRule
        def matches?(node, context)
          svc = services(context)
          return false unless svc.ast_type_checker.member_access?(node)

          !module_member?(node, svc)
        end

        def produce(node, context)
          svc = services(context)
          visitor = context.fetch(:expression_visitor)

          object_ir = visitor.visit(node.object)
          member_type = svc.type_inference_service.infer_member_type(object_ir.type, node.member, node: node)

          svc.ir_builder.member(
            object: object_ir,
            member: node.member,
            type: member_type,
            origin: node
          )
        end

        private

        def module_member?(node, services)
          services.module_resolver.resolve_member_function(
            node.object,
            node.member,
            services.function_registry,
            services.ast_type_checker
          )
        end
      end
    end
  end
end

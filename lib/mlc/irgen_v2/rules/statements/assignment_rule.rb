# frozen_string_literal: true

require_relative '../../rules/base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class AssignmentRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.assignment_statement?(node)
          end

          def produce(node, context)
            svc = services(context)
            value_ir = context[:value_ir] || context.fetch(:expression_visitor).visit(node.value)

            target = node.target
            unless target.is_a?(MLC::AST::VarRef)
              svc.type_checker.type_error('assignment target must be a variable', node: node)
            end

            target_name = target.name
            existing_type = svc.var_type_registry.get(target_name)
            unless existing_type
              svc.type_checker.type_error("assignment to undefined variable '#{target_name}'", node: node)
            end

            svc.type_checker.ensure_compatible_type(
              value_ir.type,
              existing_type,
              "assignment to '#{target_name}'",
              node: node
            )

            svc.var_type_registry.set(target_name, existing_type)

            target_ir = svc.ir_builder.var(name: target_name, type: existing_type, origin: node)
            svc.ir_builder.assignment_stmt(target: target_ir, value: value_ir, origin: node)
          end
        end
      end
    end
  end
end

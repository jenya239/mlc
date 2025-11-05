# frozen_string_literal: true

require_relative '../../rules/base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class VariableDeclRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.variable_decl_statement?(node)
          end

          def produce(node, context)
            svc = services(context)
            value_ir = context[:value_ir] || context.fetch(:expression_visitor).visit(node.value)

            type_checker = svc.type_checker
            type_checker.ensure_type!(value_ir.type, "initializer for '#{node.name}'", node: node)

            svc.var_type_registry.set(node.name, value_ir.type)

            svc.ir_builder.variable_decl_stmt(
              name: node.name,
              type: value_ir.type,
              value: value_ir,
              mutable: node.mutable,
              origin: node
            )
          end
        end
      end
    end
  end
end

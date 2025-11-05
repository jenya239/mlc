# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # IfRule - builds HighIR if expressions
      class IfRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.if_expr?(node)
        end

        def produce(node, context)
          svc = services(context)
          visitor = context.fetch(:expression_visitor)

          condition_ir = context[:condition_ir] || visitor.visit(node.condition)
          svc.type_checker.ensure_boolean_type(condition_ir.type, 'if condition', node: node.condition)

          then_ir = context[:then_ir] || visitor.visit(node.then_branch)
          else_ir = if context.key?(:else_ir)
                      context[:else_ir]
                    elsif node.else_branch
                      visitor.visit(node.else_branch)
                    end

          result_type = determine_type(
            type_checker: svc.type_checker,
            ir_builder: svc.ir_builder,
            then_ir: then_ir,
            else_ir: else_ir,
            node: node
          )

          svc.ir_builder.if_expr(
            cond: condition_ir,
            then_branch: then_ir,
            else_branch: else_ir,
            type: result_type,
            origin: node
          )
        end

        private

        def determine_type(type_checker:, ir_builder:, then_ir:, else_ir:, node:)
          if else_ir
            type_checker.ensure_compatible_type(
              else_ir.type,
              then_ir.type,
              'if expression branches',
              node: node
            )
            then_ir.type
          else
            ir_builder.prim_type(name: 'unit', origin: node)
          end
        end
      end
    end
  end
end

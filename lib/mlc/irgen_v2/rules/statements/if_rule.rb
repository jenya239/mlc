# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class IfRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.if_statement?(node)
          end

          def produce(node, context)
            svc = services(context)
            expr_visitor = context.fetch(:expression_visitor)
            stmt_visitor = context.fetch(:statement_visitor)

            condition_ir = context[:cond_ir] || expr_visitor.visit(node.condition)
            svc.type_checker.ensure_boolean_type(condition_ir.type, 'if condition', node: node.condition)

            then_block = build_block(node.then_branch, svc, stmt_visitor)
            else_block = node.else_branch ? build_block(node.else_branch, svc, stmt_visitor) : nil

            svc.ir_builder.if_stmt(
              condition: condition_ir,
              then_body: then_block,
              else_body: else_block,
              origin: node
            )
          end

          private

          def build_block(body_ast, services, stmt_visitor)
            statements = services.loop_service.normalize_loop_body(body_ast, stmt_visitor)
            services.ir_builder.block(statements: statements, origin: body_ast)
          end
        end
      end
    end
  end
end

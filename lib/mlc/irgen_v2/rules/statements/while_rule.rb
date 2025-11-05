# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class WhileRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.while_statement?(node)
          end

          def produce(node, context)
            svc = services(context)
            loops = svc.loop_service
            expr_visitor = context.fetch(:expression_visitor)
            stmt_visitor = context.fetch(:statement_visitor)

            condition_ir = context[:cond_ir] || expr_visitor.visit(node.condition)
            svc.type_checker.ensure_boolean_type(condition_ir.type, 'while condition', node: node.condition)

            body_ir = context[:body_statements] || loops.normalize_loop_body(node.body, stmt_visitor)

            result = nil
            loops.with_loop_scope do
              body_block = svc.ir_builder.block(statements: body_ir, origin: context[:body_origin] || node.body)
              result = svc.ir_builder.while_stmt(condition: condition_ir, body: body_block, origin: node)
            end

            result
          end
        end
      end
    end
  end
end

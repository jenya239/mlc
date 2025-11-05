# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      class WhileLoopRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.while_loop_expr?(node)
        end

        def produce(node, context)
          svc = services(context)
          loops = svc.loop_service
          expr_visitor = context.fetch(:expression_visitor)
          stmt_visitor = context.fetch(:statement_visitor)

          condition_ir = context[:condition_ir] || expr_visitor.visit(node.condition)
          svc.type_checker.ensure_boolean_type(condition_ir.type, 'while condition', node: node.condition)

          body_statements = loops.normalize_loop_body(node.body, stmt_visitor)

          while_stmt_ir = nil
          loops.with_loop_scope do
            body_block = svc.ir_builder.block(statements: body_statements, origin: node.body)
            while_stmt_ir = svc.ir_builder.while_stmt(condition: condition_ir, body: body_block, origin: node)
          end

          unit = svc.ir_builder.unit_literal(origin: node)
          svc.ir_builder.block_expr(
            statements: [while_stmt_ir],
            result: unit,
            type: unit.type,
            origin: node
          )
        end
      end
    end
  end
end

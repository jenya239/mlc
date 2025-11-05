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
          expr_visitor = context.fetch(:expression_visitor)
          stmt_visitor = context.fetch(:statement_visitor)

          condition_ir = context[:condition_ir] || expr_visitor.visit(node.condition)
          svc.type_checker.ensure_boolean_type(condition_ir.type, 'while condition', node: node.condition)

          body_statements = normalize_body(node.body, svc, stmt_visitor)

          while_stmt_ir = nil
          svc.scope_context.with_loop_scope do
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

        private

        def normalize_body(body, services, stmt_visitor)
          ast_factory = services.ast_factory
          statements =
            case body
            when MLC::AST::Block
              body.stmts
            when MLC::AST::BlockExpr
              stmts = body.statements.dup
              if body.result_expr
                stmts << ast_factory.expr_stmt(expr: body.result_expr, origin: body.result_expr.origin)
              end
              stmts
            else
              [ast_factory.expr_stmt(expr: body, origin: body.origin)]
            end

          stmt_visitor.visit_statements(statements)
        end
      end
    end
  end
end

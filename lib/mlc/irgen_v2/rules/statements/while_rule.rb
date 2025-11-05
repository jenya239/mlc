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
            expr_visitor = context.fetch(:expression_visitor)
            stmt_visitor = context.fetch(:statement_visitor)

            condition_ir = context[:cond_ir] || expr_visitor.visit(node.condition)
            svc.type_checker.ensure_boolean_type(condition_ir.type, 'while condition', node: node.condition)

            statements = context[:body_statements] || extract_statements(node.body)

            snapshot = svc.var_type_registry.snapshot
            body_block = nil

            svc.scope_context.with_loop_scope do
              body_ir = stmt_visitor.visit_statements(statements)
              body_block = svc.ir_builder.block(statements: body_ir, origin: context[:body_origin] || node.body)
            end

            svc.ir_builder.while_stmt(condition: condition_ir, body: body_block, origin: node)
          ensure
            svc.var_type_registry.restore(snapshot)
          end

          private

          def extract_statements(body)
            if body.is_a?(MLC::AST::Block)
              body.stmts
            else
              Array(body)
            end
          end
        end
      end
    end
  end
end

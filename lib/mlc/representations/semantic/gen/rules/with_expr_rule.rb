# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          class WithExprRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.with_expr?(node)
            end

            def produce(node, context)
              svc = services(context)
              expr_visitor = context.fetch(:expression_visitor)
              stmt_visitor = context.fetch(:statement_visitor)

              resource_ir = expr_visitor.visit(node.resource)

              saved = svc.var_type_registry.get(node.binder)
              svc.var_type_registry.set(node.binder, resource_ir.type)

              body_stmts = svc.loop_service.normalize_loop_body(node.body, stmt_visitor)
              body_block = svc.ir_builder.block(statements: body_stmts, origin: nil)

              svc.var_type_registry.set(node.binder, saved)

              with_stmt_ir = svc.ir_builder.with_stmt(
                binder: node.binder,
                resource: resource_ir,
                body: body_block,
                origin: node
              )

              unit = svc.ir_builder.unit_literal(origin: node)
              svc.ir_builder.block_expr(
                statements: [with_stmt_ir],
                result: unit,
                type: unit.type,
                origin: node
              )
            end
          end
        end
      end
    end
  end
end

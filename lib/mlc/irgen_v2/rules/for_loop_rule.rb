# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      class ForLoopRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.for_loop?(node)
        end

        def produce(node, context)
          svc = services(context)
          expr_visitor = context.fetch(:expression_visitor)
          stmt_visitor = context.fetch(:statement_visitor)

          iterable_ir = context[:iterable_ir] || expr_visitor.visit(node.iterable)
          element_type = infer_element_type(iterable_ir, svc, node)

          snapshot = svc.var_type_registry.snapshot
          svc.var_type_registry.set(node.var_name, element_type)

          for_stmt_ir = nil
          svc.scope_context.with_loop_scope do
            body_statements = normalize_body(node.body, svc, stmt_visitor)
            body_block = svc.ir_builder.block(statements: body_statements, origin: node.body)
            for_stmt_ir = svc.ir_builder.for_stmt(
              var_name: node.var_name,
              var_type: element_type,
              iterable: iterable_ir,
              body: body_block,
              origin: node
            )
          end

          unit = svc.ir_builder.unit_literal(origin: node)
          svc.ir_builder.block_expr(
            statements: [for_stmt_ir],
            result: unit,
            type: unit.type,
            origin: node
          )
        ensure
          svc.var_type_registry.restore(snapshot)
        end

        private

        def infer_element_type(iterable_ir, services, node)
          type = iterable_ir.type
          if type.respond_to?(:element_type) && type.element_type
            type.element_type
          else
            services.type_checker.type_error('cannot iterate over value without element type', node: node.iterable)
          end
        end

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

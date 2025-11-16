# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          class ForLoopRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.for_loop?(node)
        end

            def produce(node, context)
              svc = services(context)
              loops = svc.loop_service
              expr_visitor = context.fetch(:expression_visitor)
              stmt_visitor = context.fetch(:statement_visitor)

              iterable_ir = context[:iterable_ir] || expr_visitor.visit(node.iterable)
              element_type = loops.infer_iterable_element(iterable_ir, node: node.iterable)

              loops.with_loop_variable(node.var_name, element_type) do
                loops.with_loop_scope do
                  body_statements = loops.normalize_loop_body(node.body, stmt_visitor)
                  body_block = svc.ir_builder.block(statements: body_statements, origin: node.body)
                  for_stmt_ir = svc.ir_builder.for_stmt(
                    var_name: node.var_name,
                    var_type: element_type,
                    iterable: iterable_ir,
                    body: body_block,
                    origin: node
                  )

                  unit = svc.ir_builder.unit_literal(origin: node)
                  return svc.ir_builder.block_expr(
                    statements: [for_stmt_ir],
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
      end
    end
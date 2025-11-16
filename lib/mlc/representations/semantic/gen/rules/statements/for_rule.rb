# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            class ForRule < BaseRule
              def matches?(node, context)
                services(context).ast_type_checker.for_statement?(node)
          end

              def produce(node, context)
                svc = services(context)
                expr_visitor = context.fetch(:expression_visitor)
                stmt_visitor = context.fetch(:statement_visitor)
                loops = svc.loop_service

                iterable_ir = context[:iterable_ir] || expr_visitor.visit(node.iterable)
                element_type = loops.infer_iterable_element(iterable_ir, node: node.iterable)

                loops.with_loop_variable(node.var_name, element_type) do
                  loops.with_loop_scope do
                    body_statements = loops.normalize_loop_body(node.body, stmt_visitor)
                    body_block = svc.ir_builder.block(statements: body_statements, origin: node.body)

                    svc.ir_builder.for_stmt(
                      var_name: node.var_name,
                      var_type: element_type,
                      iterable: iterable_ir,
                      body: body_block,
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
    end
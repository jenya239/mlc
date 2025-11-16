# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          class DoRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.do_expr?(node)
        end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)
              factory = svc.ast_factory

              expressions = node.body
              return svc.ir_builder.unit_literal(origin: node) if expressions.empty?

              statements = expressions[0..-2].map { |expr| factory.expr_stmt(expr: expr, origin: expr.origin) }
              result_expr = expressions.last

              block_node = factory.block_expr(
                statements: statements,
                result_expr: result_expr,
                origin: node.origin
              )

              visitor.visit(block_node)
        end
          end
        end
          end
        end
      end
    end
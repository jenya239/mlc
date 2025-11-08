# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      class BlockRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.block_expr?(node)
        end

        def produce(node, context)
          svc = services(context)
          statement_visitor = context[:statement_visitor]
          expression_visitor = context.fetch(:expression_visitor)

          snapshot = svc.var_type_registry.snapshot

          statements_ir = statement_visitor.visit_statements(node.statements)
          result_ir = node.result_expr ? expression_visitor.visit(node.result_expr) : svc.ir_builder.unit_literal(origin: node)
          block_type = result_ir&.type || svc.ir_builder.prim_type(name: 'unit', origin: node)

          svc.ir_builder.block_expr(
            statements: statements_ir,
            result: result_ir,
            type: block_type,
            origin: node
          )
        ensure
          svc.var_type_registry.restore(snapshot)
        end
      end
    end
  end
end

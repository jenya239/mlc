# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # UnaryRule - produces HighIR unary expressions
      class UnaryRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.unary_op?(node)
        end

        def produce(node, context)
          svc = services(context)
          operand_ir = context.fetch(:operand_ir)

          result_type = svc.expression_type_resolver.unary_type(
            node.op,
            operand_ir.type,
            node: node
          )

          svc.ir_builder.unary_op(
            op: node.op,
            operand: operand_ir,
            type: result_type,
            origin: node
          )
        end
      end
    end
  end
end

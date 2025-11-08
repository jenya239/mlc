# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      # BinaryRule - produces SemanticIR binary expressions
      class BinaryRule < BaseRule
        def matches?(node, context)
          return false if node.respond_to?(:op) && node.op == '|>'
          services(context).ast_type_checker.binary_op?(node)
        end

        def produce(node, context)
          svc = services(context)
          left_ir = context.fetch(:left_ir)
          right_ir = context.fetch(:right_ir)

          result_type = svc.expression_type_resolver.binary_type(
            node.op,
            left_ir.type,
            right_ir.type,
            node: node
          )

          svc.ir_builder.binary_op(
            op: node.op,
            left: left_ir,
            right: right_ir,
            type: result_type,
            origin: node
          )
        end
      end
    end
  end
end

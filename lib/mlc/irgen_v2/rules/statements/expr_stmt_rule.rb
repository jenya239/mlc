# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class ExprStmtRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.expr_stmt?(node)
          end

          def produce(node, context)
            svc = services(context)
            expr_ir = context[:expr_ir] || context.fetch(:expression_visitor).visit(node.expr)
            svc.ir_builder.expr_statement(expression: expr_ir, origin: node)
          end
        end
      end
    end
  end
end

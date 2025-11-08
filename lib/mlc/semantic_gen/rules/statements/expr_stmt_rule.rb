# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module SemanticGen
    module Rules
      module Statements
        class ExprStmtRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.expr_stmt?(node)
          end

          def produce(node, context)
            svc = services(context)
            expr_ir = context[:expr_ir] || context.fetch(:expression_visitor).visit(node.expr)

            return nil if drop_noop_statement?(expr_ir)

            if expr_ir.is_a?(MLC::SemanticIR::BlockExpr)
              return expr_ir.statements
            end

            if expr_ir.is_a?(MLC::SemanticIR::IfExpr) && unit_type?(expr_ir.type, svc)
              return build_if_statement(expr_ir, svc)
            end

            svc.ir_builder.expr_statement(expression: expr_ir, origin: node)
          end

          private

          def drop_noop_statement?(expr_ir)
            expr_ir.is_a?(MLC::SemanticIR::UnitLiteral)
          end

          def unit_type?(type, services)
            name = services.type_checker.normalized_type_name(
              services.type_checker.type_name(type)
            )
            return true if %w[unit void].include?(name)
            type.is_a?(MLC::SemanticIR::UnitType)
          end

          def build_if_statement(expr_ir, services)
            then_block = block_from_expression(expr_ir.then_branch, services)
            else_block = expr_ir.else_branch ? block_from_expression(expr_ir.else_branch, services) : nil

            services.ir_builder.if_stmt(
              condition: expr_ir.condition,
              then_body: then_block,
              else_body: else_block,
              origin: expr_ir.origin
            )
          end

          def block_from_expression(expr_ir, services)
            case expr_ir
            when MLC::SemanticIR::BlockExpr
              statements = expr_ir.statements.dup
              if expr_ir.result && !expr_ir.result.is_a?(MLC::SemanticIR::UnitLiteral)
                statements << services.ir_builder.expr_statement(expression: expr_ir.result, origin: expr_ir.result.origin)
              end
              services.ir_builder.block(statements: statements, origin: expr_ir.origin)
            else
              stmt = services.ir_builder.expr_statement(expression: expr_ir, origin: expr_ir.origin)
              services.ir_builder.block(statements: [stmt], origin: expr_ir.origin)
            end
          end
        end
      end
    end
  end
end

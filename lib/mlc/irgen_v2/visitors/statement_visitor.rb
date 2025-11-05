# frozen_string_literal: true

module MLC
  module IRGenV2
    module Visitors
      # StatementVisitor - Traverses AST statements and invokes declarative rules
      class StatementVisitor
        def initialize(rule_engine:, services:, expression_visitor:)
          @rule_engine = rule_engine
          @services = services
          @expression_visitor = expression_visitor
        end

        attr_reader :expression_visitor

        def visit(node)
          svc = @services.ast_type_checker

          if svc.expr_stmt?(node)
            expr_ir = @expression_visitor.visit(node.expr)
            return apply_rule(node, expr_ir: expr_ir)
          end

          if svc.variable_decl_statement?(node)
            value_ir = @expression_visitor.visit(node.value)
            return apply_rule(node, value_ir: value_ir)
          end

          if svc.return_statement?(node)
            value_ir = node.expr ? @expression_visitor.visit(node.expr) : nil
            return apply_rule(node, value_ir: value_ir)
          end

          if svc.assignment_statement?(node)
            value_ir = @expression_visitor.visit(node.value)
            return apply_rule(node, value_ir: value_ir)
          end

          if svc.break_statement?(node) || svc.continue_statement?(node)
            return apply_rule(node)
          end

          if svc.while_statement?(node)
            cond_ir = @expression_visitor.visit(node.condition)
            body_statements = extract_block_statements(node.body)
            return apply_rule(
              node,
              cond_ir: cond_ir,
              body_statements: body_statements,
              body_origin: node.body
            )
          end

          raise MLC::CompileError, "StatementVisitor cannot handle #{node.class}"
        end

        def visit_statements(statements)
          statements.each_with_object([]) do |stmt, acc|
            result = visit(stmt)
            acc.concat(Array(result))
          end
        end

        private

        def apply_rule(node, extra_context = {})
          context = base_context.merge(extra_context)
          @rule_engine.apply(:statement, node, context: context)
        end

        def base_context
          {
            services: @services,
            expression_visitor: @expression_visitor,
            statement_visitor: self
          }
        end

        def extract_block_statements(body)
          if body.is_a?(MLC::AST::Block)
            body.stmts
          else
            Array(body)
          end
        end
      end
    end
  end
end

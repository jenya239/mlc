# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::IfStmt and unit-typed IfExpr to C++ if statements
          class IfRule < StatementRule
            def applies?(node)
              context.checker.if_stmt?(node) ||
                (context.checker.if_expr?(node) && context.should_lower_as_statement?(node))
            end

            def apply(node)
              condition = context.lower_expression(node.condition)

              # IfStmt uses then_body/else_body; IfExpr uses then_branch/else_branch
              if context.checker.if_stmt?(node)
                then_stmt = lower_statement_block(node.then_body)
                else_stmt = node.else_body ? lower_statement_block(node.else_body) : nil
              else
                then_stmt = lower_statement_block(node.then_branch)
                else_stmt = node.else_branch ? lower_statement_block(node.else_branch) : nil
              end

              context.factory.if_statement(
                condition: condition,
                then_statement: then_stmt,
                else_statement: else_stmt
              )
            end

            private

            def lower_statement_block(body_ir)
              if context.checker.block_expr?(body_ir)
                stmts = body_ir.statements.map { |stmt| context.lower_statement(stmt) }
                # Include result expr as statement if it's non-unit
                unless context.checker.unit_literal?(body_ir.result)
                  result_expr = context.lower_expression(body_ir.result)
                  stmts << context.factory.expression_statement(expression: result_expr)
                end
                context.factory.block_statement(
                  statements: stmts,
                  statement_trailings: Array.new(stmts.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              elsif context.checker.if_expr?(body_ir) && context.should_lower_as_statement?(body_ir)
                # Nested if with unit type - lower as if statement
                nested = context.lower_statement(body_ir)
                context.factory.block_statement(
                  statements: [nested],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                stmt = context.lower_statement(body_ir)
                context.factory.block_statement(
                  statements: [stmt],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              end
            end
          end
        end
      end
    end
  end
end

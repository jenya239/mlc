# frozen_string_literal: true

require_relative "../statement_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::IfStmt to C++ if statements
          class IfRule < StatementRule
            def applies?(node)
              context.checker.if_stmt?(node)
            end

            def apply(node)
              # Lower condition
              condition = context.lower_expression(node.condition)

              # Lower then and else bodies
              then_stmt = lower_statement_block(node.then_body)
              else_stmt = node.else_body ? lower_statement_block(node.else_body) : nil

              context.factory.if_statement(
                condition: condition,
                then_statement: then_stmt,
                else_statement: else_stmt
              )
            end

            private

            def lower_statement_block(body_ir)
              if context.checker.block_expr?(body_ir)
                # Lower block statements
                statements = body_ir.statements.map { |stmt| context.lower_statement(stmt) }

                context.factory.block_statement(
                  statements: statements,
                  statement_trailings: Array.new(statements.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                # Single statement - wrap in block
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

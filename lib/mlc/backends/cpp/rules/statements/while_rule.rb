# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::WhileStmt to C++ while loops
          class WhileRule < StatementRule
            def applies?(node)
              context.checker.while_stmt?(node)
            end

            def apply(node)
              # Lower condition
              condition = context.lower_expression(node.condition)

              # Lower body - handle BlockExpr or single statement
              body_statement = lower_body_as_block(node.body)

              CppAst::Nodes::WhileStatement.new(
                condition: condition,
                body: body_statement,
                while_suffix: " ",
                condition_lparen_suffix: "",
                condition_rparen_suffix: ""
              )
            end

            private

            def lower_body_as_block(body_ir)
              if context.checker.block_expr?(body_ir)
                # Lower block statements without return
                statements = body_ir.statements.map { |stmt| context.lower_statement(stmt) }

                context.factory.block_statement(
                  statements: statements,
                  statement_trailings: Array.new(statements.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                # Single statement body - wrap in block
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

# frozen_string_literal: true

require_relative "../statement_rule"
require_relative "../../for_loop_variable"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::ForStmt to C++ range-for loops
          class ForRule < StatementRule
            def applies?(node)
              context.checker.for_stmt?(node)
            end

            def apply(node)
              # Lower container expression
              container = context.lower_expression(node.iterable)

              # Map variable type to C++
              var_type_str = context.map_type(node.var_type)
              variable = ForLoopVariable.new(
                var_type_str,
                context.sanitize_identifier(node.var_name)
              )

              # Lower body
              body_block = lower_for_body(node.body)

              context.factory.range_for_statement(
                variable: variable,
                container: container,
                body: body_block
              )
            end

            private

            def lower_for_body(body_ir)
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
                # Single statement body
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

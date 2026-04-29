# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          class WithRule < StatementRule
            def applies?(node)
              context.checker.with_stmt?(node)
            end

            def apply(node)
              resource_code = context.lower_expression(node.resource).to_source
              binder = context.sanitize_identifier(node.binder)

              body_stmts = node.body.statements.map { |s| context.lower_statement(s) }

              inner_block = context.factory.block_statement(
                statements: body_stmts,
                statement_trailings: Array.new(body_stmts.length, "\n"),
                lbrace_suffix: "\n",
                rbrace_prefix: ""
              )

              drop_call = context.factory.expression_statement(
                expression: context.factory.raw_expression(code: "#{binder}.drop()")
              )

              outer_stmts = [
                context.factory.raw_statement(code: "auto #{binder} = (#{resource_code});"),
                inner_block,
                drop_call
              ]

              context.factory.block_statement(
                statements: outer_stmts,
                statement_trailings: ["\n", "\n", "\n"],
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

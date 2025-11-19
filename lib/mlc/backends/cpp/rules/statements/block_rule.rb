# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::Block to C++ block statements
          class BlockRule < StatementRule
            def applies?(node)
              context.checker.block?(node)
            end

            def apply(node)
              # Lower all statements, skipping unit literal expression statements
              lowered = []
              node.stmts.each do |stmt|
                # Skip ExprStatement containing only UnitLiteral
                if context.checker.expr_stmt?(stmt) &&
                   context.checker.unit_literal?(stmt.expression)
                  next
                end

                # Lower statement
                result = context.lower_statement(stmt)
                lowered.concat(Array(result))
              end

              context.factory.block_statement(
                statements: lowered,
                statement_trailings: Array.new(lowered.length, "\n"),
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

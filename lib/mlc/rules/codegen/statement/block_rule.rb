# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module CodeGen
      module Statement
        # Lowers SemanticIR block statements to C++ block statements
        class BlockRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::SemanticIR::Block)
          end

          def apply(node, context = {})
            lowerer = context[:lowerer]
            lowered = []
            node.stmts.each do |stmt|
              if stmt.is_a?(MLC::SemanticIR::ExprStatement) && stmt.expression.is_a?(MLC::SemanticIR::UnitLiteral)
                next
              end
              lowered.concat(Array(lowerer.send(:lower_coreir_statement, stmt)))
            end

            CppAst::Nodes::BlockStatement.new(
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

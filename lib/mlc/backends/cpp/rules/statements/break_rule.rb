# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::BreakStmt to C++ break statement
          class BreakRule < StatementRule
            def applies?(node)
              context.checker.break_stmt?(node)
            end

            def apply(node)
              context.factory.break_statement
            end
          end
        end
      end
    end
  end
end

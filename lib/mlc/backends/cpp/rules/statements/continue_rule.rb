# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::ContinueStmt to C++ continue statement
          class ContinueRule < StatementRule
            def applies?(node)
              context.checker.continue_stmt?(node)
            end

            def apply(node)
              context.factory.continue_statement
            end
          end
        end
      end
    end
  end
end

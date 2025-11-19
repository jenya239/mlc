# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        # Base class for statement rules
        # Generates C++ statements from SemanticIR::Statement nodes
        class StatementRule < BaseRule
          # Returns CppAst::Statement object
          def apply(stmt_node)
            raise NotImplementedError, "#{self.class}#apply must be implemented"
          end
        end
      end
    end
  end
end

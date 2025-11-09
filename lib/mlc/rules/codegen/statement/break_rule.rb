# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module CodeGen
      module Statement
        # Rule for lowering SemanticIR break statements to C++ break statements
        class BreakRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::SemanticIR::BreakStmt)
          end

          def apply(node, context = {})
            CppAst::Nodes::BreakStatement.new
          end
        end
      end
    end
  end
end

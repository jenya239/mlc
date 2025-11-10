# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # SemanticIRClassifier - centralizes queries about SemanticIR node kinds
      class SemanticIRClassifier
        def var_expr?(node)
          node.is_a?(MLC::SemanticIR::VarExpr)
        end

        def call_expr?(node)
          node.is_a?(MLC::SemanticIR::CallExpr)
        end

        def member_expr?(node)
          node.is_a?(MLC::SemanticIR::MemberExpr)
        end
      end
    end
  end
end

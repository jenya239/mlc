# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # HighIRClassifier - centralizes queries about HighIR node kinds
      class HighIRClassifier
        def var_expr?(node)
          node.is_a?(MLC::HighIR::VarExpr)
        end

        def call_expr?(node)
          node.is_a?(MLC::HighIR::CallExpr)
        end

        def member_expr?(node)
          node.is_a?(MLC::HighIR::MemberExpr)
        end
      end
    end
  end
end

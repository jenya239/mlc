# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::SymbolExpr to C++ mlc::Symbol
          # Handles: :foo -> mlc::Symbol("foo")
          class SymbolRule < ExpressionRule
            def applies?(node)
              context.checker.symbol_expr?(node)
            end

            def apply(node)
              context.factory.raw_expression(code: "mlc::Symbol(\"#{node.name.gsub('"', '\\"')}\")")
            end
          end
        end
      end
    end
  end
end

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
              # Generate: mlc::Symbol("name")
              # For now, symbols are represented as interned strings
              context.factory.call(
                callee: context.factory.identifier("mlc::Symbol"),
                arguments: [context.factory.string_literal(node.name)],
                argument_separators: []
              )
            end
          end
        end
      end
    end
  end
end

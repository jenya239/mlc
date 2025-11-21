# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::UnsafeBlockExpr to C++
          # Unsafe blocks are transparent in C++ - they just emit their body
          # The safety guarantee is enforced at compile time, not runtime
          class UnsafeBlockRule < ExpressionRule
            def applies?(node)
              context.checker.unsafe_block_expr?(node)
            end

            def apply(node)
              # Unsafe blocks are transparent - just lower the body
              # The safety enforcement happens at semantic analysis level
              lower_expression(node.body)
            end
          end
        end
      end
    end
  end
end

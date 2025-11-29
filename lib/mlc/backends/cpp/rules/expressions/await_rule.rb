# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::AwaitExpr to C++ co_await expression
          # await expr -> co_await expr (C++20 coroutines)
          class AwaitRule < ExpressionRule
            def applies?(node)
              context.checker.await_expr?(node)
            end

            def apply(node)
              # Recursively lower operand through context
              operand = lower_expression(node.operand)

              # Use CppAst's co_await DSL method
              context.factory.co_await(expression: operand)
            end
          end
        end
      end
    end
  end
end

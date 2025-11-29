# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::TupleAccessExpr to C++ std::get<N>(tuple)
          # Handles: tuple.0 -> std::get<0>(tuple)
          class TupleAccessRule < ExpressionRule
            def applies?(node)
              context.checker.tuple_access_expr?(node)
            end

            def apply(node)
              # Recursively lower the tuple expression
              tuple_cpp = lower_expression(node.tuple)
              index = node.index

              # Generate: std::get<N>(tuple)
              context.factory.call(
                callee: context.factory.identifier("std::get<#{index}>"),
                arguments: [tuple_cpp],
                argument_separators: []
              )
            end
          end
        end
      end
    end
  end
end

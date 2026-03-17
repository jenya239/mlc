# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::RangeExpr to std::views::iota
          class RangeRule < ExpressionRule
            def applies?(node)
              node.is_a?(MLC::SemanticIR::RangeExpr)
            end

            def apply(node)
              start_cpp = lower_expression(node.start_expr)
              end_cpp = lower_expression(node.end_expr)
              # inclusive: 0..=n -> iota(start, end+1), exclusive: 0..n -> iota(start, end)
              if node.inclusive
                end_src = end_cpp.to_source
                context.factory.raw_expression(code: "std::views::iota(#{start_cpp.to_source}, #{end_src} + 1)")
              else
                context.factory.raw_expression(code: "std::views::iota(#{start_cpp.to_source}, #{end_cpp.to_source})")
              end
            end
          end
        end
      end
    end
  end
end

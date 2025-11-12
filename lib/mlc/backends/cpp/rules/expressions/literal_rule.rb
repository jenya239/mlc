# frozen_string_literal: true

require_relative "../expression_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::LiteralExpr to C++ literal expressions
          # Handles: numeric literals (i32, f32), boolean literals (bool), string literals (str)
          class LiteralRule < ExpressionRule
            def applies?(node)
              context.checker.literal_expr?(node)
            end

            def apply(node)
              case node.type.name
              when "i32", "f32"
                context.factory.number_literal(value: node.value.to_s)
              when "bool"
                context.factory.boolean_literal(value: node.value)
              when "str", "string"
                build_aurora_string(node.value)
              else
                # Default: treat as numeric literal
                context.factory.number_literal(value: node.value.to_s)
              end
            end

            private

            # Build mlc::String(...) call for Aurora string literals
            def build_aurora_string(value)
              context.factory.function_call(
                callee: context.factory.identifier(name: "mlc::String"),
                arguments: [context.cpp_string_literal(value)],
                argument_separators: []
              )
            end
          end
        end
      end
    end
  end
end

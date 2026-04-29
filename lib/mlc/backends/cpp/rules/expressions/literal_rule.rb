# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::LiteralExpr to C++ literal expressions
          # Handles: numeric literals (i32, i64, u8, usize, f32, f64), boolean, string, char
          class LiteralRule < ExpressionRule
            def applies?(node)
              context.checker.literal_expr?(node)
            end

            def apply(node)
              case node.type.name
              when "i32", "i64", "u8", "u16", "u32", "u64", "i8", "i16", "usize", "f32", "f64"
                context.factory.number_literal(value: node.value.to_s)
              when "char"
                build_char_literal(node.value)
              when "bool"
                context.factory.boolean_literal(value: node.value)
              when "str", "string"
                build_mlc_string(node.value)
              else
                context.factory.number_literal(value: node.value.to_s)
              end
            end

            private

            def build_char_literal(codepoint)
              # Emit as char32_t literal: e.g. U'\x61' for 'a'
              context.factory.identifier(name: "static_cast<char32_t>(#{codepoint})")
            end

            # Build mlc::String(...) call for MLC string literals
            def build_mlc_string(value)
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

# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::RegexExpr to C++ mlc::Regex objects
          # Handles: /pattern/ and /pattern/i syntax
          class RegexRule < ExpressionRule
            def applies?(node)
              context.checker.regex_expr?(node)
            end

            def apply(node)
              # Generate: mlc::regex(String("pattern")) or mlc::regex_i(String("pattern"))
              pattern_string = build_aurora_string(node.pattern)

              # Choose function based on flags (case-insensitive if 'i' flag present)
              func_name = if node.flags.include?("i")
                            "mlc::regex_i"
                          else
                            "mlc::regex"
                          end

              context.factory.function_call(
                callee: context.factory.identifier(name: func_name),
                arguments: [pattern_string],
                argument_separators: []
              )
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

# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::SafeMemberExpr to C++ optional chaining
          # obj?.member -> obj.and_then([](const auto& v) { return v.member; })
          # Uses std::optional with and_then (C++23) or transform
          class SafeMemberRule < ExpressionRule
            def applies?(node)
              context.checker.safe_member_expr?(node)
            end

            def apply(node)
              object = lower_expression(node.object)
              member_name = sanitize_identifier(node.member)

              # Generate: obj.and_then([](const auto& v) { return v.member; })
              context.factory.function_call(
                callee: context.factory.member_access(
                  object: object,
                  operator: ".",
                  member: context.factory.identifier(name: "and_then")
                ),
                arguments: [
                  context.factory.lambda(
                    parameters: "const auto& v",
                    body: "return v.#{member_name};"
                  )
                ]
              )
            end
          end
        end
      end
    end
  end
end

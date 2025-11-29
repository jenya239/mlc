# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::SafeCallExpr to C++ optional chaining
          # obj?.method(args) -> obj.and_then([args...](const auto& v) { return v.method(args...); })
          # Uses std::optional with and_then (C++23) or transform
          class SafeCallRule < ExpressionRule
            def applies?(node)
              context.checker.safe_call_expr?(node)
            end

            def apply(node)
              object = lower_expression(node.object)
              method_name = sanitize_identifier(node.method_name)

              # Build the arguments string
              lowered_args = node.args.map { |arg| lower_expression(arg) }
              args_str = lowered_args.map { |arg| render_expression(arg) }.join(", ")

              # Generate: obj.and_then([](const auto& v) { return v.method(args); })
              context.factory.function_call(
                callee: context.factory.member_access(
                  object: object,
                  operator: ".",
                  member: context.factory.identifier(name: "and_then")
                ),
                arguments: [
                  context.factory.lambda(
                    parameters: "const auto& v",
                    body: "return v.#{method_name}(#{args_str});"
                  )
                ]
              )
            end

            private

            def render_expression(expr)
              CppAst::Renderer.render(expr)
            end
          end
        end
      end
    end
  end
end

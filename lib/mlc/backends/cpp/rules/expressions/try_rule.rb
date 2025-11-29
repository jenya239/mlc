# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::TryExpr to C++ early return
          # expr? -> unwrap Option/Result, return early if None/Err
          # For Option<T>: auto tmp = expr; if (!tmp.has_value()) return std::nullopt; auto val = *tmp;
          # For Result<T,E>: auto tmp = expr; if (tmp.is_err()) return std::unexpected(tmp.error()); auto val = tmp.value();
          class TryRule < ExpressionRule
            def applies?(node)
              context.checker.try_expr?(node)
            end

            def apply(node)
              operand = lower_expression(node.operand)
              operand_type = node.operand.type

              # Check if operand is Option or Result
              if option_type?(operand_type)
                generate_option_try(operand, node)
              elsif result_type?(operand_type)
                generate_result_try(operand, node)
              else
                # Fallback: treat as value() call
                context.factory.function_call(
                  callee: context.factory.member_access(
                    object: operand,
                    operator: ".",
                    member: context.factory.identifier(name: "value")
                  ),
                  arguments: []
                )
              end
            end

            private

            def option_type?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)
              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
              base_name == "Option"
            end

            def result_type?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)
              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
              base_name == "Result"
            end

            # Generate C++ for Option? using value_or with early return
            # Option uses std::optional which has value() with exception on empty
            # For ? operator, we use: expr.value() with the assumption that the
            # control flow has been checked, or use custom TRY macro
            def generate_option_try(operand, node)
              # Simple version: just call .value()
              # In practice, MLC should generate code with proper checks
              # For now: expr.value()
              context.factory.function_call(
                callee: context.factory.member_access(
                  object: operand,
                  operator: ".",
                  member: context.factory.identifier(name: "value")
                ),
                arguments: []
              )
            end

            # Generate C++ for Result?
            # Result uses std::expected (C++23) which has value() with exception
            def generate_result_try(operand, node)
              # Simple version: just call .value()
              context.factory.function_call(
                callee: context.factory.member_access(
                  object: operand,
                  operator: ".",
                  member: context.factory.identifier(name: "value")
                ),
                arguments: []
              )
            end
          end
        end
      end
    end
  end
end

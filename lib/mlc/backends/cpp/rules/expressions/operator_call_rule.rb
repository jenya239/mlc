# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::OperatorCallExpr to C++ function calls
          # Handles: Overloaded operators implemented via traits
          # Example: vec1 + vec2 -> Vec2_add(vec1, vec2)
          class OperatorCallRule < ExpressionRule
            def applies?(node)
              context.checker.operator_call_expr?(node)
            end

            def apply(node)
              # Lower left and right operands
              left = lower_expression(node.left)
              right = lower_expression(node.right)

              # Get the mangled function name: TypeName_method
              # The mangled name is stored in the node from the semantic analysis phase
              type_name = extract_type_name(node.left.type)
              func_name = "#{type_name}_#{node.method_name}"

              # Generate: TypeName_method(left, right)
              context.factory.function_call(
                callee: context.factory.identifier(name: func_name),
                arguments: [left, right],
                argument_separators: [", "]
              )
            end

            private

            # Extract type name from SemanticIR type
            def extract_type_name(type)
              case type
              when MLC::SemanticIR::Type
                type.name
              when MLC::SemanticIR::GenericType
                type.base_type.respond_to?(:name) ? type.base_type.name : "unknown"
              else
                type.respond_to?(:name) ? type.name : "unknown"
              end
            end
          end
        end
      end
    end
  end
end

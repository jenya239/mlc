# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::MemberExpr to C++ member access (.)
          # Handles: object.member syntax
          class MemberRule < ExpressionRule
            def applies?(node)
              context.checker.member_expr?(node)
            end

            def apply(node)
              # Recursively lower the object being accessed
              object = lower_expression(node.object)

              context.factory.member_access(
                object: object,
                operator: ".",
                member: context.factory.identifier(name: context.sanitize_identifier(node.member))
              )
            end
          end
        end
      end
    end
  end
end

# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::MemberExpr to C++ member access (. or ->)
          # Handles: object.member syntax
          # Uses -> for smart pointer types (Shared, Weak, Owned)
          class MemberRule < ExpressionRule
            # Smart pointer type names that require -> operator
            SMART_POINTER_TYPES = %w[Shared Weak Owned].freeze

            def applies?(node)
              context.checker.member_expr?(node)
            end

            def apply(node)
              # Recursively lower the object being accessed
              object = lower_expression(node.object)

              # Determine operator based on object type
              operator = determine_operator(node.object)

              context.factory.member_access(
                object: object,
                operator: operator,
                member: context.factory.identifier(name: context.sanitize_identifier(node.member))
              )
            end

            private

            def determine_operator(object_node)
              return "." unless object_node.respond_to?(:type) && object_node.type

              type = object_node.type

              # Check if it's a smart pointer type (GenericType with Shared/Weak/Owned base)
              if type.is_a?(MLC::SemanticIR::GenericType)
                base_name = extract_base_name(type.base_type)
                return "->" if SMART_POINTER_TYPES.include?(base_name)
              end

              "."
            end

            def extract_base_name(base_type)
              case base_type
              when MLC::SemanticIR::Type
                base_type.name
              else
                base_type.to_s
              end
            end
          end
        end
      end
    end
  end
end

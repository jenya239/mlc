# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::VarExpr to C++ identifiers
          # Handles variable references and boolean literals (true/false)
          class VarRefRule < ExpressionRule
            def applies?(node)
              context.checker.var_expr?(node)
            end

            def apply(node)
              case node.name
              when "true"
                context.factory.boolean_literal(value: true)
              when "false"
                context.factory.boolean_literal(value: false)
              else
                identifier = context.sanitize_identifier(node.name)

                # Check if this is a nullary variant constructor (RecordType with no fields)
                # In C++, we need to instantiate with {} syntax: Red{} instead of Red
                if nullary_variant_constructor?(node)
                  context.factory.raw_expression(code: "#{identifier}{}")
                else
                  context.factory.identifier(name: identifier)
                end
              end
            end

            private

            # Check if VarExpr refers to a nullary variant constructor
            # These need {} syntax in C++ to construct an instance
            def nullary_variant_constructor?(node)
              return false unless node.respond_to?(:type) && node.type

              type = node.type

              # SumType - check if the variant with this name has no fields
              if type.is_a?(MLC::SemanticIR::SumType)
                variant = type.variants.find { |v| v[:name] == node.name }
                return variant && (variant[:fields].nil? || variant[:fields].empty?)
              end

              # RecordType with no fields is also a nullary variant
              type.is_a?(MLC::SemanticIR::RecordType) && type.fields.empty?
            end
          end
        end
      end
    end
  end
end

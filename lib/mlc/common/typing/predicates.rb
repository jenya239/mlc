# frozen_string_literal: true

module MLC
  module Common
    module Typing
      # Pure functions for type predicates and name operations
      # No side effects, no dependencies, no state
      module Predicates
        NUMERIC_PRIMITIVES = %w[i8 i16 i32 i64 u8 u16 u32 u64 f32 f64].freeze
        INTEGER_PRIMITIVES = %w[i8 i16 i32 i64 u8 u16 u32 u64].freeze
        SIGNED_INTEGERS = %w[i8 i16 i32 i64].freeze
        UNSIGNED_INTEGERS = %w[u8 u16 u32 u64].freeze

        module_function

        # Extract the name from a type
        #
        # @param type [SemanticIR::Type, nil] The type to extract name from
        # @return [String, nil] The type name, or nil
        def type_name(type)
          type&.name
        end

        # Normalize a type name (e.g., "str" -> "string")
        #
        # @param name [String, nil] The type name to normalize
        # @return [String, nil] The normalized name
        def normalized_type_name(name)
          case name
          when "str"
            "string"
          else
            name
          end
        end

        # Get a normalized human-readable type description
        #
        # @param type [SemanticIR::Type, nil] The type to describe
        # @return [String] The type description, or "unknown"
        def describe_type(type)
          normalized_type_name(type_name(type)) || "unknown"
        end

        # Check if a type is numeric
        #
        # @param type [SemanticIR::Type] The type to check
        # @return [Boolean] Whether the type is numeric
        def numeric_type?(type)
          # TypeVariable is assumed to be numeric-compatible
          return true if type.is_a?(SemanticIR::TypeVariable)

          type_str = normalized_type_name(type_name(type))
          NUMERIC_PRIMITIVES.include?(type_str)
        end

        # Check if a type is integer (not floating point)
        #
        # @param type [SemanticIR::Type] The type to check
        # @return [Boolean] Whether the type is integer
        def integer_type?(type)
          # TypeVariable is assumed to be integer-compatible
          return true if type.is_a?(SemanticIR::TypeVariable)

          type_str = normalized_type_name(type_name(type))
          INTEGER_PRIMITIVES.include?(type_str)
        end

        # Check if a type is an ErrorType (poison type from error recovery)
        #
        # @param type [SemanticIR::Type] The type to check
        # @return [Boolean] Whether the type is an ErrorType
        def error_type?(type)
          type.is_a?(SemanticIR::ErrorType)
        end

        # Check if a name is a generic type parameter name (uppercase first letter)
        #
        # @param name [String, nil] The name to check
        # @return [Boolean] Whether the name is a generic type parameter
        def generic_type_name?(name)
          return false unless name.is_a?(String)
          name.empty? || name[0]&.match?(/[A-Z]/)
        end

        # Check if a type represents unit/void
        #
        # @param name [String] The normalized type name
        # @param type [SemanticIR::Type] The type to check
        # @return [Boolean] Whether the type is unit-like
        def unit_like?(name, type)
          return true if %w[unit void].include?(name)
          type.is_a?(SemanticIR::UnitType)
        end
      end
    end
  end
end

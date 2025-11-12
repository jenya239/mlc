# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Utils
          # Pure functions for type mapping from SemanticIR to C++
          # Migrated from Backend::CodeGenHelpers
          module TypeMapper
            module_function

            # Map SemanticIR type to C++ type string
            # Pure function - all dependencies passed as parameters
            def map_type(type, type_map:, type_registry: nil)
              case type
              when SemanticIR::TypeVariable
                # Type variables map directly to their name (T, U, E, etc.)
                type.name

              when SemanticIR::GenericType
                # Generic types: Base<Arg1, Arg2, ...>
                base_name = map_type(type.base_type, type_map: type_map, type_registry: type_registry)
                type_args = type.type_args.map { |arg|
                  map_type(arg, type_map: type_map, type_registry: type_registry)
                }.join(", ")
                "#{base_name}<#{type_args}>"

              when SemanticIR::ArrayType
                element_type = map_type(type.element_type, type_map: type_map, type_registry: type_registry)
                "std::vector<#{element_type}>"

              when SemanticIR::FunctionType
                # Function types: std::function<ReturnType(Arg1, Arg2, ...)>
                param_types = type.params.map { |p|
                  map_type(p[:type], type_map: type_map, type_registry: type_registry)
                }.join(", ")
                ret_type = map_type(type.ret_type, type_map: type_map, type_registry: type_registry)
                "std::function<#{ret_type}(#{param_types})>"

              when SemanticIR::OpaqueType
                # Check TypeRegistry first
                if type_registry && type_registry.has_type?(type.name)
                  return type_registry.cpp_name(type.name)
                end
                # Fallback: opaque types are pointers
                "#{type.name}*"

              when SemanticIR::RecordType, SemanticIR::SumType
                # Try TypeRegistry first
                if type_registry && type_registry.has_type?(type.name)
                  return type_registry.cpp_name(type.name)
                end
                # Fallback to type_map
                type_map[type.name] || type.name

              when SemanticIR::Type
                # Try TypeRegistry first
                if type_registry && type.respond_to?(:name) && type_registry.has_type?(type.name)
                  return type_registry.cpp_name(type.name)
                end

                # Check if it's a known primitive type
                mapped = type_map[type.name]
                return mapped if mapped

                # Uppercase name - likely type parameter (T, E, Result)
                return type.name if type.name =~ /^[A-Z][a-zA-Z0-9]*$/

                # Fallback
                type_map[type.name] || type.name

              else
                "auto"
              end
            end

            # Check if type requires 'auto' instead of explicit type
            def type_requires_auto?(type, type_map:, type_registry: nil, type_str: nil)
              return true if type.nil?

              type_str ||= map_type(type, type_map: type_map, type_registry: type_registry)
              return true if type_str.nil? || type_str.empty?
              return true if type_str.include?("auto")

              # If type contains unresolved type variables, use auto for C++20 type deduction
              return true if contains_type_variables?(type)

              case type
              when SemanticIR::ArrayType
                type_requires_auto?(type.element_type, type_map: type_map, type_registry: type_registry)
              when SemanticIR::FunctionType
                true
              when SemanticIR::RecordType
                type.name.nil? || type.name.empty? || type.name == "record"
              when SemanticIR::SumType
                type.name.nil? || type.name.empty?
              when SemanticIR::Type
                name = type.name
                return false if name && type_map.key?(name)
                name.nil? || name.empty? || name == "auto"
              else
                false
              end
            end

            # Check if type contains unresolved type variables (T, U, etc.)
            def contains_type_variables?(type)
              case type
              when SemanticIR::TypeVariable
                true
              when SemanticIR::GenericType
                # Check if any type argument is a type variable
                type.type_args.any? { |arg| contains_type_variables?(arg) }
              when SemanticIR::ArrayType
                contains_type_variables?(type.element_type)
              when SemanticIR::FunctionType
                type.params.any? { |p| contains_type_variables?(p[:type]) } ||
                  contains_type_variables?(type.ret_type)
              when SemanticIR::RecordType
                type.fields.any? do |field|
                  field_type = field[:type] || field.type
                  contains_type_variables?(field_type)
                end
              else
                false
              end
            end

            # Extract base type name without template parameters
            # Example: Pair<T, U> -> "Pair", Pair<int, int> -> "Pair"
            def extract_base_type_name(type, type_map: {}, type_registry: nil)
              case type
              when SemanticIR::GenericType
                # Recursively extract base name
                extract_base_type_name(type.base_type, type_map: type_map, type_registry: type_registry)
              when SemanticIR::RecordType, SemanticIR::SumType
                # Try TypeRegistry first
                if type_registry && type_registry.has_type?(type.name)
                  return type_registry.cpp_name(type.name)
                end
                # Return the type name
                type_map[type.name] || type.name
              when SemanticIR::Type
                # Try TypeRegistry first
                if type_registry && type.respond_to?(:name) && type_registry.has_type?(type.name)
                  return type_registry.cpp_name(type.name)
                end
                type_map[type.name] || type.name
              else
                # Fallback
                type.respond_to?(:name) ? type.name : "auto"
              end
            end
          end
        end
      end
    end
  end
end

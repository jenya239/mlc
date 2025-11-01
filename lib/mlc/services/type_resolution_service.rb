# frozen_string_literal: true

module MLC
  module Services
    # TypeResolutionService - Type reference refresh after type resolution
    # Phase 17-D: Extracted from IRGen FunctionTransformer
    #
    # Responsibilities:
    # - Refresh type references in function signatures after type resolution
    # - Recursively update nested type references (generics, arrays, functions)
    #
    # Dependencies (injected):
    # - function_registry: FunctionRegistry
    # - type_registry: TypeRegistry
    # - type_checker: TypeChecker (for type_name)
    class TypeResolutionService
      def initialize(function_registry:, type_registry:, type_checker:)
        @function_registry = function_registry
        @type_registry = type_registry
        @type_checker = type_checker
      end

      # Refresh all function signatures with resolved type
      # Updates param_types and ret_type in all registered functions
      #
      # @param resolved_name [String] The name of the resolved type
      def refresh_function_signatures!(resolved_name)
        type_info = @type_registry.lookup(resolved_name)
        return unless type_info

        resolved = type_info.core_ir_type

        @function_registry.each do |info|
          info.param_types = info.param_types.map do |type|
            refresh_type_reference(type, resolved_name, resolved)
          end
          info.ret_type = refresh_type_reference(info.ret_type, resolved_name, resolved)
        end
      end

      # Recursively refresh type references, preserving structure
      # Replaces type references that match resolved_name with resolved_type
      #
      # @param type [HighIR::Type] The type to refresh
      # @param resolved_name [String] The type name to match
      # @param resolved_type [HighIR::Type] The resolved type to use
      # @return [HighIR::Type] Updated type or original if no match
      def refresh_type_reference(type, resolved_name, resolved_type)
        case type
        when HighIR::GenericType
          # Don't replace the entire GenericType, just refresh its base_type
          base_type = refresh_type_reference(type.base_type, resolved_name, resolved_type)
          type_args = type.type_args.map { |arg| refresh_type_reference(arg, resolved_name, resolved_type) }
          if base_type != type.base_type || type_args != type.type_args
            HighIR::Builder.generic_type(base_type, type_args)
          else
            type
          end
        when HighIR::ArrayType
          # Refresh element type
          element_type = refresh_type_reference(type.element_type, resolved_name, resolved_type)
          element_type != type.element_type ? HighIR::Builder.array_type(element_type) : type
        when HighIR::FunctionType
          # Refresh params and return type
          params = type.params.map { |p| {name: p[:name], type: refresh_type_reference(p[:type], resolved_name, resolved_type)} }
          ret_type = refresh_type_reference(type.ret_type, resolved_name, resolved_type)
          (params != type.params || ret_type != type.ret_type) ? HighIR::Builder.function_type(params, ret_type) : type
        else
          # For primitive types and others, replace if name matches
          @type_checker.type_name(type) == resolved_name ? resolved_type : type
        end
      end
    end
  end
end

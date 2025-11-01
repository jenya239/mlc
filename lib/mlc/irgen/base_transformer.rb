# frozen_string_literal: true

module MLC
  class IRGen
      # BaseTransformer
      # Shared utilities for transformation
      # Auto-extracted from to_core.rb during refactoring
      module BaseTransformer
      def builtin_function_info(name)
        case name
        when "sqrt"
          f32 = HighIR::Builder.primitive_type("f32")
          FunctionInfo.new("sqrt", [f32], f32)
        else
          if IO_RETURN_TYPES.key?(name)
            FunctionInfo.new(name, [], @type_checker_service.io_return_type(name))
          else
            nil
          end
        end
      end

      def constructor_info_for(name, scrutinee_type)
        info = @sum_type_constructors[name]
        return unless info

        substitutions = generic_substitutions(info, scrutinee_type)
        return info if substitutions.empty?

        substituted_params = info.param_types.map do |type|
          apply_type_substitutions(type, substitutions)
        end
        substituted_ret_type = apply_type_substitutions(info.ret_type, substitutions)

        FunctionInfo.new(info.name, substituted_params, substituted_ret_type, info.type_params)
      end


      def ensure_function_signature(func_decl)
        register_function_signature(func_decl)
        @function_registry.fetch(func_decl.name)
      end


      def fresh_temp_name
        name = "__tmp#{@temp_counter}"
        @temp_counter += 1
        name
      end

      def function_placeholder_type(name)
        if (info = lookup_function_info(name))
          function_type_from_info(info)
        else
          HighIR::Builder.function_type([], HighIR::Builder.primitive_type("auto"))
        end
      end

      def function_type_from_info(info)
        params = info.param_types.each_with_index.map do |type, index|
          {name: "arg#{index}", type: type}
        end
        HighIR::Builder.function_type(params, info.ret_type)
      end

      def generic_type_name?(name)
        return false unless name

        return true if current_type_params.any? { |tp| tp.name == name }

        current_lambda_param_types.any? do |param_type|
          @type_checker_service.type_name(param_type) == name
        end
      end






      def generic_substitutions(info, scrutinee_type)
        return {} unless info.type_params&.any?
        return {} unless scrutinee_type.is_a?(HighIR::GenericType)

        base_match = @type_checker_service.type_name(info.ret_type) == @type_checker_service.type_name(scrutinee_type.base_type)
        return {} unless base_match

        substitutions = {}
        info.type_params.each_with_index do |param, idx|
          actual_arg = scrutinee_type.type_args[idx]
          substitutions[param.name] = actual_arg if actual_arg
        end
        substitutions
      end

      def apply_type_substitutions(type, substitutions)
        case type
        when HighIR::TypeVariable
          substitutions[type.name] || type
        when HighIR::GenericType
          base = apply_type_substitutions(type.base_type, substitutions)
          args = type.type_args.map { |arg| apply_type_substitutions(arg, substitutions) }
          HighIR::Builder.generic_type(base, args)
        when HighIR::ArrayType
          HighIR::Builder.array_type(apply_type_substitutions(type.element_type, substitutions))
        when HighIR::FunctionType
          params = type.params.map { |param| {name: param[:name], type: apply_type_substitutions(param[:type], substitutions)} }
          ret_type = apply_type_substitutions(type.ret_type, substitutions)
          HighIR::Builder.function_type(params, ret_type)
        when HighIR::RecordType
          fields = type.fields.map { |field| {name: field[:name], type: apply_type_substitutions(field[:type], substitutions)} }
          HighIR::Builder.record_type(type.name, fields)
        when HighIR::SumType
          variants = type.variants.map do |variant|
            fields = Array(variant[:fields]).map do |field|
              {name: field[:name], type: apply_type_substitutions(field[:type], substitutions)}
            end
            {name: variant[:name], fields: fields}
          end
          HighIR::Builder.sum_type(type.name, variants)
        else
          type
        end
      end

      def lookup_function_info(name)
        @function_registry.fetch(name) || @sum_type_constructors[name] || builtin_function_info(name)
      end





      def resolve_module_alias(identifier)
        return identifier unless identifier.is_a?(String) && !identifier.empty?
        (@current_import_aliases && @current_import_aliases[identifier]) || identifier
      end

      def module_member_entry(container_name, member_name)
        return unless container_name && member_name
        resolved = resolve_module_alias(container_name)
        @function_registry.fetch_entry_for_member(resolved, member_name)
      end

      def module_member_info(container_name, member_name)
        entry = module_member_entry(container_name, member_name)
        entry&.info
      end


      def with_current_node(node)
        previous = @current_node
        @current_node = node if node
        yield
      ensure
        @current_node = previous
      end
    end
  end
end

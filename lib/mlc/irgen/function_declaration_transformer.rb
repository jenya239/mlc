# frozen_string_literal: true

module MLC
  class IRGen
    # FunctionDeclarationTransformer
    # Function declaration registration and transformation
    # Phase 18-F: Extracted from FunctionTransformer
    #
    # Responsibilities:
    # - Register function signatures with type transformation
    # - Ensure function signatures exist in registry
    # - Transform function declarations (parameters, body, return type checking)
    # - Handle external functions
    # - Apply function transformation rules
    #
    # Dependencies (from IRGen):
    # - @function_registry: For function registration and lookup
    # - @function_registration_service: For signature registration
    # - @type_checker_service: For type checking, normalize_type_params, type_error
    # - @type_inference_service: For void_type? checks
    # - @var_type_registry: For parameter type tracking
    # - @scope_context_service: For type params context
    # - @rule_engine: For applying function rules
    # - @effect_analyzer: For effect analysis
    # - @type_registry: For type lookup
    # - with_type_params: From TransformationContext
    # - with_current_node: From TransformationContext
    # - with_function_return: From TransformationContext
    # - transform_type: From TypeTransformer
    # - transform_expression: From ExpressionTransformer
    module FunctionDeclarationTransformer
      # Ensure function signature exists in registry
      # Wrapper that transforms types and delegates registration to service
      def ensure_function_signature(func_decl)
        register_function_signature(func_decl)
        @function_registry.fetch(func_decl.name)
      end

      # Register function signature with type transformation
      # Transforms types using local context, then delegates registration to service
      def register_function_signature(func_decl)
        if @function_registry.registered?(func_decl.name)
          return @function_registry.fetch(func_decl.name)
        end

        # Set type parameters context before transforming types
        type_params = @type_checker_service.normalize_type_params(func_decl.type_params)

        info = nil
        with_type_params(type_params) do
          param_types = func_decl.params.map { |param| transform_type(param.type) }
          ret_type = transform_type(func_decl.ret_type)

          # Delegate registration to service
          info = @function_registration_service.register_function_signature(
            func_decl,
            param_types,
            ret_type,
            type_params
          )
        end

        info
      end

      # Transform function declaration to HighIR::Func
      # Handles type checking, parameter binding, body transformation, and external functions
      def transform_function(func)
        with_current_node(func) do
          # Normalize and set type params FIRST, before transforming any types
          type_params = @type_checker_service.normalize_type_params(func.type_params)
          with_type_params(type_params) do
            signature = ensure_function_signature(func)
            param_types = signature.param_types

            if param_types.length != func.params.length
              @type_checker_service.type_error("Function '#{func.name}' expects #{param_types.length} parameter(s), got #{func.params.length}")
            end

            params = func.params.each_with_index.map do |param, index|
              HighIR::Param.new(name: param.name, type: param_types[index])
            end

            ret_type = signature.ret_type

            # For external functions, skip body transformation
            if func.external
              result_func = HighIR::Func.new(
                name: func.name,
                params: params,
                ret_type: ret_type,
                body: nil,
                effects: [],
                type_params: type_params,
                external: true
              )
              if @function_registry.registered?(func.name)
                @function_registry.update(
                  func.name,
                  effects: result_func.effects,
                  external: true
                )
              end
              return result_func
            end

            saved_var_types = @var_type_registry.snapshot
            result_func = nil

            with_function_return(ret_type) do
              params.each do |param|
                @var_type_registry.set(param.name, param.type)
              end

              body = transform_expression(func.body)

              unless @type_inference_service.void_type?(ret_type)
                @type_checker_service.ensure_compatible_type(body.type, ret_type, "function '#{func.name}' result")
              else
                @type_checker_service.type_error("function '#{func.name}' should not return a value") unless @type_inference_service.void_type?(body.type)
              end

              result_func = HighIR::Func.new(
                name: func.name,
                params: params,
                ret_type: ret_type,
                body: body,
                effects: [],
                type_params: type_params,
                external: func.external
              )
            end

            rule_result = @rule_engine.apply(
              :core_ir_function,
              result_func,
              context: {
                type_context: @scope_context_service,
                type_registry: @type_registry,
                effect_analyzer: @effect_analyzer
              }
            )

            case rule_result
            when MLC::HighIR::Func
              result_func = rule_result
            when Array
              replacement = rule_result.find { |node| node.is_a?(MLC::HighIR::Func) && node.name == result_func.name }
              result_func = replacement if replacement
            when nil
              # keep original
            else
              result_func = rule_result if rule_result.is_a?(MLC::HighIR::Func)
            end

            if @function_registry.registered?(func.name)
              @function_registry.update(
                func.name,
                effects: result_func.effects,
                external: func.external
              )
            end

            @var_type_registry.restore(saved_var_types)
            result_func
          end
        end
      end
    end
  end
end

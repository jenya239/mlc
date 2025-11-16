# frozen_string_literal: true

require_relative "../../../../../registries/function_signature"

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # FunctionRegistrationService - Function signature registration
          # Phase 17-F: Extracted from the legacy FunctionTransformer
          #
          # Responsibilities:
          # - Register function signatures with metadata
          # - Check if function is already registered
          # - Fetch registered function signatures
          #
          # Dependencies (injected):
          # - function_registry: FunctionRegistry
          # - module_context_service: ModuleContextService
          class FunctionRegistrationService
            def initialize(function_registry:, module_context_service:)
              @function_registry = function_registry
              @module_context_service = module_context_service
            end

            # Ensure function signature exists in registry
            # Registers if not present, returns existing signature if already registered
            #
            # @param func_decl [AST::FuncDecl] Function declaration
            # @param param_types [Array<SemanticIR::Type>] Transformed parameter types
            # @param ret_type [SemanticIR::Type] Transformed return type
            # @param type_params [Array<SemanticIR::TypeParam>] Type parameters
            # @return [FunctionSignature] Function signature information
            def ensure_function_signature(func_decl, param_types, ret_type, type_params)
              if @function_registry.registered?(func_decl.name)
                return @function_registry.fetch(func_decl.name)
              end

              register_function_signature(
                func_decl,
                param_types,
                ret_type,
                type_params
              )
            end

            # Register function signature with metadata
            # Creates FunctionSignature and registers in function registry
            #
            # @param func_decl [AST::FuncDecl] Function declaration
            # @param param_types [Array<SemanticIR::Type>] Transformed parameter types
            # @param ret_type [SemanticIR::Type] Transformed return type
            # @param type_params [Array<SemanticIR::TypeParam>] Type parameters
            # @return [FunctionSignature] Registered function information
            def register_function_signature(func_decl, param_types, ret_type, type_params)
              if @function_registry.registered?(func_decl.name)
                return @function_registry.fetch(func_decl.name)
              end

              info = MLC::Registries::FunctionSignature.new(func_decl.name, param_types, ret_type, type_params)

              @function_registry.register(
                func_decl.name,
                info,
                module_name: @module_context_service.current_module_name,
                namespace: @module_context_service.current_module_namespace,
                exported: func_decl.exported,
                external: func_decl.external,
                ast_node: func_decl,
                origin: func_decl.origin
              )

              info
            end
          end
        end
      end
    end
  end
end

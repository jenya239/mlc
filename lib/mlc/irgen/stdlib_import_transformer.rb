# frozen_string_literal: true

module MLC
  class IRGen
    # StdlibImportTransformer
    # Standard library import registration and metadata handling
    # Phase 18-F: Extracted from FunctionTransformer
    #
    # Responsibilities:
    # - Register stdlib imports via rule engine
    # - Register stdlib function metadata with proper aliasing
    # - Register stdlib type metadata with proper namespace
    # - Wrapper for module import registration
    # - Wrapper for sum type constructor registration
    #
    # Dependencies (from IRGen):
    # - @stdlib_resolver: For stdlib_module? checks
    # - @stdlib_registry: For stdlib metadata lookup
    # - @function_registry: For function registration
    # - @type_registry: For type registration
    # - @type_decl_table: For type declaration tracking
    # - @type_checker_service: For normalize_type_params, infer_type_kind, type_error
    # - @module_context_service: For build_module_alias_keys
    # - @sum_type_constructor_service: For register_sum_type_constructors
    # - @rule_engine: For applying stdlib_import rules
    # - @event_bus: For publishing events
    # - with_type_params: From TransformationContext
    # - with_current_node: From TransformationContext
    # - transform_type: From TypeTransformer
    # - build_type_decl_for_import: From TypeTransformer
    module StdlibImportTransformer
      # Register stdlib imports via rule engine
      # Processes stdlib import declarations and registers functions/types
      def register_stdlib_imports(import_decl)
        return unless @stdlib_resolver.stdlib_module?(import_decl.path)

        @rule_engine.apply(
          :core_ir_stdlib_import,
          import_decl,
          context: {
            stdlib_registry: @stdlib_registry,
            module_alias: import_decl.alias,
            register_stdlib_function: method(:register_stdlib_function_metadata),
            register_stdlib_type: method(:register_stdlib_type_metadata),
            on_missing_item: lambda do |name, origin|
              import_origin = origin || import_decl.origin
              @event_bus.publish(
                :stdlib_missing_item,
                module: import_decl.path,
                item: name,
                origin: import_origin
              )
              @type_checker_service.type_error("Unknown item '#{name}' in stdlib import '#{import_decl.path}'", node: import_decl, origin: import_origin)
            end,
            event_bus: @event_bus
          }
        )
      end

      # Register stdlib function metadata with proper aliasing
      # Transforms function signature and registers in function_registry with module context
      def register_stdlib_function_metadata(metadata, module_info, module_alias = nil)
        decl = metadata&.ast_node
        return unless decl
        return if @function_registry.registered?(decl.name)

        type_params = @type_checker_service.normalize_type_params(decl.type_params)

        with_current_node(decl) do
          with_type_params(type_params) do
            param_types = decl.params.map { |param| transform_type(param.type) }
            ret_type = transform_type(decl.ret_type)
            alias_keys = @module_context_service.build_module_alias_keys(decl.name, module_info, module_alias)
            @function_registry.register(
              decl.name,
              FunctionInfo.new(decl.name, param_types, ret_type, type_params),
              module_name: module_info&.name,
              namespace: module_info&.namespace,
              exported: decl.exported,
              external: decl.external,
              ast_node: decl,
              origin: decl.origin,
              aliases: alias_keys
            )
          end
        end
      end

      # Register stdlib type metadata with proper namespace
      # Transforms type declaration and registers in type_registry
      def register_stdlib_type_metadata(decl, namespace, module_name = nil)
        return if @type_decl_table.key?(decl.name)

        type_decl_ir = build_type_decl_for_import(decl)

        kind = @type_checker_service.infer_type_kind(decl, type_decl_ir.type)
        @type_registry.register(
          decl.name,
          ast_node: decl,
          core_ir_type: type_decl_ir.type,
          namespace: namespace,
          kind: kind,
          exported: decl.exported,
          module_name: module_name
        )

        @type_decl_table[decl.name] = decl
        @sum_type_constructor_service.register_sum_type_constructors(decl.name, type_decl_ir.type) if type_decl_ir.type.is_a?(HighIR::SumType)
      end

      # Wrapper for service - delegates module import alias registration
      def register_module_import(import_decl, current_module)
        @module_context_service.register_module_import(import_decl, current_module)
      end

      # Wrapper for service - needed because method passed as callback to rule_engine
      def register_sum_type_constructors(sum_type_name, sum_type)
        @sum_type_constructor_service.register_sum_type_constructors(sum_type_name, sum_type)
      end
    end
  end
end

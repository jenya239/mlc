# frozen_string_literal: true

module MLC
  class IRGen
      # FunctionTransformer
      # Function and type declaration transformation
      # Auto-extracted from to_core.rb during refactoring
      # Phase 17-A: Module context methods migrated to ModuleContextService
      # Phase 17-B: Import alias methods migrated to ModuleContextService
      # Phase 17-C: infer_type_kind migrated to TypeChecker
      # Phase 17-D: Type resolution methods migrated to TypeResolutionService
      # Phase 17-E: with_import_aliases migrated to ModuleContextService
      module FunctionTransformer
      def ensure_function_signature(func_decl)
        register_function_signature(func_decl)
        @function_registry.fetch(func_decl.name)
      end

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
          info = FunctionInfo.new(func_decl.name, param_types, ret_type, type_params)
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
        end

        info
      end

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

      def register_module_import(import_decl, current_module)
        module_name = import_decl.path
        prefixes = @module_context_service.module_alias_prefixes(module_name, import_decl.alias)
        return if prefixes.empty?

        selected_items = import_decl.items

        @function_registry.names.each do |canonical_name|
          entry = @function_registry.fetch_entry(canonical_name)
          next unless entry&.module_name == module_name
          next if selected_items && !selected_items.include?(entry.name)

          alias_keys = prefixes.map { |prefix| "#{prefix}.#{entry.name}" }
          alias_keys << entry.name if selected_items

          alias_keys.each do |alias_key|
            next if alias_key == entry.name
            begin
              @function_registry.register_alias(alias_key, entry.name)
            rescue ArgumentError
              next
            end
          end
        end
      end

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
        register_sum_type_constructors(decl.name, type_decl_ir.type) if type_decl_ir.type.is_a?(HighIR::SumType)
      end

      def build_type_decl_for_import(decl)
        type_params = @type_checker_service.normalize_type_params(decl.type_params)

        type = nil

        with_current_node(decl) do
          with_type_params(type_params) do
            type = transform_type(decl.type)

            type = case type
                   when HighIR::RecordType
                     HighIR::Builder.record_type(decl.name, type.fields)
                   when HighIR::SumType
                     HighIR::Builder.sum_type(decl.name, type.variants)
                   else
                     type
                   end
          end
        end

        HighIR::TypeDecl.new(name: decl.name, type: type, type_params: type_params)
      end

      def register_sum_type_constructors(sum_type_name, sum_type)
        return unless sum_type.respond_to?(:variants)

        type_decl = @type_decl_table[sum_type_name]
        type_params = type_decl ? @type_checker_service.normalize_type_params(type_decl.type_params) : []

        type_param_vars = type_params.map do |tp|
          HighIR::Builder.type_variable(tp.name, constraint: tp.constraint)
        end
        generic_ret_type = if type_param_vars.any?
          HighIR::Builder.generic_type(sum_type, type_param_vars)
        else
          sum_type
        end

        sum_type.variants.each do |variant|
          field_types = (variant[:fields] || []).map { |field| field[:type] }
          @sum_type_constructors[variant[:name]] = FunctionInfo.new(variant[:name], field_types, generic_ret_type, type_params)
        end
      end

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

      def transform_program(program)
        module_name = program.module_decl ? program.module_decl.name : "main"
        context = {
          program: program,
          imports: [],
          type_items: [],
          func_items: [],
          module_name: module_name,
          import_aliases: {}
        }

        @module_context_service.with_current_module(module_name) do
          @module_context_service.with_import_aliases(context[:import_aliases]) do
            build_program_pass_manager.run(context)
          end
        end

        items = context[:type_items] + context[:func_items]

        HighIR::Module.new(
          name: context[:module_name],
          items: items,
          imports: context[:imports]
        )
      end

      def build_program_pass_manager
        MLC::PassManager.new.tap do |manager|
          manager.register(:collect_imports, method(:pass_collect_imports))
          manager.register(:preregister_types, method(:pass_preregister_types))
          manager.register(:preregister_functions, method(:pass_preregister_functions))
          manager.register(:lower_declarations, method(:pass_lower_declarations))
        end
      end

      def pass_collect_imports(context)
        program = context[:program]

        program.imports.each do |import_decl|
          context[:import_aliases][import_decl.alias] = import_decl.path if import_decl.alias
          context[:imports] << HighIR::Import.new(
            path: import_decl.path,
            items: import_decl.items
          )

          if @stdlib_resolver.stdlib_module?(import_decl.path)
            register_stdlib_imports(import_decl)
          else
            register_module_import(import_decl, context[:module_name])
          end
        end
      end

      def pass_preregister_types(context)
        program = context[:program]

        program.declarations.each do |decl|
          next unless decl.is_a?(AST::TypeDecl)
          @type_decl_table[decl.name] = decl
        end
      end

      def pass_preregister_functions(context)
        program = context[:program]

        program.declarations.each do |decl|
          register_function_signature(decl) if decl.is_a?(AST::FuncDecl)
        end
      end
      def pass_lower_declarations(context)
        program = context[:program]

        program.declarations.each do |decl|
          case decl
          when AST::TypeDecl
            type_decl = transform_type_decl(decl)
            context[:type_items] << type_decl
            @type_resolution_service.refresh_function_signatures!(decl.name)
          when AST::FuncDecl
            context[:func_items] << transform_function(decl)
          end
        end
      end

      def transform_type(type)
        with_current_node(type) do
          case type
          when AST::PrimType
            # Check if this is a reference to a type parameter
            if current_type_params.any? { |tp| tp.name == type.name }
              # This is a type variable (reference to type parameter)
              constraint_param = current_type_params.find { |tp| tp.name == type.name }
              HighIR::Builder.type_variable(type.name, constraint: constraint_param&.constraint)
            else
              HighIR::Builder.primitive_type(type.name)
            end
          when AST::OpaqueType
            HighIR::Builder.opaque_type(type.name)
          when AST::GenericType
            # Validate generic constraints before lowering
            base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
            @type_checker_service.validate_type_constraints(base_name, type.type_params) if base_name

            # Transform to HighIR::GenericType with proper type arguments
            base_type = transform_type(type.base_type)
            type_args = type.type_params.map { |tp| transform_type(tp) }
            HighIR::Builder.generic_type(base_type, type_args)
          when AST::FunctionType
            # Transform function type: fn(T, U) -> V
            param_types = type.param_types.map { |pt| transform_type(pt) }
            ret_type = transform_type(type.ret_type)

            # Convert to params format expected by HighIR::FunctionType
            params = param_types.map.with_index { |pt, i| {name: "arg#{i}", type: pt} }
            HighIR::Builder.function_type(params, ret_type)
          when AST::RecordType
            fields = type.fields.map { |field| {name: field[:name], type: transform_type(field[:type])} }
            HighIR::Builder.record_type(type.name, fields)
          when AST::SumType
            variants = type.variants.map do |variant|
              fields = variant[:fields].map { |field| {name: field[:name], type: transform_type(field[:type])} }
              {name: variant[:name], fields: fields}
            end
            HighIR::Builder.sum_type(type.name, variants)
          when AST::ArrayType
            element_type = transform_type(type.element_type)
            HighIR::Builder.array_type(element_type)
          else
            raise "Unknown type: #{type.class}"
          end
        end
      end

      def transform_type_decl(decl)
        with_current_node(decl) do
          # Normalize type params first
          type_params = @type_checker_service.normalize_type_params(decl.type_params)

          type = nil

          with_type_params(type_params) do
            # Transform the type definition
            type = transform_type(decl.type)

            type = case type
                   when HighIR::RecordType
                     HighIR::Builder.record_type(decl.name, type.fields)
                   when HighIR::SumType
                     HighIR::Builder.sum_type(decl.name, type.variants)
                   else
                     type
                   end

            # Register type in TypeRegistry
            kind = @type_checker_service.infer_type_kind(decl, type)
            @type_registry.register(
              decl.name,
              ast_node: decl,
              core_ir_type: type,
              namespace: nil,  # Main module types have no namespace
              kind: kind,
              exported: decl.exported,
              module_name: @module_context_service.current_module_name
            )
          end

          # Create TypeDecl
          type_decl = HighIR::TypeDecl.new(name: decl.name, type: type, type_params: type_params)

          @rule_engine.apply(
            :core_ir_type_decl,
            type_decl,
            context: {
              type: type,
              source_decl: decl,
              type_registry: @type_registry,
              register_sum_type_constructors: method(:register_sum_type_constructors)
            }
          )

          type_decl
        end
      end

      end
  end
end

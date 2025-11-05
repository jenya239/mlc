# frozen_string_literal: true

module MLC
  class IRGen
    # ProgramTransformer
    # Program-level transformation orchestration
    # Phase 18-F: Extracted from FunctionTransformer
    # Phase 23-D: Fixed user module import ordering
    #
    # Responsibilities:
    # - Orchestrate full program transformation via pass manager
    # - Collect imports and register import aliases
    # - Preregister types and functions
    # - Register user module import aliases (AFTER functions are registered)
    # - Lower declarations (types and functions)
    # - Build final HighIR::Module
    #
    # Dependencies (from IRGen):
    # - @module_context_service: For current module name and import alias context
    # - @type_decl_table: For type declaration tracking
    # - @type_resolution_service: For refresh_function_signatures!
    # - register_stdlib_imports: From StdlibImportTransformer
    # - register_module_import: From StdlibImportTransformer
    # - register_function_signature: From FunctionDeclarationTransformer
    # - transform_function: From FunctionDeclarationTransformer
    # - transform_type_decl: From TypeTransformer
    module ProgramTransformer
      # Transform entire program to HighIR::Module
      # Orchestrates 5-pass transformation:
      # 1. collect imports (defers user module import registration)
      # 2. preregister types
      # 3. preregister functions
      # 4. register import aliases (NOW user module imports can be processed)
      # 5. lower declarations
      def transform_program(program)
        module_name = program.module_decl ? program.module_decl.name : "main"
        context = {
          program: program,
          imports: [],
          type_items: [],
          func_items: [],
          module_name: module_name,
          import_aliases: {},
          user_module_imports: []  # legacy – retained for compatibility but unused in v2 path
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

      # Build pass manager with 5 transformation passes
      # Phase 23-D: Added pass 4 (register_import_aliases) between preregister_functions and lower_declarations
      def build_program_pass_manager
        MLC::PassManager.new.tap do |manager|
          manager.register(:collect_imports, method(:pass_collect_imports))
          manager.register(:preregister_types, method(:pass_preregister_types))
          manager.register(:preregister_functions, method(:pass_preregister_functions))
          manager.register(:register_import_aliases, method(:pass_register_import_aliases))  # NEW
          manager.register(:lower_declarations, method(:pass_lower_declarations))
        end
      end

      # Pass 1: Collect imports and register import aliases
      # Phase 23-D: Defers user module import registration until functions are available
      # Processes stdlib imports immediately (they're pre-registered)
      def pass_collect_imports(context)
        program = context[:program]

        program.imports.each do |import_decl|
          context[:imports] << HighIR::Import.new(
            path: import_decl.path,
            items: import_decl.items
          )

          @import_service.process(
            import_decl,
            function_registry: @function_registry,
            type_registry: @type_registry
          )
        end
      end

      # Pass 2: Preregister types
      # Adds all type declarations to type_decl_table for forward references
      def pass_preregister_types(context)
        program = context[:program]

        program.declarations.each do |decl|
          next unless decl.is_a?(AST::TypeDecl)
          @type_decl_table[decl.name] = decl

          type_decl_ir = @type_declaration_service.build(decl)
          context[:type_items] << type_decl_ir
        end
      end

      # Pass 3: Preregister functions
      # Registers function signatures before transforming bodies (enables forward references and recursion)
      def pass_preregister_functions(context)
        program = context[:program]

        program.declarations.each do |decl|
          register_function_signature(decl) if decl.is_a?(AST::FuncDecl)
        end
      end

      # Pass 4: Register user module import aliases
      # Phase 23-D: NEW PASS - Process deferred user module imports
      # Phase 24-B: Load .mlcmeta files before registering import aliases
      # Now that functions are registered, we can create import aliases
      def pass_register_import_aliases(_context)
        # Legacy hook retained for compatibility; imports handled eagerly in pass_collect_imports.
      end

      # Pass 5: Lower declarations
      # Transforms type and function declarations to HighIR
      def pass_lower_declarations(context)
        program = context[:program]

        program.declarations.each do |decl|
          case decl
          when AST::TypeDecl
            @type_resolution_service.refresh_function_signatures!(decl.name)
          when AST::FuncDecl
            context[:func_items] << transform_function(decl)
          end
        end
      end
    end
  end
end

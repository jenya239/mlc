# frozen_string_literal: true

module MLC
  class IRGen
    # ProgramTransformer
    # Program-level transformation orchestration
    # Phase 18-F: Extracted from FunctionTransformer
    #
    # Responsibilities:
    # - Orchestrate full program transformation via pass manager
    # - Collect imports and register import aliases
    # - Preregister types and functions
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
      # Orchestrates 4-pass transformation: collect imports, preregister types, preregister functions, lower declarations
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

      # Build pass manager with 4 transformation passes
      def build_program_pass_manager
        MLC::PassManager.new.tap do |manager|
          manager.register(:collect_imports, method(:pass_collect_imports))
          manager.register(:preregister_types, method(:pass_preregister_types))
          manager.register(:preregister_functions, method(:pass_preregister_functions))
          manager.register(:lower_declarations, method(:pass_lower_declarations))
        end
      end

      # Pass 1: Collect imports and register import aliases
      # Processes both stdlib and user module imports
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

      # Pass 2: Preregister types
      # Adds all type declarations to type_decl_table for forward references
      def pass_preregister_types(context)
        program = context[:program]

        program.declarations.each do |decl|
          next unless decl.is_a?(AST::TypeDecl)
          @type_decl_table[decl.name] = decl
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

      # Pass 4: Lower declarations
      # Transforms type and function declarations to HighIR
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
    end
  end
end

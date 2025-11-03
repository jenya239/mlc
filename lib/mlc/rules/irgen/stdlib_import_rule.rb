# frozen_string_literal: true

require_relative "../../rules/base_rule"

module MLC
  module Rules
    module IRGen
      class StdlibImportRule < BaseRule
        def applies?(import_decl, context = {})
          registry = context[:stdlib_registry]
          registry && registry.module_info(import_decl.path)
        end

        def apply(import_decl, context = {})
          registry = context.fetch(:stdlib_registry)
          module_info = registry.module_info(import_decl.path)
          return unless module_info

          import_all = import_decl.import_all
          requested_items = Array(import_decl.items)

          function_handler = context[:register_stdlib_function]
          type_handler = context[:register_stdlib_type]
          missing_handler = context[:on_missing_item]
          event_bus = context[:event_bus]
          module_alias = context[:module_alias]

          missing_items = []

          if import_all
            module_info.functions.each_value do |metadata|
              function_handler&.call(metadata, module_info, module_alias)
              event_bus&.publish(
                :stdlib_function_imported,
                module: module_info.name,
                function: metadata.name,
                origin: import_decl.origin
              )
            end

            module_info.types.each_value do |metadata|
              type_handler&.call(metadata.ast_node, module_info.namespace, module_info.name)
              event_bus&.publish(
                :stdlib_type_imported,
                module: module_info.name,
                type: metadata.name,
                origin: import_decl.origin
              )

              # Register variant constructors for sum types
              register_variant_constructors(metadata, module_info, module_alias, function_handler, event_bus, import_decl.origin) if metadata.sum_type?
            end
          else
            requested_items.each do |name|
              if (func_meta = module_info.functions[name])
                function_handler&.call(func_meta, module_info, module_alias)
                event_bus&.publish(
                  :stdlib_function_imported,
                  module: module_info.name,
                  function: func_meta.name,
                  origin: import_decl.origin
                )
              elsif (type_meta = module_info.types[name])
                type_handler&.call(type_meta.ast_node, module_info.namespace, module_info.name)
                event_bus&.publish(
                  :stdlib_type_imported,
                  module: module_info.name,
                  type: type_meta.name,
                  origin: import_decl.origin
                )

                # Register variant constructors for sum types
                register_variant_constructors(type_meta, module_info, module_alias, function_handler, event_bus, import_decl.origin) if type_meta.sum_type?
              else
                missing_items << name
              end
            end
          end

          missing_items.uniq.each do |missing|
            event_bus&.publish(
              :stdlib_missing_item,
              module: module_info.name,
              item: missing,
              origin: import_decl.origin
            )
            missing_handler.call(missing, import_decl.origin) if missing_handler
          end
        end

        private

        # Register variant constructors as identifiers for sum types
        # For each variant in the sum type, make the constructor available
        def register_variant_constructors(type_metadata, module_info, module_alias, function_handler, event_bus, origin)
          return unless function_handler

          # Each variant becomes a constructor "function"
          type_metadata.variants.each do |variant|
            # Create synthetic FunctionMetadata for the constructor
            # For now, we treat constructors as special identifiers
            # The actual type checking and IR generation will be handled elsewhere
            constructor_meta = create_constructor_metadata(variant, type_metadata, module_info)

            function_handler.call(constructor_meta, module_info, module_alias)

            event_bus&.publish(
              :stdlib_constructor_imported,
              module: module_info.name,
              type: type_metadata.name,
              constructor: variant[:name],
              origin: origin
            )
          end
        end

        # Create synthetic FunctionMetadata for a sum type variant constructor
        def create_constructor_metadata(variant, type_metadata, module_info)
          # Build a synthetic AST FuncDecl node for the constructor
          # Constructor signature: VariantName(field_types...) -> TypeName
          variant_name = variant[:name]
          variant_fields = variant[:fields] || []

          # Create parameter list from variant fields
          params = variant_fields.map.with_index do |field, idx|
            AST::Param.new(
              name: field[:name] || "arg#{idx}",
              type: field[:type],
              origin: nil
            )
          end

          # Return type is the sum type itself
          return_type = type_metadata.ast_node.type

          # Create synthetic function declaration
          constructor_decl = AST::FuncDecl.new(
            name: variant_name,
            params: params,
            ret_type: return_type,
            body: nil,  # Constructors have no body
            type_params: type_metadata.ast_node.type_params,
            exported: true,
            external: true,  # Mark as external so it's treated specially
            origin: nil
          )

          MLC::FunctionMetadata.new(
            name: variant_name,
            qualified_name: "#{module_info.namespace}::#{variant_name}",
            extern: true,
            params: params.map { |p| { name: p.name, type: p.type } },
            return_type: return_type,
            ast_node: constructor_decl
          )
        end
      end
    end
  end
end

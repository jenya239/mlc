# frozen_string_literal: true

require_relative '../../core/function_signature'
require_relative '../../compiler/stdlib/signature_registry'

module MLC
  module SemanticGen
    module Services
      # ImportService - handles stdlib and user module imports for SemanticGen
      class ImportService
        def initialize(stdlib_registry:, module_resolver:, type_builder:, type_declaration_service:, type_registration_service:, type_checker:)
          @stdlib_registry = stdlib_registry
          @module_resolver = module_resolver
          @type_builder = type_builder
          @type_declaration_service = type_declaration_service
          @type_registration_service = type_registration_service
          @type_checker = type_checker
        end

        def process(import_decl, function_registry:, type_registry:)
          if stdlib_module?(import_decl.path)
            process_stdlib_import(import_decl, function_registry: function_registry, type_registry: type_registry)
          else
            process_user_import(import_decl, function_registry: function_registry)
          end
        end

        private

        def stdlib_module?(name)
          @stdlib_registry.module_info(name)
        end

        def process_stdlib_import(import_decl, function_registry:, type_registry:)
          module_info = @stdlib_registry.module_info(import_decl.path)
          return unless module_info

          functions = module_info.functions
          types = module_info.types

          if import_decl.import_all
            functions.each_value do |meta|
              register_function(meta, module_info, function_registry, import_decl.alias)
            end
            types.each_value do |meta|
              register_type(meta, module_info)
            end
          else
            Array(import_decl.items).each do |item|
              if (meta = functions[item])
                register_function(meta, module_info, function_registry, import_decl.alias)
              elsif (meta = types[item])
                register_type(meta, module_info)
              else
                raise MLC::CompileError, "Unknown item '#{item}' in import '#{import_decl.path}'"
              end
            end
          end
        end

        def register_function(metadata, module_info, function_registry, alias_name)
          decl = metadata.ast_node
          type_params = decl ? @type_checker.normalize_type_params(decl.type_params) : []

          params = nil
          ret_type = nil

          @type_builder.with_type_params(type_params) do
            params = metadata.params.each_with_index.map do |param, index|
              type_ir = transform_type(param[:type])
              { name: param[:name] || "arg#{index}", type: type_ir }
            end
            ret_type = transform_type(metadata.return_type)
          end

          canonical_name = metadata.qualified_name || [module_info.name, metadata.name].join('.')

          info = MLC::Core::FunctionSignature.new(
            metadata.name,
            params.map { |p| p[:type] },
            ret_type,
            type_params
          )

          function_registry.register(
            canonical_name,
            info,
            module_name: module_info.name,
            namespace: module_info.namespace,
            exported: true,
            external: metadata.extern?
          )

          register_aliases(function_registry, metadata.name, canonical_name, module_info.name, alias_name)
        end

        def register_aliases(function_registry, function_name, canonical_name, module_name, alias_name)
          prefixes = []
          prefixes << module_name if module_name && !module_name.empty?
          prefixes << alias_name if alias_name && !alias_name.empty?
          prefixes = prefixes.compact.uniq

          prefixes.each do |prefix|
            alias_key = [prefix, function_name].join('.')
            next if alias_key == canonical_name
            safe_register_alias(function_registry, alias_key, canonical_name)
          end

          if alias_name.nil?
            safe_register_alias(function_registry, function_name, canonical_name)
          end
        end

        def safe_register_alias(function_registry, alias_name, canonical_name)
          function_registry.register_alias(alias_name, canonical_name)
        rescue KeyError, ArgumentError
          nil
        end

        def register_type(metadata, module_info)
          decl = metadata.ast_node
          type_params = @type_checker.normalize_type_params(decl.type_params)

          core_type = nil
          @type_builder.with_type_params(type_params) do
            core_type = @type_builder.transform(decl.type)
          end
          core_type = rename_core_type(core_type, decl.name)
          kind = @type_checker.infer_type_kind(decl, core_type)

          @type_registration_service.register(
            decl: decl,
            core_type: core_type,
            namespace: module_info.namespace,
            kind: kind,
            exported: decl.exported,
            module_name: module_info.name
          )
        end

        def rename_core_type(core_type, name)
          case core_type
          when MLC::SemanticIR::RecordType
            MLC::SemanticIR::Builder.record_type(name, core_type.fields)
          when MLC::SemanticIR::SumType
            MLC::SemanticIR::Builder.sum_type(name, core_type.variants)
          else
            core_type
          end
        end

        def transform_type(type_ast)
          return nil unless type_ast

          @type_builder.transform(type_ast)
        end

        def process_user_import(import_decl, function_registry:)
          @module_resolver.register_module_import(import_decl, function_registry)
        end
      end
    end
  end
end

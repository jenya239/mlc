# frozen_string_literal: true

module MLC
  module Backend
    # MetadataGenerator
    # Phase 24-A: Generate .mlcmeta files for module exports
    #
    # Responsibilities:
    # - Extract exported functions from SemanticIR::Module
    # - Extract exported types (records, sum types)
    # - Generate JSON metadata for cross-module compilation
    #
    # Output format (.mlcmeta):
    # {
    #   "module_name": "math",
    #   "exports": {
    #     "functions": [...],
    #     "types": [...],
    #     "sum_types": [...]
    #   }
    # }
    class MetadataGenerator
      # Generate metadata hash from SemanticIR::Module
      # @param semantic_ir_module [SemanticIR::Module] The compiled module
      # @return [Hash] Metadata structure
      def generate(semantic_ir_module)
        {
          module_name: semantic_ir_module.name,
          exports: {
            functions: extract_exported_functions(semantic_ir_module),
            types: extract_exported_types(semantic_ir_module),
            sum_types: extract_exported_sum_types(semantic_ir_module)
          }
        }
      end

      # Write metadata to JSON file
      # @param metadata [Hash] Metadata structure
      # @param output_path [String] Path to .mlcmeta file
      def write(metadata, output_path)
        require 'json'
        File.write(output_path, JSON.pretty_generate(metadata))
      end

      private

      # Extract exported functions from module
      # Only includes functions marked with 'export' keyword
      def extract_exported_functions(semantic_ir_module)
        semantic_ir_module.items.filter_map do |item|
          next unless item.is_a?(MLC::SemanticIR::Func)
          next unless item.exported

          {
            name: item.name,
            params: item.params.map { |p| serialize_param(p) },
            return_type: serialize_type(item.ret_type),
            is_exported: true
          }
        end
      end

      # Extract exported record types
      def extract_exported_types(semantic_ir_module)
        semantic_ir_module.items.filter_map do |item|
          next unless item.is_a?(MLC::SemanticIR::TypeDecl)
          next unless item.exported
          next unless item.type.is_a?(MLC::SemanticIR::RecordType)

          {
            name: item.name,
            kind: "record",
            fields: item.type.fields.map { |f| serialize_field(f) },
            is_exported: true
          }
        end
      end

      # Extract exported sum types
      def extract_exported_sum_types(semantic_ir_module)
        semantic_ir_module.items.filter_map do |item|
          next unless item.is_a?(MLC::SemanticIR::TypeDecl)
          next unless item.exported
          next unless item.type.is_a?(MLC::SemanticIR::SumType)

          {
            name: item.name,
            kind: "sum",
            type_params: item.type_params || [],
            variants: item.type.variants.map { |v| v[:name] },
            is_exported: true
          }
        end
      end

      # Serialize function parameter to metadata format
      def serialize_param(param)
        {
          name: param.name,
          type: serialize_type(param.type)
        }
      end

      # Serialize field to metadata format
      def serialize_field(field)
        {
          name: field[:name] || field.name,
          type: serialize_type(field[:type] || field.type)
        }
      end

      # Serialize SemanticIR type to string representation
      # Handles primitives, generics, records, etc.
      def serialize_type(type)
        case type
        when MLC::SemanticIR::RecordType
          type.name
        when MLC::SemanticIR::GenericType
          # e.g., "Result<T, E>" or "List<i32>"
          type_args = type.type_args.map { |arg| serialize_type(arg) }.join(", ")
          "#{type.base_type.name}<#{type_args}>"
        when MLC::SemanticIR::TypeVariable
          type.name
        when MLC::SemanticIR::FunctionType
          # e.g., "(i32, i32) -> i32"
          param_types = type.param_types.map { |pt| serialize_type(pt) }.join(", ")
          return_type = serialize_type(type.return_type)
          "(#{param_types}) -> #{return_type}"
        when MLC::SemanticIR::Type
          # Base type class - includes primitives (kind: :prim)
          type.name.to_s
        when String
          type
        else
          # Fallback: inspect the type
          type.respond_to?(:name) ? type.name.to_s : type.to_s
        end
      end
    end
  end
end

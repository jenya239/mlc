# frozen_string_literal: true

require 'json'

module MLC
  module Services
    # MetadataLoaderService
    # Phase 24-B: Load .mlcmeta files and register exported symbols
    #
    # Responsibilities:
    # - Load metadata from .mlcmeta JSON files
    # - Parse type strings back into HighIR types
    # - Register exported functions in FunctionRegistry
    # - Register exported types in TypeRegistry
    # - Enable cross-module compilation and imports
    #
    # Dependencies (injected):
    # - function_registry: FunctionRegistry
    # - type_registry: TypeRegistry
    class MetadataLoaderService
      def initialize(function_registry:, type_registry:)
        @function_registry = function_registry
        @type_registry = type_registry
      end

      # Load metadata from .mlcmeta file and register symbols
      # @param metadata_path [String] Path to .mlcmeta file
      # @return [Boolean] true if loaded successfully, false if file doesn't exist
      def load(metadata_path)
        return false unless File.exist?(metadata_path)

        metadata = JSON.parse(File.read(metadata_path))
        module_name = metadata["module_name"]

        # Register exported functions
        metadata["exports"]["functions"].each do |func_meta|
          register_function_from_metadata(func_meta, module_name)
        end

        # Register exported types (records)
        metadata["exports"]["types"].each do |type_meta|
          register_type_from_metadata(type_meta, module_name)
        end

        # Register sum types
        metadata["exports"]["sum_types"].each do |sum_type_meta|
          register_sum_type_from_metadata(sum_type_meta, module_name)
        end

        true
      end

      private

      # Register a function from metadata
      def register_function_from_metadata(func_meta, module_name)
        # Parse parameter types
        param_types = func_meta["params"].map do |param|
          parse_type(param["type"])
        end

        # Parse return type
        return_type = parse_type(func_meta["return_type"])

        # Create FunctionInfo
        info = MLC::IRGen::FunctionInfo.new(
          func_meta["name"],
          param_types,
          return_type,
          [] # type_params - will be empty for imported functions
        )

        # Register in function registry
        @function_registry.register(
          func_meta["name"],
          info,
          module_name: module_name,
          exported: true,
          external: true # Imported functions are treated as external
        )
      end

      # Register a record type from metadata
      def register_type_from_metadata(type_meta, module_name)
        return unless type_meta["kind"] == "record"

        # Parse field types
        fields = type_meta["fields"].map do |field_meta|
          {
            name: field_meta["name"],
            type: parse_type(field_meta["type"])
          }
        end

        # Create RecordType
        record_type = MLC::HighIR::RecordType.new(
          name: type_meta["name"],
          fields: fields
        )

        # Register in type registry
        @type_registry.register(
          type_meta["name"],
          core_ir_type: record_type,
          kind: :record,
          exported: true,
          module_name: module_name
        )
      end

      # Register a sum type from metadata
      def register_sum_type_from_metadata(sum_type_meta, module_name)
        return unless sum_type_meta["kind"] == "sum"

        # For sum types, we just register the type name
        # Variants will be handled separately as they're also constructors
        variants = sum_type_meta["variants"].map do |variant_name|
          { name: variant_name, fields: [] }
        end

        sum_type = MLC::HighIR::SumType.new(
          name: sum_type_meta["name"],
          variants: variants
        )

        @type_registry.register(
          sum_type_meta["name"],
          core_ir_type: sum_type,
          kind: :sum,
          exported: true,
          module_name: module_name
        )
      end

      # Parse type string back into HighIR::Type
      # Handles primitives, arrays, generics, and function types
      def parse_type(type_str)
        return nil if type_str.nil?

        # Handle array types: "i32[]"
        if type_str.end_with?("[]")
          element_type_str = type_str[0...-2]
          element_type = parse_type(element_type_str)
          return MLC::HighIR::ArrayType.new(element_type: element_type)
        end

        # Handle generic types: "Result<T, E>" or "List<i32>"
        if type_str =~ /^([A-Za-z_][A-Za-z0-9_]*)<(.+)>$/
          base_name = Regexp.last_match(1)
          type_args_str = Regexp.last_match(2)

          base_type = MLC::HighIR::Type.new(kind: :generic, name: base_name)
          type_args = parse_type_args(type_args_str)

          return MLC::HighIR::GenericType.new(
            base_type: base_type,
            type_args: type_args
          )
        end

        # Handle function types: "(i32, i32) -> i32"
        if type_str =~ /^\((.+)\)\s*->\s*(.+)$/
          param_types_str = Regexp.last_match(1)
          return_type_str = Regexp.last_match(2)

          param_types = param_types_str.split(',').map do |pt|
            { type: parse_type(pt.strip) }
          end
          return_type = parse_type(return_type_str.strip)

          return MLC::HighIR::FunctionType.new(
            params: param_types,
            ret_type: return_type
          )
        end

        # Primitive or named type
        MLC::HighIR::Type.new(kind: :prim, name: type_str)
      end

      # Parse comma-separated type arguments
      # Handles nested generics correctly
      def parse_type_args(type_args_str)
        args = []
        current = ""
        depth = 0

        type_args_str.each_char do |char|
          case char
          when '<'
            depth += 1
            current += char
          when '>'
            depth -= 1
            current += char
          when ','
            if depth == 0
              args << parse_type(current.strip)
              current = ""
            else
              current += char
            end
          else
            current += char
          end
        end

        args << parse_type(current.strip) unless current.strip.empty?
        args
      end
    end
  end
end

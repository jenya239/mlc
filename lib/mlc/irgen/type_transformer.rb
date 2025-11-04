# frozen_string_literal: true

module MLC
  class IRGen
    # TypeTransformer
    # Type AST → HighIR transformation
    # Phase 18-F: Extracted from FunctionTransformer
    #
    # Responsibilities:
    # - Transform AST type nodes to HighIR types
    # - Transform type declarations (record, sum, opaque types)
    # - Build type declarations for imports
    # - Register types in TypeRegistry
    #
    # Dependencies (from IRGen):
    # - @type_checker_service: For validate_type_constraints, infer_type_kind, normalize_type_params
    # - @type_registry: For type registration
    # - @module_context_service: For current_module_name
    # - @rule_engine: For applying type_decl rules
    # - current_type_params: From TransformationContext
    # - with_type_params: From TransformationContext
    # - with_current_node: From TransformationContext
    module TypeTransformer
      # Transform AST type to HighIR type
      # Handles: PrimType, OpaqueType, GenericType, FunctionType, RecordType, SumType, ArrayType
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

      # Transform type declaration (type Point = {x: i32, y: i32})
      # Registers type in TypeRegistry and applies rules
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
          type_decl = HighIR::TypeDecl.new(name: decl.name, type: type, type_params: type_params, exported: decl.exported)

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

      # Build type declaration for import (used by stdlib imports)
      # Transforms type without registering in main module
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

        HighIR::TypeDecl.new(name: decl.name, type: type, type_params: type_params, exported: decl.exported)
      end
    end
  end
end

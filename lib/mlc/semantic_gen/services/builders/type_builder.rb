# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # TypeBuilder - lowers AST type syntax to SemanticIR types in v2 pipeline
      class TypeBuilder
        def initialize(ir_builder:, type_checker:)
          @ir_builder = ir_builder
          @type_checker = type_checker
          @type_params_stack = []
        end

        def with_type_params(params)
          @type_params_stack.push(Array(params))
          yield
        ensure
          @type_params_stack.pop
        end

        def transform(node)
          case node
          when MLC::AST::PrimType
            transform_prim(node)
          when MLC::AST::OpaqueType
            @ir_builder.opaque_type(name: node.name)
          when MLC::AST::GenericType
            base = transform(node.base_type)
            args = node.type_params.map { |type| transform(type) }

            # Validate type constraints if base type has constraints
            if node.base_type.respond_to?(:name)
              @type_checker.validate_type_constraints(node.base_type.name, node.type_params)
            end

            @ir_builder.generic_type(base_type: base, type_args: args)
          when MLC::AST::FunctionType
            params = node.param_types.each_with_index.map do |type, index|
              { name: "arg#{index}", type: transform(type) }
            end
            ret = transform(node.ret_type)
            @ir_builder.function_type(params: params, return_type: ret)
          when MLC::AST::RecordType
            fields = node.fields.map { |field| { name: field[:name], type: transform(field[:type]) } }
            @ir_builder.record_type(name: node.name, fields: fields)
          when MLC::AST::SumType
            variants = node.variants.map do |variant|
              fields = Array(variant[:fields]).map do |field|
                { name: field[:name], type: transform(field[:type]) }
              end
              { name: variant[:name], fields: fields }
            end
            @ir_builder.sum_type(name: node.name, variants: variants)
          when MLC::AST::ArrayType
            element = transform(node.element_type)
            @ir_builder.array_type(element_type: element)
          else
            raise MLC::CompileError, "Unsupported type node #{node.class}"
          end
        end

        private

        def transform_prim(node)
          param = find_type_param(node.name)
          return @ir_builder.type_variable(name: node.name, constraint: param&.constraint) if param

          @ir_builder.prim_type(name: node.name)
        end

        def find_type_param(name)
          @type_params_stack.reverse_each do |params|
            candidate = params.find { |param| param.name == name }
            return candidate if candidate
          end
          nil
        end
      end
    end
  end
end

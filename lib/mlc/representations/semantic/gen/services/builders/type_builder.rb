# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # TypeBuilder - lowers AST type syntax to SemanticIR types in v2 pipeline
          class TypeBuilder
            def initialize(ir_builder:, type_checker:, scope_context: nil)
              @ir_builder = ir_builder
              @type_checker = type_checker
              @scope_context = scope_context
              @type_params_stack = []
            end

            attr_writer :scope_context

            def with_type_params(params)
              @type_params_stack.push(Array(params))
              yield
            ensure
              @type_params_stack.pop
            end

            def transform(node)
              case node
              when MLC::Source::AST::PrimType
                transform_prim(node)
              when MLC::Source::AST::OpaqueType
                @ir_builder.opaque_type(name: node.name)
              when MLC::Source::AST::GenericType
                base = transform(node.base_type)
                args = node.type_params.map { |type| transform(type) }

                # Validate type constraints if base type has constraints
                if node.base_type.respond_to?(:name)
                  @type_checker.validate_type_constraints(node.base_type.name, node.type_params)
                end

                @ir_builder.generic_type(base_type: base, type_args: args)
              when MLC::Source::AST::FunctionType
                params = node.param_types.each_with_index.map do |type, index|
                  { name: "arg#{index}", type: transform(type) }
                end
                ret = transform(node.ret_type)
                @ir_builder.function_type(params: params, return_type: ret)
              when MLC::Source::AST::RecordType
                fields = node.fields.map { |field| { name: field[:name], type: transform(field[:type]) } }
                @ir_builder.record_type(name: node.name, fields: fields)
              when MLC::Source::AST::SumType
                variants = node.variants.map do |variant|
                  fields = Array(variant[:fields]).map do |field|
                    { name: field[:name], type: transform(field[:type]) }
                  end
                  { name: variant[:name], fields: fields }
                end
                @ir_builder.sum_type(name: node.name, variants: variants)
              when MLC::Source::AST::ArrayType
                element = transform(node.element_type)
                @ir_builder.array_type(element_type: element)
              when MLC::Source::AST::RefType
                validate_unsafe_context!(node, "ref T")
                inner = transform(node.inner_type)
                @ir_builder.ref_type(inner_type: inner)
              when MLC::Source::AST::MutRefType
                validate_unsafe_context!(node, "ref mut T")
                inner = transform(node.inner_type)
                @ir_builder.mut_ref_type(inner_type: inner)
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

            # Validate that reference types are used only in unsafe context
            def validate_unsafe_context!(node, type_desc)
              return if @scope_context.nil?  # Skip if no scope context (e.g., tests)
              return if @scope_context.inside_unsafe?

              raise MLC::CompileError, "Reference type #{type_desc} can only be used in unsafe blocks"
            end
          end
        end
      end
    end
  end
end

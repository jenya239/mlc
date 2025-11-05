# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # MemberAccessService - resolves HighIR types for member access expressions
      class MemberAccessService
        ARRAY_FUNCTION_MEMBERS = %w[map filter fold].freeze
        ARRAY_VALUE_MEMBERS = {
          'length' => 'i32',
          'size' => 'i32',
          'is_empty' => 'bool'
        }.freeze
        STRING_VALUE_MEMBERS = {
          'trim' => 'string',
          'trim_start' => 'string',
          'trim_end' => 'string',
          'upper' => 'string',
          'lower' => 'string',
          'is_empty' => 'bool',
          'length' => 'i32'
        }.freeze
        STRING_ARRAY_MEMBERS = %w[split].freeze

        def initialize(type_checker:, type_registry:, type_decl_table:, ir_builder:)
          @type_checker = type_checker
          @type_registry = type_registry
          @type_decl_table = type_decl_table
          @ir_builder = ir_builder
        end

        # Resolve the HighIR type for a member access
        def resolve(object_type, member, node: nil)
          @type_checker.ensure_type!(object_type, "member access '#{member}' has unknown object type", node: node)

          if object_type.is_a?(MLC::HighIR::GenericType)
            return resolve_generic_member(object_type, member, node: node)
          end

          resolve_concrete_member(object_type, member, node: node)
        end

        private

        def resolve_generic_member(generic_type, member, node:)
          base_name = @type_checker.type_name(generic_type.base_type)
          member_type = @type_registry.resolve_member(base_name, member)
          return substitute_generic_member(base_name, member_type, generic_type, node: node) if member_type

          base_core_type = @type_registry.lookup(base_name)&.core_ir_type
          return resolve_concrete_member(base_core_type, member, node: node) if base_core_type

          resolve_concrete_member(generic_type.base_type, member, node: node)
        end

        def substitute_generic_member(base_name, member_type, generic_type, node:)
          return resolve_concrete_member(member_type, nil, node: node) unless member_type

          decl = @type_decl_table[base_name]
          return member_type unless decl && decl.type_params.any?

          type_map = {}
          decl.type_params.each_with_index do |param, index|
            actual = generic_type.type_args[index]
            type_map[param.name] = actual if actual
          end

          substitute_type(member_type, type_map)
        end

        def resolve_concrete_member(object_type, member, node:)
          if object_type.respond_to?(:name) && object_type.name && (field_type = @type_registry.resolve_member(object_type.name, member))
            return field_type
          end

          case object_type
          when MLC::HighIR::RecordType
            field = object_type.fields.find { |f| f[:name] == member }
            return field[:type] if field
          when MLC::HighIR::ArrayType
            return resolve_array_member(object_type, member, node: node)
          when MLC::HighIR::Type
            normalized = @type_checker.normalized_type_name(@type_checker.type_name(object_type))
            return resolve_string_member(member, node: node) if normalized == 'string'
            return resolve_numeric_member(member, object_type: object_type, node: node) if @type_checker.numeric_type?(object_type)
          end

          raise_unknown_member(object_type, member, node: node)
        end

        def resolve_array_member(object_type, member, node:)
          if ARRAY_VALUE_MEMBERS.key?(member)
            return primitive_type(ARRAY_VALUE_MEMBERS.fetch(member))
          end

          if ARRAY_FUNCTION_MEMBERS.include?(member)
            return function_placeholder(return_type: primitive_type('auto'))
          end

          raise_unknown_member(object_type, member, node: node, known: (ARRAY_VALUE_MEMBERS.keys + ARRAY_FUNCTION_MEMBERS))
        end

        def resolve_string_member(member, node:)
          if STRING_VALUE_MEMBERS.key?(member)
            return primitive_type(STRING_VALUE_MEMBERS.fetch(member))
          end

          if STRING_ARRAY_MEMBERS.include?(member)
            element = primitive_type('string')
            return MLC::HighIR::ArrayType.new(element_type: element)
          end

          raise MLC::CompileError, "Unknown string member '#{member}'. Known members: #{(STRING_VALUE_MEMBERS.keys + STRING_ARRAY_MEMBERS).join(', ')}"
        end

        def resolve_numeric_member(member, object_type:, node:)
          if member == 'sqrt'
            return primitive_type('f32')
          end

          raise_unknown_member(object_type, member, node: node)
        end

        def substitute_type(type, type_map)
          case type
          when MLC::HighIR::TypeVariable
            type_map[type.name] || type
          when MLC::HighIR::GenericType
            base = substitute_type(type.base_type, type_map)
            args = type.type_args.map { |arg| substitute_type(arg, type_map) }
            return type if base.equal?(type.base_type) && args == type.type_args
            MLC::HighIR::Builder.generic_type(base, args)
          when MLC::HighIR::ArrayType
            element = substitute_type(type.element_type, type_map)
            return type if element.equal?(type.element_type)
            MLC::HighIR::Builder.array_type(element)
          when MLC::HighIR::FunctionType
            params = type.params.map do |param|
              substituted = substitute_type(param[:type], type_map)
              substituted.equal?(param[:type]) ? param : { name: param[:name], type: substituted }
            end
            ret = substitute_type(type.ret_type, type_map)
            return type if params == type.params && ret.equal?(type.ret_type)
            MLC::HighIR::Builder.function_type(params, ret)
          else
            type
          end
        end

        def primitive_type(name)
          @ir_builder.prim_type(name: name)
        end

        def function_placeholder(return_type:)
          params = []
          MLC::HighIR::Builder.function_type(params, return_type)
        end

        def raise_unknown_member(object_type, member, node:, known: nil)
          description = @type_checker.describe_type(object_type)
          message = "Unknown member '#{member}' for type #{description}"
          message += ". Known members: #{Array(known).join(', ')}" if known && !known.empty?
          @type_checker.type_error(message, node: node)
        end
      end
    end
  end
end

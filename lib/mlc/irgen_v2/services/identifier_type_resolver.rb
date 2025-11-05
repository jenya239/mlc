# frozen_string_literal: true

require_relative '../../services/var_type_registry'
require_relative '../../services/type_checker'

module MLC
  module IRGenV2
    module Services
      # IdentifierTypeResolver - Resolve HighIR types for identifiers
      class IdentifierTypeResolver
        def initialize(function_registry:, ir_builder:, var_type_registry:)
          @function_registry = function_registry
          @ir_builder = ir_builder
          @var_type_registry = var_type_registry
        end

        def type_for(name)
          return @var_type_registry.get(name) if @var_type_registry.has?(name)

          if (entry = @function_registry.fetch_entry(name))
            return function_type_from_info(entry.info)
          end

          if (io_kind = io_return_kind(name))
            return io_type(io_kind)
          end

          return @ir_builder.prim_type(name: 'bool') if %w[true false].include?(name)

          scope = @var_type_registry.keys.sort.join(', ')
          message = "Unknown identifier '#{name}'"
          message += " (in scope: #{scope})" unless scope.empty?
          raise MLC::CompileError, message
        end

        private

        def function_type_from_info(info)
          params = info&.param_types || []
          ret_type = info&.ret_type || @ir_builder.prim_type(name: 'void')
          @ir_builder.function_type(params: params, return_type: ret_type)
        end

        def io_return_kind(name)
          MLC::Services::TypeChecker::IO_RETURN_TYPES[name]
        end

        def io_type(kind)
          case kind
          when 'i32'
            @ir_builder.prim_type(name: 'i32')
          when 'string'
            @ir_builder.prim_type(name: 'string')
          when :array_of_string
            element = @ir_builder.prim_type(name: 'string')
            @ir_builder.array_type(element_type: element)
          else
            @ir_builder.prim_type(name: 'i32')
          end
        end
      end
    end
  end
end

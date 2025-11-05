# frozen_string_literal: true

require 'test_helper'
require 'mlc/irgen_v2/engine'

module MLC
  module IRGenV2
    module Services
      class ContainerIntegrationTest < Minitest::Test
        def setup
          @function_registry = MLC::FunctionRegistry.new
          @type_registry = MLC::TypeRegistry.new
          @container = Container.new(function_registry: @function_registry, type_registry: @type_registry)
        end

        def test_sum_type_constructor_registration
          builder = @container.ir_builder
          some_variant = { name: 'Some', fields: [{ name: 'value', type: builder.prim_type(name: 'i32') }] }
          none_variant = { name: 'None', fields: [] }
          sum_type = builder.sum_type(name: 'Option', variants: [some_variant, none_variant])

          type_decl = MLC::AST::TypeDecl.new(name: 'Option', type: nil)

          @container.type_registration_service.register(
            decl: type_decl,
            core_type: sum_type,
            namespace: nil,
            kind: :sum,
            exported: true
          )

          constructor_info = @container.type_unification_service.constructor_info_for('Some', sum_type)
          refute_nil constructor_info
          assert_equal 1, constructor_info.param_types.size
          assert_equal 'i32', constructor_info.param_types.first.name
        end

        def test_module_import_alias_registration
          builder = @container.ir_builder
          info = MLC::IRGen::FunctionInfo.new('sqrt', [], builder.prim_type(name: 'f64'))
          @function_registry.register('Math.sqrt', info, module_name: 'Math')

          import_decl = MLC::AST::ImportDecl.new(path: 'Math', items: ['sqrt'], import_all: false, alias_name: 'M')

          @container.module_resolver.register_module_import(import_decl, @function_registry)

          assert_equal info, @function_registry.fetch('M.sqrt')
          assert_equal info, @function_registry.fetch('sqrt')
        end
      end
    end
  end
end

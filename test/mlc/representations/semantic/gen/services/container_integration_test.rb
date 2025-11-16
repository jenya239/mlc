# frozen_string_literal: true

require 'test_helper'
require 'mlc/representations/semantic/gen/engine'

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          class ContainerIntegrationTest < Minitest::Test
            def setup
              @function_registry = MLC::Registries::FunctionRegistry.new
              @type_registry = MLC::Registries::TypeRegistry.new
              @container = Container.new(
                function_registry: @function_registry,
                type_registry: @type_registry
              )
            end

            def test_sum_type_constructor_registration
              builder = @container.ir_builder
              some_variant = { name: 'Some', fields: [{ name: 'value', type: builder.prim_type(name: 'i32') }] }
              none_variant = { name: 'None', fields: [] }
              sum_type = builder.sum_type(name: 'Option', variants: [some_variant, none_variant])

              type_decl = MLC::Source::AST::TypeDecl.new(name: 'Option', type: nil)

              @container.type_registration_service.register(
                decl: type_decl,
                core_type: sum_type,
                namespace: nil,
                kind: :sum,
                exported: true
              )

              constructor_info = @container.type_unification_service.constructor_info_for('Some', sum_type)
              refute_nil constructor_info
              assert_equal 'i32', constructor_info.param_types.first.name
            end

            def test_type_declaration_service_registers_sum_type
              sum_ast = MLC::Source::AST::SumType.new(
                name: 'Maybe',
                variants: [
                  { name: 'Just', fields: [{ name: 'value', type: MLC::Source::AST::PrimType.new(name: 'i32') }] },
                  { name: 'Nothing', fields: [] }
                ]
              )

              decl = MLC::Source::AST::TypeDecl.new(name: 'Maybe', type: sum_ast)

              type_decl_ir = @container.type_declaration_service.build(decl)

              assert_instance_of MLC::SemanticIR::TypeDecl, type_decl_ir
              constructor_info = @container.type_unification_service.constructor_info_for('Just', type_decl_ir.type)
              refute_nil constructor_info
              assert_equal 'i32', constructor_info.param_types.first.name
            end

            def test_stdlib_import_registers_function_and_type
              import_decl = MLC::Source::AST::ImportDecl.new(path: 'Conv', items: ['to_string_i32'], import_all: false, alias_name: 'C')

              @container.import_service.process(
                import_decl,
                function_registry: @function_registry,
                type_registry: @type_registry
              )

              assert @function_registry.fetch('Conv.to_string_i32'), 'expected canonical stdlib function'
              assert @function_registry.fetch('C.to_string_i32'), 'expected alias for stdlib function'
            end

            def test_stdlib_import_registers_sum_type_constructors
              import_decl = MLC::Source::AST::ImportDecl.new(path: 'Option', items: ['Option'], import_all: false, alias_name: nil)

              @container.import_service.process(
                import_decl,
                function_registry: @function_registry,
                type_registry: @type_registry
              )

              type_info = @type_registry.lookup('Option')
              refute_nil type_info
              sum_type = type_info.core_ir_type
              constructor_info = @container.type_unification_service.constructor_info_for('Some', sum_type)
              refute_nil constructor_info
            end

            def test_user_import_registers_alias
              info = MLC::Registries::FunctionSignature.new('bar', [], @container.ir_builder.prim_type(name: 'i32'))
              @function_registry.register('Foo.bar', info, module_name: 'Foo')

              import_decl = MLC::Source::AST::ImportDecl.new(path: 'Foo', items: ['bar'], import_all: false, alias_name: 'Alias')

              @container.import_service.process(
                import_decl,
                function_registry: @function_registry,
                type_registry: @type_registry
              )

              assert_equal info, @function_registry.fetch('Alias.bar')
            end
          end
        end
      end
    end
  end
end

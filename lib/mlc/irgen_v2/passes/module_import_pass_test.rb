# frozen_string_literal: true

require 'test_helper'
require 'mlc/irgen_v2/services/import_service'
require 'mlc/irgen_v2/passes/module_import_pass'

module MLC
  module IRGenV2
    module Passes
      class ModuleImportPassTest < Minitest::Test
        def setup
          function_registry = MLC::FunctionRegistry.new
          type_registry = MLC::TypeRegistry.new
          container = Services::Container.new(
            function_registry: function_registry,
            type_registry: type_registry
          )

          @pass = ModuleImportPass.new(
            import_service: container.import_service,
            function_registry: function_registry,
            type_registry: type_registry
          )
          @function_registry = function_registry
          @type_registry = type_registry
        end

        def test_stdlib_imports_are_registered
          ast = MLC::AST::Program.new(
            module_decl: nil,
            imports: [MLC::AST::ImportDecl.new(path: 'Conv', items: ['to_string_i32'], import_all: false, alias_name: 'C')],
            declarations: []
          )

          @pass.run(ast)

          assert @function_registry.fetch('Conv.to_string_i32')
          assert @function_registry.fetch('C.to_string_i32')
        end
      end
    end
  end
end

# frozen_string_literal: true

module MLC
  module Common
    module ModularCompilation
      # Registers types and functions from SemanticIR::Module into registries.
      # Step 10 of modular compilation plan.
      class IrModuleRegistrar
        def initialize(type_registry:, function_registry:, type_decl_table:, sum_type_constructor_service:)
          @type_registry = type_registry
          @function_registry = function_registry
          @type_decl_table = type_decl_table
          @sum_type_constructor_service = sum_type_constructor_service
        end

        def register(module_ir, module_name: nil)
          module_name ||= module_ir.name || "main"

          module_ir.items.each do |item|
            case item
            when MLC::SemanticIR::TypeDecl
              register_type(item, module_name)
            when MLC::SemanticIR::Func
              register_func(item, module_name)
            end
          end
        end

        private

        def register_type(decl, module_name)
          return if @type_registry.lookup(decl.name)

          @type_decl_table[decl.name] = decl
          kind = kind_from_ir(decl.type)
          @type_registry.register(
            decl.name,
            core_ir_type: decl.type,
            kind: kind,
            exported: decl.exported,
            module_name: module_name
          )
          @sum_type_constructor_service.register_sum_type_constructors(decl.name, decl.type) if decl.type.is_a?(MLC::SemanticIR::SumType)
        end

        def kind_from_ir(core_type)
          return :record if core_type.is_a?(MLC::SemanticIR::RecordType)
          return :sum if core_type.is_a?(MLC::SemanticIR::SumType)
          return :opaque if core_type.is_a?(MLC::SemanticIR::OpaqueType)

          :record
        end

        def register_func(func, module_name)
          return if @function_registry.registered?(func.name)

          ptypes = func.params.map(&:type)
          req = func.params.find_index { |p| p.default } || ptypes.length
          pnames = func.params.map(&:name).map(&:to_s)
          info = MLC::Registries::FunctionSignature.new(
            func.name,
            ptypes,
            func.ret_type,
            func.type_params || [],
            req,
            pnames
          )
          @function_registry.register(
            func.name,
            info,
            module_name: module_name,
            exported: func.exported,
            external: func.external
          )
        end
      end
    end
  end
end

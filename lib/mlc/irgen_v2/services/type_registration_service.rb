# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # TypeRegistrationService - centralizes type registration concerns for v2
      class TypeRegistrationService
        def initialize(type_registry:, type_decl_table:, sum_type_constructor_service:)
          @type_registry = type_registry
          @type_decl_table = type_decl_table
          @sum_type_constructor_service = sum_type_constructor_service
        end

        def register(decl:, core_type:, namespace: nil, kind:, exported: false, module_name: nil)
          @type_registry.register(
            decl.name,
            ast_node: decl,
            core_ir_type: core_type,
            namespace: namespace,
            kind: kind,
            exported: exported,
            module_name: module_name
          )

          @type_decl_table[decl.name] = decl
          register_constructors_if_needed(decl.name, core_type)
        end

        private

        def register_constructors_if_needed(type_name, core_type)
          return unless core_type.is_a?(MLC::HighIR::SumType)

          @sum_type_constructor_service.register_sum_type_constructors(type_name, core_type)
        end
      end
    end
  end
end

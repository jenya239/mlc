# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # TypeDeclarationService - lowers AST type declarations and registers them
      class TypeDeclarationService
        def initialize(type_checker:, type_builder:, type_registration_service:, module_context_service:)
          @type_checker = type_checker
          @type_builder = type_builder
          @type_registration_service = type_registration_service
          @module_context_service = module_context_service
        end

        def build(decl)
          type_params = @type_checker.normalize_type_params(decl.type_params)

          core_type = nil

          @type_builder.with_type_params(type_params) do
            core_type = @type_builder.transform(decl.type)
          end

          core_type = specialize_named_type(core_type, decl.name)
          kind = @type_checker.infer_type_kind(decl, core_type)

          @type_registration_service.register(
            decl: decl,
            core_type: core_type,
            namespace: nil,
            kind: kind,
            exported: decl.exported,
            module_name: @module_context_service.current_module_name
          )

          MLC::SemanticIR::TypeDecl.new(name: decl.name, type: core_type, type_params: type_params, exported: decl.exported)
        end

        private

        def specialize_named_type(core_type, name)
          case core_type
          when MLC::SemanticIR::RecordType
            MLC::SemanticIR::Builder.record_type(name, core_type.fields)
          when MLC::SemanticIR::SumType
            MLC::SemanticIR::Builder.sum_type(name, core_type.variants)
          else
            core_type
          end
        end
      end
    end
  end
end

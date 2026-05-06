# frozen_string_literal: true

require_relative "derive_clause_validation"

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # TypeDeclarationService - lowers AST type declarations and registers them
          class TypeDeclarationService
            attr_accessor :expression_visitor

            def initialize(type_checker:, type_builder:, type_registration_service:, module_context_service:)
              @type_checker = type_checker
              @type_builder = type_builder
              @type_registration_service = type_registration_service
              @module_context_service = module_context_service
              @expression_visitor = nil
            end

            def build(decl)
              type_params = @type_checker.normalize_type_params(decl.type_params)

              core_type = nil

              @type_builder.with_type_params(type_params) do
                core_type = @type_builder.transform(decl.type)
              end

              core_type = specialize_named_type(core_type, decl.name)
              core_type = apply_record_field_defaults(decl, core_type)
              DeriveClauseValidation.validate!(decl, core_type)
              kind = @type_checker.infer_type_kind(decl, core_type)

              @type_registration_service.register(
                decl: decl,
                core_type: core_type,
                namespace: @module_context_service.current_module_namespace,
                kind: kind,
                exported: decl.exported,
                module_name: @module_context_service.current_module_name
              )

              MLC::SemanticIR::TypeDecl.new(name: decl.name, type: core_type, type_params: type_params, exported: decl.exported, derive_traits: Array(decl.derive_traits))
            end

            private

            def apply_record_field_defaults(decl, core_type)
              return core_type unless decl.type.is_a?(MLC::Source::AST::RecordType)
              return core_type unless decl.type.fields.any? { |field| field[:default] }

              if Array(decl.type_params).any?
                raise MLC::CompileError, "record field default values are not supported when the type has generic parameters"
              end

              unless @expression_visitor
                raise MLC::CompileError, "record field defaults require SemanticGen pipeline (missing expression visitor)"
              end

              ast_fields = decl.type.fields
              ir_fields = core_type.fields
              if ast_fields.length != ir_fields.length
                raise MLC::CompileError, "internal: record AST/IR field count mismatch for #{decl.name}"
              end

              ast_fields.zip(ir_fields).each do |ast_field, ir_field|
                next unless ast_field[:default]

                ir_expression = @expression_visitor.visit(ast_field[:default], {})
                @type_checker.ensure_type!(
                  ir_expression.type,
                  "default value expression for field '#{ast_field[:name]}' of '#{decl.name}' has unknown type",
                  node: decl
                )
                @type_checker.ensure_compatible_type(
                  ir_expression.type,
                  ir_field[:type],
                  "default value for field '#{ast_field[:name]}' of '#{decl.name}'",
                  node: decl
                )
                ir_field[:default] = ir_expression
              end

              core_type
            end

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
  end
end

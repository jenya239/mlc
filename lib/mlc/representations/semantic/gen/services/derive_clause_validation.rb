# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # Validates derive { … } clauses before codegen (E1 / B1).
          module DeriveClauseValidation
            ALLOWED_DERIVE_TRAITS = %w[Display Eq Ord Hash Json].freeze
            HASH_ALLOWED_PRIM_NAMES = %w[i32 bool string].freeze

            module_function

            def validate!(decl, core_type)
              traits = Array(decl.derive_traits)
              return if traits.empty?

              validate_known_traits!(traits, decl)
              validate_no_duplicate_traits!(traits, decl)

              if traits.include?("Hash")
                validate_hash_not_generic!(decl)
                validate_hash_field_types!(core_type, decl)
              end
              if traits.include?("Json")
                validate_json_not_generic!(decl)
                validate_json_field_types!(core_type, decl)
              end
            end

            def validate_known_traits!(traits, decl)
              traits.each do |trait|
                next if ALLOWED_DERIVE_TRAITS.include?(trait)

                raise MLC::CompileError.new("unknown derive trait \"#{trait}\"", origin: decl.origin)
              end
            end

            def validate_no_duplicate_traits!(traits, decl)
              seen = {}
              traits.each do |trait|
                if seen[trait]
                  raise MLC::CompileError.new("duplicate derive trait \"#{trait}\"", origin: decl.origin)
                end
                seen[trait] = true
              end
            end

            def validate_hash_not_generic!(decl)
              return if decl.type_params.nil? || decl.type_params.empty?

              raise MLC::CompileError.new(
                "derive Hash is not supported for generic types in this version",
                origin: decl.origin
              )
            end

            def validate_hash_field_types!(core_type, decl)
              case core_type
              when MLC::SemanticIR::RecordType
                core_type.fields.each do |field|
                  validate_one_hash_field!(field[:type], field[:name].to_s, "record", decl)
                end
              when MLC::SemanticIR::SumType
                core_type.variants.each do |variant|
                  vname = variant[:name].to_s
                  Array(variant[:fields]).each do |field|
                    label = field[:name] ? field[:name].to_s : "payload"
                    validate_one_hash_field!(field[:type], label, vname, decl)
                  end
                end
              end
            end

            def validate_json_not_generic!(decl)
              return if decl.type_params.nil? || decl.type_params.empty?

              raise MLC::CompileError.new(
                "derive Json is not supported for generic types in this version",
                origin: decl.origin
              )
            end

            def validate_json_field_types!(core_type, decl)
              case core_type
              when MLC::SemanticIR::RecordType
                core_type.fields.each do |field|
                  validate_one_json_field!(field[:type], field[:name].to_s, "record", decl)
                end
              when MLC::SemanticIR::SumType
                core_type.variants.each do |variant|
                  vname = variant[:name].to_s
                  Array(variant[:fields]).each do |field|
                    label = field[:name] ? field[:name].to_s : "payload"
                    validate_one_json_field!(field[:type], label, vname, decl)
                  end
                end
              end
            end

            def json_type_problem(semantic_type)
              case semantic_type
              when MLC::SemanticIR::MapType
                "Map"
              when MLC::SemanticIR::FunctionType
                "function"
              when MLC::SemanticIR::ArrayType
                json_type_problem(semantic_type.element_type)
              when MLC::SemanticIR::GenericType
                base = semantic_type.base_type.respond_to?(:name) ? semantic_type.base_type.name : nil
                if base == "Option"
                  json_type_problem(semantic_type.type_args.first)
                elsif base == "Shared" || base == "Map"
                  base
                end
              end
            end

            def validate_one_json_field!(semantic_type, label, variant_or_record, decl)
              problem = json_type_problem(semantic_type)
              return if problem.nil?

              raise MLC::CompileError.new(
                "derive Json: unsupported field type for \"#{label}\" in #{variant_or_record} (#{problem})",
                origin: decl.origin
              )
            end

            def validate_one_hash_field!(semantic_type, label, variant_or_record, decl)
              ok = semantic_type.is_a?(MLC::SemanticIR::Type) &&
                   semantic_type.kind == :prim &&
                   HASH_ALLOWED_PRIM_NAMES.include?(semantic_type.name)
              return if ok

              raise MLC::CompileError.new(
                "derive Hash: unsupported field type for \"#{label}\" in #{variant_or_record} " \
                "(allowed: i32, bool, string)",
                origin: decl.origin
              )
            end
          end
        end
      end
    end
  end
end

# frozen_string_literal: true

require 'set'

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # RecordLiteralBuilder - builds SemanticIR record literals using services
          class RecordLiteralBuilder
            def initialize(ir_builder:, type_registry:, type_checker:)
              @ir_builder = ir_builder
              @type_registry = type_registry
              @type_checker = type_checker
            end

            def build(node:, fields_ir:, expected_type: nil)
              normalized = normalize_fields(fields_ir)

              type_name = node.type_name.to_s

              # If anonymous record literal, try to infer type from context
              if (type_name.empty? || type_name == 'record') && expected_type
                inferred_name = extract_type_name(expected_type)
                type_name = inferred_name if inferred_name && !inferred_name.empty?
              end

              # For truly anonymous records (no expected type), generate structural type
              # This allows `{x: 1, y: 2}` to work in contexts like function return
              if type_name.empty? || type_name == 'record'
                type_name = "__anon_record_#{normalized.keys.sort.join('_')}"
              end

              # Build record type and get ordered fields
              record_type, ordered_fields =
                if (info = @type_registry.lookup(type_name))&.record?
                  build_named_record(type_name, info, normalized, node)
                else
                  [inferred_record_type(type_name, normalized, node), normalized]
                end

              @ir_builder.record_expr(
                type_name: type_name,
                fields: ordered_fields,
                type: record_type,
                origin: node
              )
            end

            private

            def normalize_fields(fields_ir)
              fields_ir.each_with_object({}) do |(name, expr), acc|
                acc[name.to_s] = expr
              end
            end

            # Returns [record_type, ordered_fields_ir]
            def build_named_record(type_name, type_info, fields_ir, node)
              expected_names = Array(type_info.fields).map { |field| field[:name].to_s }
              actual_names = fields_ir.keys

              # Compare as sets - order doesn't matter for spread expansion
              expected_set = expected_names.to_set
              actual_set = actual_names.to_set

              unless expected_names.empty? || expected_set == actual_set
                missing = (expected_set - actual_set).to_a
                extra = (actual_set - expected_set).to_a
                msg = "Record literal fields do not match type '#{type_name}'"
                msg += ". Missing: #{missing.join(', ')}" if missing.any?
                msg += ". Extra: #{extra.join(', ')}" if extra.any?
                @type_checker.type_error(msg, node: node)
              end

              Array(type_info.fields).each do |field|
                field_name = field[:name].to_s
                literal_expr = fields_ir[field_name]
                next unless literal_expr

                ensure_expr_type!(literal_expr, field_name, node)
                @type_checker.ensure_compatible_type(
                  literal_expr.type,
                  field[:type],
                  "field '#{field_name}' of '#{type_name}'",
                  node: node
                )
              end

              # Reorder fields to match expected order for correct C++ brace initialization
              ordered_fields_ir = expected_names.each_with_object({}) do |name, acc|
                acc[name] = fields_ir[name] if fields_ir.key?(name)
              end

              [inferred_record_type(type_name, ordered_fields_ir, node), ordered_fields_ir]
            end

            def inferred_record_type(type_name, fields_ir, node)
              field_types = fields_ir.map do |field_name, expr|
                ensure_expr_type!(expr, field_name, node)
                { name: field_name.to_s, type: expr.type }
              end
              @ir_builder.record_type(name: type_name, fields: field_types)
            end

            def ensure_expr_type!(expr, field_name, node)
              @type_checker.ensure_type!(
                expr.type,
                "field '#{field_name}' has unknown type",
                node: node
              )
            end

            def extract_type_name(expected_type)
              return nil unless expected_type

              case expected_type
              when MLC::SemanticIR::RecordType
                expected_type.name&.to_s
              when Hash
                expected_type[:name]&.to_s
              when String
                # Strip generic parameters: "Pair<T, U>" -> "Pair"
                expected_type.split('<').first
              else
                expected_type.respond_to?(:name) ? expected_type.name&.to_s : nil
              end
            end
          end
        end
      end
    end
  end
end

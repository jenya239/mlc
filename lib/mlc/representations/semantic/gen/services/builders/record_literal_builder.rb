# frozen_string_literal: true

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

            def build(node:, fields_ir:)
              normalized = normalize_fields(fields_ir)
              type_name = node.type_name.to_s.empty? ? 'record' : node.type_name

              record_type =
                if node.type_name && !node.type_name.to_s.empty? && (info = @type_registry.lookup(type_name))&.record?
                  build_named_record(type_name, info, normalized, node)
                else
                  inferred_record_type(type_name, normalized, node)
                end

              @ir_builder.record_expr(
                type_name: type_name,
                fields: normalized,
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

            def build_named_record(type_name, type_info, fields_ir, node)
              expected_names = Array(type_info.fields).map { |field| field[:name].to_s }
              actual_names = fields_ir.keys

              unless expected_names.empty? || expected_names == actual_names
                @type_checker.type_error(
                  "Record literal fields do not match type '#{type_name}'",
                  node: node
                )
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

              inferred_record_type(type_name, fields_ir, node)
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
          end
        end
      end
    end
  end
end

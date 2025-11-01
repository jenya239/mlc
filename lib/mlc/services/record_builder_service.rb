# frozen_string_literal: true

module MLC
  module Services
    # RecordBuilderService - сервис для построения record типов
    # Содержит всю логику построения records (перемещено из IRGen)
    #
    # Использование:
    #   record_builder = context[:record_builder]
    #   type = record_builder.build_named(type_name, fields)
    class RecordBuilderService
      def initialize(base_transformer)
        @transformer = base_transformer
      end

      # Вывести record тип из контекста (пока не используется)
      def infer_from_context(fields)
        nil
      end

      # Вывести record тип из registry
      # Ищет подходящий record тип по полям в type registry
      def infer_from_registry(fields)
        type_registry = @transformer.instance_variable_get(:@type_registry)
        type_decl_table = @transformer.instance_variable_get(:@type_decl_table)
        return nil unless type_registry

        candidates = type_registry.types.each_value.filter_map do |type_info|
          next unless type_info.record?

          type_decl = type_decl_table[type_info.name]
          construct_from_info(type_info.name, type_info, fields, type_decl)
        end

        select_best_candidate(candidates)
      end

      # Построить анонимный record (без type annotation)
      def build_anonymous(fields)
        inferred_fields = fields.map { |name, value| {name: name.to_s, type: value.type} }
        record_type = MLC::HighIR::Builder.record_type("record", inferred_fields)
        MLC::HighIR::Builder.record("record", fields, record_type)
      end

      # Построить именованный record (с явным типом)
      def build_named(type_name, fields)
        type_registry = @transformer.instance_variable_get(:@type_registry)
        type_decl_table = @transformer.instance_variable_get(:@type_decl_table)

        if type_registry && (type_info = type_registry.lookup(type_name))
          type_decl = type_decl_table[type_name]
          constructed = construct_from_info(type_name, type_info, fields, type_decl)
          return constructed[:record] if constructed

          base_type = type_info.core_ir_type
        end

        unless defined?(base_type) && base_type
          inferred_fields = fields.map { |name, value| {name: name.to_s, type: value.type} }
          base_type = MLC::HighIR::Builder.record_type(type_name, inferred_fields)
        end

        MLC::HighIR::Builder.record(type_name, fields, base_type)
      end

      # Construct record from type registry info
      # Validates fields match, unifies types for generics
      # Returns {record: RecordExpr, concreteness: Int} or nil
      def construct_from_info(type_name, type_info, fields, type_decl)
        record_fields = Array(type_info.fields)
        return nil if record_fields.empty?

        literal_field_names = fields.keys.map(&:to_s)
        info_field_names = record_fields.map { |field| field[:name].to_s }
        return nil unless literal_field_names == info_field_names

        type_map = {}

        record_fields.each do |field|
          field_name = field[:name].to_s
          literal_expr = fields[field_name] || fields[field[:name]]
          return nil unless literal_expr

          literal_type = literal_expr.type
          return nil unless literal_type

          # Use transformer's unify_type method
          matched = @transformer.send(:unify_type, field[:type], literal_type, type_map, context: "field '#{field_name}' of '#{type_name}'")
          return nil unless matched
        end

        type_params = Array(type_decl&.type_params)
        type_args = if type_params.any?
                      type_params.map do |tp|
                        inferred = type_map[tp.name]
                        return nil unless inferred
                        inferred
                      end
                    else
                      []
                    end

        record_type = if type_args.any?
                        MLC::HighIR::Builder.generic_type(type_info.core_ir_type, type_args)
                      else
                        type_info.core_ir_type
                      end

        record_expr = MLC::HighIR::Builder.record(type_name, fields, record_type)
        concreteness = type_args.count { |arg| !arg.is_a?(MLC::HighIR::TypeVariable) }

        {record: record_expr, concreteness: concreteness}
      end

      # Выбрать лучший candidate record (с наибольшей concreteness)
      def select_best_candidate(candidates)
        return nil if candidates.empty?
        best = candidates.max_by { |candidate| candidate[:concreteness] }
        best && best[:record]
      end
    end
  end
end

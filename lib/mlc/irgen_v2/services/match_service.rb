# frozen_string_literal: true

require_relative '../../type_system/match_analyzer'
require_relative '../../services/type_unification_service'

module MLC
  module IRGenV2
    module Services
      # MatchService - Handles match expression lowering and pattern binding
      class MatchService
        def initialize(ir_builder:, type_checker:, var_type_registry:, type_unification_service:, match_analyzer:)
          @ir_builder = ir_builder
          @type_checker = type_checker
          @var_type_registry = var_type_registry
          @type_unification_service = type_unification_service
          @match_analyzer = match_analyzer
        end

        def build_expression(node, expression_visitor:, statement_visitor:)
          scrutinee_ir = expression_visitor.visit(node.scrutinee)

          analysis = @match_analyzer.analyze(
            scrutinee_type: scrutinee_ir.type,
            arms: node.arms,
            transform_arm: ->(scrutinee_type, arm) do
              transform_expression_arm(scrutinee_type, arm, expression_visitor)
            end
          )

          @ir_builder.match_expr(scrutinee: scrutinee_ir, arms: analysis.arms, type: analysis.result_type, origin: node)
        end

        private

        def transform_expression_arm(scrutinee_type, arm, expression_visitor)
          snapshot = @var_type_registry.snapshot

          pattern_data = transform_pattern(arm[:pattern])
          bind_pattern_variables(pattern_data, scrutinee_type)

          guard_ir = arm[:guard] ? expression_visitor.visit(arm[:guard]) : nil
          body_ir = expression_visitor.visit(arm[:body])

          { pattern: pattern_data, guard: guard_ir, body: body_ir }
        ensure
          @var_type_registry.restore(snapshot)
        end

        def transform_pattern(pattern_ast)
          data = pattern_ast.data || {}

          case pattern_ast.kind
          when :wildcard
            { kind: :wildcard }
          when :literal
            { kind: :literal, value: data[:value] }
          when :constructor
            {
              kind: :constructor,
              name: data[:name],
              fields: Array(data[:fields])
            }
          when :var
            { kind: :var, name: data[:name] }
          when :regex
            {
              kind: :regex,
              pattern: data[:pattern],
              flags: data[:flags],
              bindings: Array(data[:bindings])
            }
          else
            raise MLC::CompileError, "Unknown pattern kind: #{pattern_ast.kind}"
          end
        end

        def bind_pattern_variables(pattern, scrutinee_type)
          case pattern[:kind]
          when :constructor
            bind_constructor_pattern(pattern, scrutinee_type)
          when :var
            name = pattern[:name]
            bind_variable(name, scrutinee_type)
          when :regex
            Array(pattern[:bindings]).each do |name|
              bind_variable(name, string_type) unless ignored_binding?(name)
            end
          end
        end

        def bind_constructor_pattern(pattern, scrutinee_type)
          info = @type_unification_service.constructor_info_for(pattern[:name], scrutinee_type)
          field_types = info ? info.param_types : []

          Array(pattern[:fields]).each_with_index do |field, index|
            next if ignored_binding?(field)

            field_type = field_types[index] || unknown_type
            bind_variable(field, field_type)
          end
        end

        def bind_variable(name, type)
          return if ignored_binding?(name)
          ensure_type!(type, "pattern binding '#{name}'")
          @var_type_registry.set(name, type)
        end

        def ignored_binding?(name)
          name.nil? || name == '_'
        end

        def ensure_type!(type, context)
          @type_checker.ensure_type!(type, "#{context} has unknown type")
        end

        def string_type
          @string_type ||= @ir_builder.prim_type(name: 'string')
        end

        def unknown_type
          @unknown_type ||= @ir_builder.prim_type(name: 'auto')
        end
      end
    end
  end
end

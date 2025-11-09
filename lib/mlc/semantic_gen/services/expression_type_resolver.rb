# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # ExpressionTypeResolver - infers result types for unary/binary operations
      class ExpressionTypeResolver
        NUMERIC_PRECEDENCE = {
          'f64' => 6,
          'f32' => 5,
          'i64' => 4,
          'u64' => 3,
          'i32' => 2,
          'u32' => 1
        }.freeze

        def initialize(type_checker:, ir_builder:)
          @type_checker = type_checker
          @ir_builder = ir_builder
        end

        def unary_type(op, operand_type, node: nil)
          case op
          when '!'
            @type_checker.ensure_boolean_type(operand_type, "operand of '!'", node: node)
            bool_type
          when '-', '+'
            @type_checker.ensure_numeric_type(operand_type, "operand of '#{op}'", node: node)
            operand_type
          else
            operand_type
          end
        end

        def binary_type(op, left_type, right_type, node: nil)
          case op
          when '+'
            if string_type?(left_type) && string_type?(right_type)
              return string_type
            end
            ensure_numeric_pair(op, left_type, right_type, node)
            combine_numeric(left_type, right_type)
          when '-', '*', '%'
            ensure_numeric_pair(op, left_type, right_type, node)
            combine_numeric(left_type, right_type)
          when '/'
            ensure_numeric_pair(op, left_type, right_type, node)
            # Division always returns float; use combine_numeric to preserve exact float type
            return combine_numeric(left_type, right_type) if float_type?(left_type) || float_type?(right_type)
            # Integer division defaults to f32
            float_type('f32')
          when '==', '!='
            @type_checker.ensure_compatible_type(left_type, right_type, "comparison '#{op}'", node: node)
            bool_type
          when '<', '>', '<=', '>='
            ensure_numeric_pair(op, left_type, right_type, node)
            bool_type
          when '&&', '||'
            @type_checker.ensure_boolean_type(left_type, "left operand of '#{op}'", node: node)
            @type_checker.ensure_boolean_type(right_type, "right operand of '#{op}'", node: node)
            bool_type
          else
            left_type || right_type
          end
        end

        private

        def ensure_numeric_pair(op, left, right, node)
          @type_checker.ensure_numeric_type(left, "left operand of '#{op}'", node: node)
          @type_checker.ensure_numeric_type(right, "right operand of '#{op}'", node: node)
        end

        def combine_numeric(left, right)
          left_name = type_name(left)
          right_name = type_name(right)
          if float_type?(left) || float_type?(right)
            return float_type(float_type?(left) ? left_name : right_name)
          end

          left_priority = NUMERIC_PRECEDENCE[left_name] || 0
          right_priority = NUMERIC_PRECEDENCE[right_name] || 0
          if left_priority >= right_priority
            left
          else
            right
          end
        end

        def type_name(type)
          @type_checker.type_name(type)
        end

        def string_type?(type)
          @type_checker.normalized_type_name(type_name(type)) == 'string'
        end

        def string_type
          @ir_builder.prim_type(name: 'string')
        end

        def bool_type
          @ir_builder.prim_type(name: 'bool')
        end

        def float_type?(type)
          %w[f32 f64].include?(@type_checker.normalized_type_name(type_name(type)))
        end

        def float_type(name)
          @ir_builder.prim_type(name: name)
        end
      end
    end
  end
end

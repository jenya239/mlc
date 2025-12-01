# frozen_string_literal: true

require 'test_helper'
require 'mlc/representations/semantic/gen/passes/constant_folding_pass'
require 'mlc/representations/semantic/nodes/nodes'

module MLC
  module Representations
    module Semantic
      module Gen
        module Passes
          class ConstantFoldingPassTest < Minitest::Test
            def setup
              @pass = ConstantFoldingPass.new
            end

            # Helper to create a literal expression
            def lit(value, type_name = nil)
              type_name ||= case value
                            when Integer then 'i32'
                            when Float then 'f32'
                            when TrueClass, FalseClass then 'bool'
                            else 'unknown'
                            end
              MLC::SemanticIR::LiteralExpr.new(
                value: value,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: type_name)
              )
            end

            def var(name, type_name = 'i32')
              MLC::SemanticIR::VarExpr.new(
                name: name,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: type_name)
              )
            end

            def binary(op, left, right, type_name = nil)
              type_name ||= case op
                            when '+', '-', '*', '/', '%' then 'i32'
                            when '==', '!=', '<', '>', '<=', '>=' then 'bool'
                            when '&&', '||' then 'bool'
                            else 'i32'
                            end
              MLC::SemanticIR::BinaryExpr.new(
                op: op,
                left: left,
                right: right,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: type_name)
              )
            end

            def unary(op, operand, type_name = nil)
              type_name ||= case op
                            when '!' then 'bool'
                            else 'i32'
                            end
              MLC::SemanticIR::UnaryExpr.new(
                op: op,
                operand: operand,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: type_name)
              )
            end

            def func(name, body)
              MLC::SemanticIR::Func.new(
                name: name,
                params: [],
                ret_type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32'),
                body: body
              )
            end

            def program(functions)
              Struct.new(:functions, :types, :imports, :origin).new(functions, [], [], nil)
            end

            # ==================== Arithmetic Tests ====================

            def test_fold_addition_of_integers
              # 1 + 2 -> 3
              expr = binary('+', lit(1), lit(2))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 3, result.value
            end

            def test_fold_subtraction_of_integers
              # 10 - 3 -> 7
              expr = binary('-', lit(10), lit(3))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 7, result.value
            end

            def test_fold_multiplication_of_integers
              # 4 * 5 -> 20
              expr = binary('*', lit(4), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 20, result.value
            end

            def test_fold_division_of_integers
              # 20 / 4 -> 5
              expr = binary('/', lit(20), lit(4))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 5, result.value
            end

            def test_fold_modulo_of_integers
              # 17 % 5 -> 2
              expr = binary('%', lit(17), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 2, result.value
            end

            def test_no_fold_division_by_zero
              # 10 / 0 -> no folding (avoid runtime error)
              expr = binary('/', lit(10), lit(0))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::BinaryExpr, result
            end

            def test_no_fold_modulo_by_zero
              # 10 % 0 -> no folding
              expr = binary('%', lit(10), lit(0))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::BinaryExpr, result
            end

            # ==================== Float Arithmetic Tests ====================

            def test_fold_addition_of_floats
              # 1.5 + 2.5 -> 4.0
              expr = binary('+', lit(1.5), lit(2.5), 'f32')
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_in_delta 4.0, result.value, 0.0001
            end

            def test_fold_multiplication_of_floats
              # 2.0 * 3.5 -> 7.0
              expr = binary('*', lit(2.0), lit(3.5), 'f32')
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_in_delta 7.0, result.value, 0.0001
            end

            # ==================== Comparison Tests ====================

            def test_fold_equality_true
              # 5 == 5 -> true
              expr = binary('==', lit(5), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_equality_false
              # 5 == 6 -> false
              expr = binary('==', lit(5), lit(6))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal false, result.value
            end

            def test_fold_not_equal
              # 5 != 6 -> true
              expr = binary('!=', lit(5), lit(6))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_less_than
              # 3 < 5 -> true
              expr = binary('<', lit(3), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_greater_than
              # 7 > 5 -> true
              expr = binary('>', lit(7), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_less_or_equal
              # 5 <= 5 -> true
              expr = binary('<=', lit(5), lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_greater_or_equal
              # 5 >= 6 -> false
              expr = binary('>=', lit(5), lit(6))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal false, result.value
            end

            # ==================== Boolean Logic Tests ====================

            def test_fold_logical_and_true
              # true && true -> true
              expr = binary('&&', lit(true), lit(true))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_logical_and_false
              # true && false -> false
              expr = binary('&&', lit(true), lit(false))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal false, result.value
            end

            def test_fold_logical_or_true
              # false || true -> true
              expr = binary('||', lit(false), lit(true))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            def test_fold_logical_or_false
              # false || false -> false
              expr = binary('||', lit(false), lit(false))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal false, result.value
            end

            # ==================== Unary Tests ====================

            def test_fold_unary_negation
              # -5 -> -5
              expr = unary('-', lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal(-5, result.value)
            end

            def test_fold_unary_plus
              # +5 -> 5
              expr = unary('+', lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 5, result.value
            end

            def test_fold_unary_not_true
              # !true -> false
              expr = unary('!', lit(true))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal false, result.value
            end

            def test_fold_unary_not_false
              # !false -> true
              expr = unary('!', lit(false))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal true, result.value
            end

            # ==================== Bitwise Tests ====================

            def test_fold_bitwise_and
              # 0b1100 & 0b1010 -> 0b1000
              expr = binary('&', lit(12), lit(10))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 8, result.value
            end

            def test_fold_bitwise_or
              # 0b1100 | 0b1010 -> 0b1110
              expr = binary('|', lit(12), lit(10))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 14, result.value
            end

            def test_fold_bitwise_xor
              # 0b1100 ^ 0b1010 -> 0b0110
              expr = binary('^', lit(12), lit(10))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 6, result.value
            end

            def test_fold_left_shift
              # 1 << 4 -> 16
              expr = binary('<<', lit(1), lit(4))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 16, result.value
            end

            def test_fold_right_shift
              # 16 >> 2 -> 4
              expr = binary('>>', lit(16), lit(2))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 4, result.value
            end

            def test_fold_bitwise_not
              # ~0 -> -1 (in Ruby)
              expr = unary('~', lit(0))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal(-1, result.value)
            end

            # ==================== Nested Expression Tests ====================

            def test_fold_nested_arithmetic
              # (1 + 2) * 3 -> 9
              inner = binary('+', lit(1), lit(2))
              expr = binary('*', inner, lit(3))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 9, result.value
            end

            def test_fold_deeply_nested
              # ((2 + 3) * 4) - 5 -> 15
              sum = binary('+', lit(2), lit(3))
              product = binary('*', sum, lit(4))
              expr = binary('-', product, lit(5))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 15, result.value
            end

            # ==================== No-Fold Cases ====================

            def test_no_fold_variable_left
              # x + 1 -> no folding
              expr = binary('+', var('x'), lit(1))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::BinaryExpr, result
            end

            def test_no_fold_variable_right
              # 1 + x -> no folding
              expr = binary('+', lit(1), var('x'))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::BinaryExpr, result
            end

            def test_partial_fold_with_variable
              # (1 + 2) + x -> 3 + x
              inner = binary('+', lit(1), lit(2))
              expr = binary('+', inner, var('x'))
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::BinaryExpr, result
              assert_instance_of MLC::SemanticIR::LiteralExpr, result.left
              assert_equal 3, result.left.value
              assert_instance_of MLC::SemanticIR::VarExpr, result.right
            end

            # ==================== If Expression Tests ====================

            def test_fold_if_true_condition
              # if true then 1 else 2 -> 1
              then_branch = lit(1)
              else_branch = lit(2)
              expr = MLC::SemanticIR::IfExpr.new(
                condition: lit(true),
                then_branch: then_branch,
                else_branch: else_branch,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32')
              )
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 1, result.value
            end

            def test_fold_if_false_condition
              # if false then 1 else 2 -> 2
              then_branch = lit(1)
              else_branch = lit(2)
              expr = MLC::SemanticIR::IfExpr.new(
                condition: lit(false),
                then_branch: then_branch,
                else_branch: else_branch,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32')
              )
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::LiteralExpr, result
              assert_equal 2, result.value
            end

            def test_no_fold_if_variable_condition
              # if x then 1 else 2 -> no folding
              then_branch = lit(1)
              else_branch = lit(2)
              expr = MLC::SemanticIR::IfExpr.new(
                condition: var('x', 'bool'),
                then_branch: then_branch,
                else_branch: else_branch,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32')
              )
              result = @pass.send(:fold_expr, expr)

              assert_instance_of MLC::SemanticIR::IfExpr, result
            end

            # ==================== Statistics Tests ====================

            def test_stats_increment_on_fold
              @pass.send(:fold_expr, binary('+', lit(1), lit(2)))
              assert_equal 1, @pass.stats[:folded]
            end

            def test_stats_visited_count
              @pass.send(:fold_expr, binary('+', lit(1), lit(2)))
              # Visits: BinaryExpr, LiteralExpr(1), LiteralExpr(2) = 3
              assert @pass.stats[:visited] >= 1
            end

            # ==================== Program Level Tests ====================

            def test_run_on_program
              body = binary('+', lit(10), lit(20))
              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              assert_instance_of MLC::SemanticIR::LiteralExpr, result_body
              assert_equal 30, result_body.value

              assert context[:constant_folding_stats]
              assert context[:constant_folding_stats][:folded] >= 1
            end

            def test_required_keys
              assert_includes @pass.required_keys, :semantic_ir
            end

            def test_produced_keys
              assert_includes @pass.produced_keys, :semantic_ir
              assert_includes @pass.produced_keys, :constant_folding_stats
            end

            def test_input_output_levels
              assert_equal :semantic_ir, @pass.input_level
              assert_equal :semantic_ir, @pass.output_level
            end
          end
        end
      end
    end
  end
end

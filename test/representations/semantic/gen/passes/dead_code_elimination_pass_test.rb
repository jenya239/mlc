# frozen_string_literal: true

require 'test_helper'
require 'mlc/representations/semantic/gen/passes/dead_code_elimination_pass'
require 'mlc/representations/semantic/nodes/nodes'

module MLC
  module Representations
    module Semantic
      module Gen
        module Passes
          class DeadCodeEliminationPassTest < Minitest::Test
            def setup
              @pass = DeadCodeEliminationPass.new
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

            def binary(op, left, right, type_name = 'i32')
              MLC::SemanticIR::BinaryExpr.new(
                op: op,
                left: left,
                right: right,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: type_name)
              )
            end

            def var_decl(name, value, mutable: false)
              MLC::SemanticIR::VariableDeclStmt.new(
                name: name,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32'),
                value: value,
                mutable: mutable
              )
            end

            def expr_stmt(expr)
              MLC::SemanticIR::ExprStatement.new(expression: expr)
            end

            def return_stmt(value = nil)
              MLC::SemanticIR::Return.new(expr: value)
            end

            def block(statements, result = nil)
              MLC::SemanticIR::BlockExpr.new(
                statements: statements,
                result: result,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32')
              )
            end

            def call(name, args = [])
              MLC::SemanticIR::CallExpr.new(
                callee: var(name),
                args: args,
                type: MLC::SemanticIR::Type.new(kind: :prim, name: 'i32')
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

            # ==================== Dead Code After Return Tests ====================

            def test_removes_code_after_return
              # { return 1; let x = 2; x }
              body = block([
                             return_stmt(lit(1)),
                var_decl('x', lit(2))
                           ], var('x'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Should only have the return statement
              assert_equal 1, result_body.statements.size
              assert_instance_of MLC::SemanticIR::Return, result_body.statements.first
              assert_nil result_body.result # Result eliminated because of return
            end

            def test_removes_multiple_statements_after_return
              # { return 1; let x = 2; let y = 3; }
              body = block([
                             return_stmt(lit(1)),
                var_decl('x', lit(2)),
                var_decl('y', lit(3))
                           ], nil)

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              assert_equal 1, result_body.statements.size
              assert_instance_of MLC::SemanticIR::Return, result_body.statements.first
            end

            def test_keeps_code_before_return
              # { let x = 1; return x; }
              body = block([
                             var_decl('x', lit(1)),
                return_stmt(var('x'))
                           ], nil)

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              assert_equal 2, result_body.statements.size
              assert_instance_of MLC::SemanticIR::VariableDeclStmt, result_body.statements[0]
              assert_instance_of MLC::SemanticIR::Return, result_body.statements[1]
            end

            # ==================== Unused Variable Tests ====================

            def test_removes_unused_variable_without_side_effects
              # { let x = 1; 2 }
              body = block([
                             var_decl('x', lit(1))
                           ], lit(2))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Unused variable should be eliminated
              assert_equal 0, result_body.statements.size
              assert_equal 2, result_body.result.value
            end

            def test_keeps_used_variable
              # { let x = 1; x }
              body = block([
                             var_decl('x', lit(1))
                           ], var('x'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Used variable should be kept
              assert_equal 1, result_body.statements.size
              assert_equal 'x', result_body.statements[0].name
            end

            def test_keeps_variable_used_in_expression
              # { let x = 1; let y = x + 1; y }
              body = block([
                             var_decl('x', lit(1)),
                var_decl('y', binary('+', var('x'), lit(1)))
                           ], var('y'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Both variables should be kept
              assert_equal 2, result_body.statements.size
            end

            def test_keeps_variable_with_side_effect_value
              # { let x = call(); 1 } - call may have side effects
              body = block([
                             var_decl('x', call('side_effect_func'))
                           ], lit(1))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Variable with side-effect call should be kept
              assert_equal 1, result_body.statements.size
            end

            # ==================== Side Effect Tests ====================

            def test_keeps_expression_statement_with_call
              # { call(); 1 }
              body = block([
                             expr_stmt(call('do_something'))
                           ], lit(1))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Call statement should be kept (has side effects)
              assert_equal 1, result_body.statements.size
            end

            def test_removes_expression_statement_without_side_effects
              # { 1 + 2; 3 }
              body = block([
                             expr_stmt(binary('+', lit(1), lit(2)))
                           ], lit(3))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Pure expression statement should be eliminated
              assert_equal 0, result_body.statements.size
            end

            # ==================== Statistics Tests ====================

            def test_stats_increment_on_elimination
              body = block([
                             return_stmt(lit(1)),
                var_decl('x', lit(2))
                           ], var('x'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              # At least one elimination should have occurred
              assert @pass.stats[:eliminated] >= 1
            end

            def test_stats_visited_count
              body = block([
                             var_decl('x', lit(1))
                           ], var('x'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              assert @pass.stats[:visited] >= 1
            end

            # ==================== Metadata Tests ====================

            def test_required_keys
              assert_includes @pass.required_keys, :semantic_ir
            end

            def test_produced_keys
              assert_includes @pass.produced_keys, :semantic_ir
              assert_includes @pass.produced_keys, :dead_code_elimination_stats
            end

            def test_input_output_levels
              assert_equal :semantic_ir, @pass.input_level
              assert_equal :semantic_ir, @pass.output_level
            end

            def test_pass_name
              assert_equal 'DeadCodeElimination', @pass.name
            end

            # ==================== Combined Scenarios ====================

            def test_complex_elimination_scenario
              # { let a = 1; let b = 2; return a; let c = 3; c }
              # Should keep: a, return a
              # Should eliminate: b (unused), c (after return)
              body = block([
                             var_decl('a', lit(1)),
                var_decl('b', lit(2)),
                return_stmt(var('a')),
                var_decl('c', lit(3))
                           ], var('c'))

              f = func('test', body)
              prog = program([f])

              context = { semantic_ir: prog }
              @pass.run(context)

              result_prog = context[:semantic_ir]
              result_body = result_prog.functions.first.body

              # Should have: let a, return a
              assert_equal 2, result_body.statements.size
              assert_equal 'a', result_body.statements[0].name
              assert_instance_of MLC::SemanticIR::Return, result_body.statements[1]
              assert_nil result_body.result
            end
          end
        end
      end
    end
  end
end

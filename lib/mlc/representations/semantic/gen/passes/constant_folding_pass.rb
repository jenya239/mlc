# frozen_string_literal: true

require_relative '../../../../common/analysis/base_pass'

module MLC
  module Representations
    module Semantic
      module Gen
        module Passes
          # ConstantFoldingPass - Evaluates constant expressions at compile time
          #
          # Optimizations performed:
          # - Binary operations on literals: 1 + 2 -> 3
          # - Unary operations on literals: -5 -> -5 (as literal)
          # - Boolean operations: true && false -> false
          # - Comparison operations: 3 > 2 -> true
          #
          # Safety: Only folds pure expressions (no side effects)
          class ConstantFoldingPass < MLC::Common::Analysis::BasePass
            ARITHMETIC_OPS = %w[+ - * / %].freeze
            COMPARISON_OPS = %w[== != < > <= >=].freeze
            BOOLEAN_OPS = %w[&& ||].freeze
            BITWISE_OPS = %w[& | ^ << >>].freeze

            def initialize(ir_builder: nil)
              super(name: 'ConstantFolding')
              @ir_builder = ir_builder
              @stats = { folded: 0, visited: 0 }
            end

            attr_reader :stats

            def input_level
              :semantic_ir
            end

            def output_level
              :semantic_ir
            end

            def required_keys
              [:semantic_ir]
            end

            def produced_keys
              [:semantic_ir, :constant_folding_stats]
            end

            def run(context)
              validate_context!(context)
              @ir_builder ||= context[:ir_builder]
              program = context[:semantic_ir]

              optimized_program = fold_program(program)
              context[:semantic_ir] = optimized_program
              context[:constant_folding_stats] = @stats.dup
            end

            private

            def fold_program(program)
              return program unless program.respond_to?(:functions)

              optimized_functions = program.functions.map { |func| fold_function(func) }

              # Rebuild program with optimized functions
              if program.respond_to?(:with_functions)
                program.with_functions(optimized_functions)
              else
                # Fallback: create new program-like structure
                program.class.new(
                  functions: optimized_functions,
                  types: program.respond_to?(:types) ? program.types : [],
                  imports: program.respond_to?(:imports) ? program.imports : [],
                  origin: program.respond_to?(:origin) ? program.origin : nil
                )
              end
            end

            def fold_function(func)
              return func unless func.respond_to?(:body) && func.body

              optimized_body = fold_expr(func.body)

              MLC::SemanticIR::Func.new(
                name: func.name,
                params: func.params,
                ret_type: func.ret_type,
                body: optimized_body,
                effects: func.respond_to?(:effects) ? func.effects : [],
                type_params: func.respond_to?(:type_params) ? func.type_params : [],
                external: func.respond_to?(:external) ? func.external : false,
                exported: func.respond_to?(:exported) ? func.exported : false,
                is_async: func.respond_to?(:is_async) ? func.is_async : false,
                origin: func.origin
              )
            end

            def fold_expr(expr)
              @stats[:visited] += 1

              case expr
              when MLC::SemanticIR::BinaryExpr
                fold_binary(expr)
              when MLC::SemanticIR::UnaryExpr
                fold_unary(expr)
              when MLC::SemanticIR::IfExpr
                fold_if(expr)
              when MLC::SemanticIR::BlockExpr
                fold_block(expr)
              when MLC::SemanticIR::CallExpr
                fold_call(expr)
              when MLC::SemanticIR::LiteralExpr, MLC::SemanticIR::VarExpr
                expr
              else
                expr
              end
            end

            def fold_binary(expr)
              left = fold_expr(expr.left)
              right = fold_expr(expr.right)

              # Only fold if both operands are literals
              if foldable_literal?(left) && foldable_literal?(right)
                result = evaluate_binary(expr.op, left.value, right.value, left.type, right.type)
                if result != :cannot_fold
                  @stats[:folded] += 1
                  return make_literal(result, expr.type, expr.origin)
                end
              end

              # Return potentially optimized expr
              if left.equal?(expr.left) && right.equal?(expr.right)
                expr
              else
                MLC::SemanticIR::BinaryExpr.new(
                  op: expr.op,
                  left: left,
                  right: right,
                  type: expr.type,
                  origin: expr.origin
                )
              end
            end

            def fold_unary(expr)
              operand = fold_expr(expr.operand)

              if foldable_literal?(operand)
                result = evaluate_unary(expr.op, operand.value, operand.type)
                if result != :cannot_fold
                  @stats[:folded] += 1
                  return make_literal(result, expr.type, expr.origin)
                end
              end

              if operand.equal?(expr.operand)
                expr
              else
                MLC::SemanticIR::UnaryExpr.new(
                  op: expr.op,
                  operand: operand,
                  type: expr.type,
                  origin: expr.origin
                )
              end
            end

            def fold_if(expr)
              cond = fold_expr(expr.condition)

              # If condition is constant, eliminate branch
              if foldable_literal?(cond) && boolean_value?(cond.value)
                @stats[:folded] += 1
                if truthy?(cond.value)
                  return fold_expr(expr.then_branch)
                elsif expr.else_branch
                  return fold_expr(expr.else_branch)
                end
              end

              then_branch = fold_expr(expr.then_branch)
              else_branch = expr.else_branch ? fold_expr(expr.else_branch) : nil

              if cond.equal?(expr.condition) && then_branch.equal?(expr.then_branch) &&
                 (else_branch.nil? || else_branch.equal?(expr.else_branch))
                expr
              else
                MLC::SemanticIR::IfExpr.new(
                  condition: cond,
                  then_branch: then_branch,
                  else_branch: else_branch,
                  type: expr.type,
                  origin: expr.origin
                )
              end
            end

            def fold_block(expr)
              optimized_stmts = expr.statements.map { |stmt| fold_statement(stmt) }
              optimized_result = expr.result ? fold_expr(expr.result) : nil

              MLC::SemanticIR::BlockExpr.new(
                statements: optimized_stmts,
                result: optimized_result,
                type: expr.type,
                origin: expr.origin
              )
            end

            def fold_call(expr)
              optimized_args = expr.args.map { |arg| fold_expr(arg) }
              optimized_callee = fold_expr(expr.callee)

              MLC::SemanticIR::CallExpr.new(
                callee: optimized_callee,
                args: optimized_args,
                type: expr.type,
                origin: expr.origin
              )
            end

            def fold_statement(stmt)
              case stmt
              when MLC::SemanticIR::VariableDeclStmt
                MLC::SemanticIR::VariableDeclStmt.new(
                  name: stmt.name,
                  type: stmt.type,
                  value: fold_expr(stmt.value),
                  mutable: stmt.mutable,
                  constant: stmt.constant,
                  origin: stmt.origin
                )
              when MLC::SemanticIR::ExprStatement
                MLC::SemanticIR::ExprStatement.new(
                  expression: fold_expr(stmt.expression),
                  origin: stmt.origin
                )
              when MLC::SemanticIR::Return
                MLC::SemanticIR::Return.new(
                  value: stmt.value ? fold_expr(stmt.value) : nil,
                  origin: stmt.origin
                )
              else
                stmt
              end
            end

            def foldable_literal?(expr)
              expr.is_a?(MLC::SemanticIR::LiteralExpr) && numeric_or_boolean?(expr.value)
            end

            def numeric_or_boolean?(value)
              value.is_a?(Integer) || value.is_a?(Float) || value == true || value == false
            end

            def boolean_value?(value)
              [true, false].include?(value)
            end

            def truthy?(value)
              value == true
            end

            def evaluate_binary(op, left, right, _left_type, _right_type) # rubocop:disable Naming/MethodParameterName
              case op
              when '+'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left + right
              when '-'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left - right
              when '*'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left * right
              when '/'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)
                return :cannot_fold if right.zero?

                left.is_a?(Integer) && right.is_a?(Integer) ? left / right : left.to_f / right
              when '%'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)
                return :cannot_fold if right.zero?

                left % right
              when '=='
                left == right
              when '!='
                left != right
              when '<'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left < right
              when '>'
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left > right
              when '<='
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left <= right
              when '>='
                return :cannot_fold unless left.is_a?(Numeric) && right.is_a?(Numeric)

                left >= right
              when '&&'
                return :cannot_fold unless boolean_value?(left) && boolean_value?(right)

                left && right
              when '||'
                return :cannot_fold unless boolean_value?(left) && boolean_value?(right)

                left || right
              when '&'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)

                left & right
              when '|'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)

                left | right
              when '^'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)

                left ^ right
              when '<<'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)

                left << right
              when '>>'
                return :cannot_fold unless left.is_a?(Integer) && right.is_a?(Integer)

                left >> right
              else
                :cannot_fold
              end
            end

            def evaluate_unary(op, operand, _type) # rubocop:disable Naming/MethodParameterName
              case op
              when '-'
                return :cannot_fold unless operand.is_a?(Numeric)

                -operand
              when '+'
                return :cannot_fold unless operand.is_a?(Numeric)

                operand
              when '!'
                return :cannot_fold unless boolean_value?(operand)

                !operand
              when '~'
                return :cannot_fold unless operand.is_a?(Integer)

                ~operand
              else
                :cannot_fold
              end
            end

            def make_literal(value, type, origin)
              MLC::SemanticIR::LiteralExpr.new(
                value: value,
                type: type,
                origin: origin
              )
            end
          end
        end
      end
    end
  end
end

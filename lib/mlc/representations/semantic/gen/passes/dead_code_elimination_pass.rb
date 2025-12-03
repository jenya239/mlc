# frozen_string_literal: true

require_relative '../../../../common/analysis/base_pass'

module MLC
  module Representations
    module Semantic
      module Gen
        module Passes
          # DeadCodeEliminationPass - Removes unreachable and unused code
          #
          # Optimizations performed:
          # - Remove statements after unconditional return
          # - Remove unused variable declarations
          # - Remove empty blocks
          # - Simplify if expressions with constant conditions (already in constant folding)
          #
          # Safety: Only removes provably dead code
          class DeadCodeEliminationPass < MLC::Common::Analysis::BasePass
            def initialize
              super(name: 'DeadCodeElimination')
              @stats = { eliminated: 0, visited: 0 }
              @used_vars = Set.new
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
              [:semantic_ir, :dead_code_elimination_stats]
            end

            def run(context)
              validate_context!(context)
              program = context[:semantic_ir]

              optimized_program = eliminate_program(program)
              context[:semantic_ir] = optimized_program
              context[:dead_code_elimination_stats] = @stats.dup
            end

            private

            def eliminate_program(program)
              return program unless program.respond_to?(:functions)

              optimized_functions = program.functions.map { |func| eliminate_function(func) }

              if program.respond_to?(:with_functions)
                program.with_functions(optimized_functions)
              else
                program.class.new(
                  functions: optimized_functions,
                  types: program.respond_to?(:types) ? program.types : [],
                  imports: program.respond_to?(:imports) ? program.imports : [],
                  origin: program.respond_to?(:origin) ? program.origin : nil
                )
              end
            end

            def eliminate_function(func)
              return func unless func.respond_to?(:body) && func.body

              # First pass: collect used variables
              @used_vars = Set.new
              collect_used_vars(func.body)

              # Second pass: eliminate dead code
              optimized_body = eliminate_expr(func.body)

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

            def collect_used_vars(expr)
              case expr
              when MLC::SemanticIR::VarExpr
                @used_vars << expr.name
              when MLC::SemanticIR::BinaryExpr
                collect_used_vars(expr.left)
                collect_used_vars(expr.right)
              when MLC::SemanticIR::UnaryExpr
                collect_used_vars(expr.operand)
              when MLC::SemanticIR::CallExpr
                collect_used_vars(expr.callee)
                expr.args.each { |arg| collect_used_vars(arg) }
              when MLC::SemanticIR::MemberExpr
                collect_used_vars(expr.object)
              when MLC::SemanticIR::IfExpr
                collect_used_vars(expr.condition)
                collect_used_vars(expr.then_branch)
                collect_used_vars(expr.else_branch) if expr.else_branch
              when MLC::SemanticIR::BlockExpr
                expr.statements.each { |stmt| collect_used_vars_stmt(stmt) }
                collect_used_vars(expr.result) if expr.result
              when MLC::SemanticIR::RecordExpr
                expr.fields.each_value { |field| collect_used_vars(field) }
              when MLC::SemanticIR::ArrayLiteralExpr
                expr.elements.each { |elem| collect_used_vars(elem) }
              when MLC::SemanticIR::LambdaExpr
                collect_used_vars(expr.body)
              when MLC::SemanticIR::MatchExpr
                collect_used_vars(expr.scrutinee)
                expr.arms.each { |arm| collect_used_vars(arm[:body]) }
              end
            end

            def collect_used_vars_stmt(stmt)
              case stmt
              when MLC::SemanticIR::VariableDeclStmt
                collect_used_vars(stmt.value)
              when MLC::SemanticIR::ExprStatement
                collect_used_vars(stmt.expression)
              when MLC::SemanticIR::Return
                collect_used_vars(stmt.expr) if stmt.expr
              when MLC::SemanticIR::AssignmentStmt
                @used_vars << stmt.target if stmt.target.is_a?(String)
                collect_used_vars(stmt.target) if stmt.target.respond_to?(:name)
                collect_used_vars(stmt.value)
              when MLC::SemanticIR::IfStmt
                collect_used_vars(stmt.condition)
                collect_used_vars_stmt(stmt.then_branch)
                collect_used_vars_stmt(stmt.else_branch) if stmt.else_branch
              when MLC::SemanticIR::WhileStmt
                collect_used_vars(stmt.condition)
                stmt.body.each { |s| collect_used_vars_stmt(s) }
              when MLC::SemanticIR::ForStmt
                collect_used_vars(stmt.iterable)
                stmt.body.each { |s| collect_used_vars_stmt(s) }
              when MLC::SemanticIR::Block
                stmt.stmts.each { |s| collect_used_vars_stmt(s) }
              end
            end

            def eliminate_expr(expr)
              @stats[:visited] += 1

              case expr
              when MLC::SemanticIR::BlockExpr
                eliminate_block(expr)
              when MLC::SemanticIR::IfExpr
                eliminate_if(expr)
              when MLC::SemanticIR::CallExpr
                eliminate_call(expr)
              when MLC::SemanticIR::BinaryExpr
                eliminate_binary(expr)
              when MLC::SemanticIR::UnaryExpr
                eliminate_unary(expr)
              else
                expr
              end
            end

            def eliminate_block(expr)
              optimized_stmts = []
              found_return = false

              expr.statements.each do |stmt|
                if found_return
                  @stats[:eliminated] += 1
                  next
                end

                optimized_stmt = eliminate_statement(stmt)

                # Check if we should keep this statement
                if keep_statement?(optimized_stmt)
                  optimized_stmts << optimized_stmt
                else
                  @stats[:eliminated] += 1
                end

                # Check if this is an unconditional return
                found_return = unconditional_return?(optimized_stmt)
              end

              optimized_result = if found_return
nil
else
(expr.result ? eliminate_expr(expr.result) : nil)
end

              MLC::SemanticIR::BlockExpr.new(
                statements: optimized_stmts,
                result: optimized_result,
                type: expr.type,
                origin: expr.origin
              )
            end

            def eliminate_if(expr)
              cond = eliminate_expr(expr.condition)
              then_branch = eliminate_expr(expr.then_branch)
              else_branch = expr.else_branch ? eliminate_expr(expr.else_branch) : nil

              MLC::SemanticIR::IfExpr.new(
                condition: cond,
                then_branch: then_branch,
                else_branch: else_branch,
                type: expr.type,
                origin: expr.origin
              )
            end

            def eliminate_call(expr)
              optimized_args = expr.args.map { |arg| eliminate_expr(arg) }
              optimized_callee = eliminate_expr(expr.callee)

              MLC::SemanticIR::CallExpr.new(
                callee: optimized_callee,
                args: optimized_args,
                type: expr.type,
                origin: expr.origin
              )
            end

            def eliminate_binary(expr)
              left = eliminate_expr(expr.left)
              right = eliminate_expr(expr.right)

              MLC::SemanticIR::BinaryExpr.new(
                op: expr.op,
                left: left,
                right: right,
                type: expr.type,
                origin: expr.origin
              )
            end

            def eliminate_unary(expr)
              operand = eliminate_expr(expr.operand)

              MLC::SemanticIR::UnaryExpr.new(
                op: expr.op,
                operand: operand,
                type: expr.type,
                origin: expr.origin
              )
            end

            def eliminate_statement(stmt)
              case stmt
              when MLC::SemanticIR::VariableDeclStmt
                MLC::SemanticIR::VariableDeclStmt.new(
                  name: stmt.name,
                  type: stmt.type,
                  value: eliminate_expr(stmt.value),
                  mutable: stmt.mutable,
                  origin: stmt.origin
                )
              when MLC::SemanticIR::ExprStatement
                MLC::SemanticIR::ExprStatement.new(
                  expression: eliminate_expr(stmt.expression),
                  origin: stmt.origin
                )
              when MLC::SemanticIR::Return
                MLC::SemanticIR::Return.new(
                  expr: stmt.expr ? eliminate_expr(stmt.expr) : nil,
                  origin: stmt.origin
                )
              when MLC::SemanticIR::AssignmentStmt
                MLC::SemanticIR::AssignmentStmt.new(
                  target: stmt.target,
                  value: eliminate_expr(stmt.value),
                  origin: stmt.origin
                )
              else
                stmt
              end
            end

            def keep_statement?(stmt)
              case stmt
              when MLC::SemanticIR::VariableDeclStmt
                # Keep if variable is used or has side effects
                @used_vars.include?(stmt.name) || has_side_effects?(stmt.value)
              when MLC::SemanticIR::ExprStatement
                # Keep if expression has side effects
                has_side_effects?(stmt.expression)
              when MLC::SemanticIR::Return, MLC::SemanticIR::AssignmentStmt
                true
              else
                true
              end
            end

            def unconditional_return?(stmt)
              case stmt
              when MLC::SemanticIR::Return
                true
              when MLC::SemanticIR::Block
                stmt.stmts.any? { |s| unconditional_return?(s) }
              else
                false
              end
            end

            def side_effects?(expr)
              case expr
              when MLC::SemanticIR::LiteralExpr, MLC::SemanticIR::VarExpr
                false
              when MLC::SemanticIR::BinaryExpr
                side_effects?(expr.left) || side_effects?(expr.right)
              when MLC::SemanticIR::UnaryExpr
                side_effects?(expr.operand)
              when MLC::SemanticIR::CallExpr
                # Assume all calls have side effects (conservative)
                true
              when MLC::SemanticIR::MemberExpr
                side_effects?(expr.object)
              when MLC::SemanticIR::RecordExpr
                expr.fields.values.any? { |field| side_effects?(field) }
              when MLC::SemanticIR::ArrayLiteralExpr
                expr.elements.any? { |elem| side_effects?(elem) }
              when MLC::SemanticIR::BlockExpr
                expr.statements.any? { |stmt| statement_has_side_effects?(stmt) } ||
                  (expr.result && side_effects?(expr.result))
              else
                true # Conservative: assume unknown nodes have side effects
              end
            end

            alias has_side_effects? side_effects?

            def statement_has_side_effects?(stmt)
              case stmt
              when MLC::SemanticIR::VariableDeclStmt
                has_side_effects?(stmt.value)
              when MLC::SemanticIR::ExprStatement
                has_side_effects?(stmt.expression)
              when MLC::SemanticIR::Return
                stmt.expr ? has_side_effects?(stmt.expr) : false
              when MLC::SemanticIR::AssignmentStmt
                true # Assignments are side effects
              else
                true
              end
            end
          end
        end
      end
    end
  end
end

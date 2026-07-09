# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          module Features
            # EscapeAnalyzer - determines which Fn(...)-typed parameters of a function
            # are non-escaping: every occurrence of the parameter name in the function
            # body is the direct callee of a call expression (`param(args)`), never
            # stored in a field/variable, returned, passed as data to another call, or
            # referenced inside a nested lambda.
            #
            # Non-escaping parameters compile to a template type parameter instead of
            # `std::function<...>` (zero allocation, zero refcount) - see
            # docs/agent/TRACK_LANG_CLOSURE_ESCAPE.md.
            #
            # Conservative by construction and ignores let-shadowing: any occurrence
            # that is not unambiguously a direct call marks the parameter escaping,
            # and a shadowed reuse of the same name only adds spurious "escaping"
            # occurrences, never spurious "non-escaping" conclusions. Escaping is the
            # existing (safe) default behavior, so imprecision here can only fall back
            # to today's codegen, never produce incorrect code.
            class EscapeAnalyzer
              # @param func_decl [MLC::Source::AST::FuncDecl]
              # @param function_typed_param_names [Set<String>] names of params whose
              #   declared type is AST::FunctionType
              # @return [Set<String>] subset of function_typed_param_names that is non-escaping
              def non_escaping_params(func_decl, function_typed_param_names)
                return Set.new if function_typed_param_names.empty? || func_decl.body.nil?

                escaping = Set.new
                visit(func_decl.body, function_typed_param_names, escaping)
                function_typed_param_names - escaping
              end

              private

              def visit(node, watched, escaping)
                return unless node

                case node
                when MLC::Source::AST::VarRef
                  escaping.add(node.name) if watched.include?(node.name)

                when MLC::Source::AST::Call
                  visit_call_like(node.callee, Array(node.args), watched, escaping)

                when MLC::Source::AST::SafeCall
                  visit_call_like(node.object, Array(node.args), watched, escaping)

                when MLC::Source::AST::Lambda
                  # Any watched name referenced inside a nested lambda body is
                  # conservatively escaping - whether that nested lambda itself
                  # escapes is a separate question this pass does not answer.
                  visit(node.body, watched, escaping)

                when MLC::Source::AST::Let
                  visit(node.value, watched, escaping)
                  visit(node.body, watched, escaping) if node.respond_to?(:body)

                when MLC::Source::AST::Block, MLC::Source::AST::BlockExpr
                  Array(node.statements).each { |stmt| visit(stmt, watched, escaping) }
                  visit(node.result_expr, watched, escaping) if node.respond_to?(:result_expr)

                when MLC::Source::AST::DoExpr
                  Array(node.statements).each { |stmt| visit(stmt, watched, escaping) } if node.respond_to?(:statements)
                  visit(node.result, watched, escaping) if node.respond_to?(:result)

                when MLC::Source::AST::BinaryOp
                  visit(node.left, watched, escaping)
                  visit(node.right, watched, escaping)

                when MLC::Source::AST::UnaryOp
                  visit(node.operand, watched, escaping)

                when MLC::Source::AST::MemberAccess
                  visit(node.object, watched, escaping)

                when MLC::Source::AST::TupleAccess
                  visit(node.tuple, watched, escaping)

                when MLC::Source::AST::IndexAccess
                  visit(node.object, watched, escaping)
                  visit(node.index, watched, escaping)

                when MLC::Source::AST::SliceAccess
                  visit(node.object, watched, escaping)
                  visit(node.start_index, watched, escaping) if node.respond_to?(:start_index)
                  visit(node.end_index, watched, escaping) if node.respond_to?(:end_index)

                when MLC::Source::AST::IfExpr, MLC::Source::AST::IfStmt
                  visit(node.condition, watched, escaping)
                  visit(node.then_branch, watched, escaping)
                  visit(node.else_branch, watched, escaping) if node.else_branch

                when MLC::Source::AST::MatchExpr
                  visit(node.scrutinee, watched, escaping)
                  Array(node.arms).each do |arm|
                    guard = arm[:guard] || arm["guard"]
                    body = arm[:body] || arm["body"]
                    visit(guard, watched, escaping) if guard
                    visit(body, watched, escaping) if body
                  end

                when MLC::Source::AST::ArrayLiteral
                  Array(node.elements).each { |elem| visit(elem, watched, escaping) }

                when MLC::Source::AST::TupleLit
                  Array(node.elements).each { |elem| visit(elem, watched, escaping) }

                when MLC::Source::AST::RecordLit
                  node.fields.each_value { |field_value| visit(field_value, watched, escaping) }

                when MLC::Source::AST::ForLoop
                  visit(node.iterable, watched, escaping)
                  visit(node.body, watched, escaping)

                when MLC::Source::AST::WhileLoop, MLC::Source::AST::WhileStmt
                  visit(node.condition, watched, escaping)
                  visit(node.body, watched, escaping)

                when MLC::Source::AST::ListComprehension
                  if node.respond_to?(:generators) && node.generators
                    node.generators.each do |gen|
                      visit(gen.iterable, watched, escaping)
                      visit(gen.condition, watched, escaping) if gen.respond_to?(:condition) && gen.condition
                    end
                  end
                  visit(node.body, watched, escaping)

                when MLC::Source::AST::PipeOp
                  visit(node.left, watched, escaping)
                  visit(node.right, watched, escaping)

                when MLC::Source::AST::RangeExpr
                  visit(node.start_expr, watched, escaping) if node.respond_to?(:start_expr)
                  visit(node.end_expr, watched, escaping) if node.respond_to?(:end_expr)

                when MLC::Source::AST::Return
                  visit(node.expr, watched, escaping) if node.expr

                when MLC::Source::AST::ExprStmt
                  visit(node.expr, watched, escaping) if node.respond_to?(:expr)

                when MLC::Source::AST::VariableDecl
                  visit(node.value, watched, escaping) if node.respond_to?(:value) && node.value

                when MLC::Source::AST::Assignment
                  visit(node.target, watched, escaping) if node.respond_to?(:target)
                  visit(node.value, watched, escaping)

                when MLC::Source::AST::StringInterpolation
                  Array(node.parts).each { |part| visit(part, watched, escaping) } if node.respond_to?(:parts)

                when MLC::Source::AST::UnsafeBlock
                  visit(node.body, watched, escaping) if node.respond_to?(:body)

                # Literals never reference anything.
                when MLC::Source::AST::IntLit, MLC::Source::AST::FloatLit,
                     MLC::Source::AST::StringLit, MLC::Source::AST::RegexLit,
                     MLC::Source::AST::UnitLit
                  nil

                else
                  # Unknown node shape: conservatively mark every watched name
                  # escaping rather than silently missing a reference. Escaping is
                  # the existing safe default codegen, so this never breaks
                  # correctness - only forfeits the optimization for that function.
                  watched.each { |name| escaping.add(name) }
                end
              end

              # A `Call`/`SafeCall`-like node: only the direct callee position is
              # exempt from marking escaping; everything else (arguments) is a plain
              # use of the value, which counts as escaping if it references a
              # watched name.
              def visit_call_like(callee, args, watched, escaping)
                unless callee.is_a?(MLC::Source::AST::VarRef) && watched.include?(callee.name)
                  visit(callee, watched, escaping)
                end

                args.each do |argument_node|
                  value = argument_node.is_a?(MLC::Source::AST::NamedArg) ? argument_node.value : argument_node
                  visit(value, watched, escaping)
                end
              end
            end
          end
        end
      end
    end
  end
end

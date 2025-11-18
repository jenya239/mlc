# frozen_string_literal: true

# All classes provided via dependency injection:
# - match_analyzer (MLC::Common::Typing::MatchAnalyzer)
# - type_unification_service (TypeUnificationService)

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # MatchService - Handles match expression lowering and pattern binding
          class MatchService
            def initialize(
              ir_builder:,
              type_checker:,
              var_type_registry:,
              ast_factory:,
              ast_type_checker:,
              type_unification_service:,
              match_analyzer:
            )
              @ir_builder = ir_builder
              @type_checker = type_checker
              @var_type_registry = var_type_registry
              @ast_factory = ast_factory
              @ast_type_checker = ast_type_checker
              @type_unification_service = type_unification_service
              @match_analyzer = match_analyzer
            end

            def build(node, expression_visitor:, statement_visitor:)
              scrutinee_ir = expression_visitor.visit(node.scrutinee)

              if statement_visitor && statement_match?(node)
                return build_statement_form(
                  node,
                  scrutinee_ir,
                  expression_visitor: expression_visitor,
                  statement_visitor: statement_visitor
                )
              end

              build_expression_form(
                node,
                scrutinee_ir,
                expression_visitor: expression_visitor
              )
            end

            private

            def build_expression_form(node, scrutinee_ir, expression_visitor:)
              analysis = @match_analyzer.analyze(
                scrutinee_type: scrutinee_ir.type,
                arms: node.arms,
                transform_arm: ->(scrutinee_type, arm) do
                  transform_expression_arm(scrutinee_type, arm, expression_visitor)
                end
              )

              @ir_builder.match_expr(scrutinee: scrutinee_ir, arms: analysis.arms, type: analysis.result_type, origin: node)
            end

            def build_statement_form(node, scrutinee_ir, expression_visitor:, statement_visitor:)
              arms = node.arms.map do |arm|
                transform_statement_arm(
                  scrutinee_ir.type,
                  arm,
                  expression_visitor: expression_visitor,
                  statement_visitor: statement_visitor
                )
              end

              match_stmt = @ir_builder.match_stmt(scrutinee: scrutinee_ir, arms: arms, origin: node)
              unit = @ir_builder.unit_literal(origin: node)

              @ir_builder.block_expr(
                statements: [match_stmt],
                result: unit,
                type: unit.type,
                origin: node
              )
            end

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

            def transform_statement_arm(scrutinee_type, arm, expression_visitor:, statement_visitor:)
              snapshot = @var_type_registry.snapshot

              pattern_data = transform_pattern(arm[:pattern])
              bind_pattern_variables(pattern_data, scrutinee_type)

              guard_ir = arm[:guard] ? expression_visitor.visit(arm[:guard]) : nil
              body_ir = build_statement_body(arm[:body], statement_visitor)

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
              bindings = []

              Array(pattern[:fields]).each_with_index do |field, index|
                next if ignored_binding?(field)

                field_type = field_types[index] || unknown_type
                bind_variable(field, field_type)
                bindings << field
              end

              pattern[:bindings] = bindings unless bindings.empty?
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

            def statement_match?(node)
              node.arms.all? do |arm|
                arm[:guard].nil? &&
                  statement_safe_pattern?(arm[:pattern]) &&
                  unit_branch?(arm[:body])
              end
            end

            def statement_safe_pattern?(pattern)
              kind = pattern&.kind
              return false unless kind

              %i[constructor wildcard var].include?(kind)
            end

            def unit_branch?(body)
              return false unless @ast_type_checker.block_expr?(body)

              result = body.result_expr
              result && @ast_type_checker.unit_literal?(result)
            end

            def build_statement_body(body_ast, statement_visitor)
              statements = extract_block_statements(body_ast)
              ir_statements = statement_visitor.visit_statements(statements)
              @ir_builder.block(statements: ir_statements, origin: body_ast)
            end

            def extract_block_statements(body_ast)
              if @ast_type_checker.block_expr?(body_ast)
                stmts = body_ast.statements.dup
                if body_ast.result_expr && !@ast_type_checker.unit_literal?(body_ast.result_expr)
                  expr = body_ast.result_expr
                  stmts << @ast_factory.expr_stmt(expr: expr, origin: expr.origin)
                end
                stmts
              elsif @ast_type_checker.block_statement?(body_ast)
                body_ast.stmts
              else
                [@ast_factory.expr_stmt(expr: body_ast, origin: body_ast.origin)]
              end
            end
          end
        end
      end
    end
  end
end

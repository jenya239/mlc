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
                # Handle nested patterns: fields can be strings or Pattern AST nodes
                transformed_fields = Array(data[:fields]).map do |field|
                  if field.is_a?(MLC::Source::AST::Pattern)
                    # Recursively transform nested pattern
                    transform_pattern(field)
                  else
                    # Simple binding (string)
                    field
                  end
                end

                {
                  kind: :constructor,
                  name: data[:name],
                  fields: transformed_fields
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
              when :or
                # Or-pattern: alternatives list
                transformed_alternatives = Array(data[:alternatives]).map do |alt|
                  transform_pattern(alt)
                end
                {
                  kind: :or,
                  alternatives: transformed_alternatives
                }
              when :array
                # Array pattern: elements list
                transformed_elements = Array(data[:elements]).map do |elem|
                  if elem.is_a?(MLC::Source::AST::Pattern)
                    # Recursively transform nested pattern
                    transform_pattern(elem)
                  else
                    # Unexpected non-pattern in array (shouldn't happen with proper parser)
                    elem
                  end
                end
                {
                  kind: :array,
                  elements: transformed_elements
                }
              when :tuple
                # Tuple pattern: elements list
                transformed_elements = Array(data[:elements]).map do |elem|
                  if elem.is_a?(MLC::Source::AST::Pattern)
                    transform_pattern(elem)
                  else
                    elem
                  end
                end
                {
                  kind: :tuple,
                  elements: transformed_elements
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
              when :or
                # For or-patterns, bind variables from the first alternative
                # All alternatives must have same bindings (verified at semantic level)
                first_alt = pattern[:alternatives]&.first
                bind_pattern_variables(first_alt, scrutinee_type) if first_alt
              when :array
                # Array pattern: bind variables from element patterns
                # Get element type from scrutinee type (if it's an array type)
                element_type = if scrutinee_type.respond_to?(:element_type)
                                 scrutinee_type.element_type
                               else
                                 unknown_type
                               end

                Array(pattern[:elements]).each do |elem|
                  # Recursively bind variables from each element pattern
                  bind_pattern_variables(elem, element_type)
                end
              when :tuple
                # Tuple pattern: bind variables from element patterns
                # Get element types from scrutinee type (if it's a tuple type)
                element_types = if scrutinee_type.respond_to?(:element_types)
                                  scrutinee_type.element_types
                                else
                                  []
                                end

                Array(pattern[:elements]).each_with_index do |elem, index|
                  elem_type = element_types[index] || unknown_type
                  bind_pattern_variables(elem, elem_type)
                end
              end
            end

            def bind_constructor_pattern(pattern, scrutinee_type)
              info = @type_unification_service.constructor_info_for(pattern[:name], scrutinee_type)
              field_types = info ? info.param_types : []
              bindings = []

              Array(pattern[:fields]).each_with_index do |field, index|
                field_type = field_types[index] || unknown_type

                if field.is_a?(Hash) && field[:kind]
                  # Nested pattern - recursively bind its variables
                  bind_pattern_variables(field, field_type)
                  bindings << field
                elsif ignored_binding?(field)
                  next
                else
                  # Simple variable binding
                  bind_variable(field, field_type)
                  bindings << field
                end
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

              %i[constructor wildcard var tuple].include?(kind)
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

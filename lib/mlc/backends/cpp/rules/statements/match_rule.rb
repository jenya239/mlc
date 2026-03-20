# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::MatchStmt to C++ std::visit
          class MatchRule < StatementRule
            def applies?(node)
              context.checker.match_stmt?(node)
            end

            def apply(node)
              scrutinee = context.lower_expression(node.scrutinee)
              scrutinee_type = node.scrutinee&.type
              visit_value = build_visit_value(scrutinee, scrutinee_type, scrutinee_node: node.scrutinee)

              arms = node.arms.map { |arm| lower_match_arm(arm) }

              CppAst::Nodes::MatchStatement.new(
                value: visit_value,
                arms: arms,
                arm_separators: Array.new([arms.size - 1, 0].max, ",\n")
              )
            end

            private

            def lower_match_arm(arm)
              pattern = arm[:pattern]

              if pattern[:kind] == :wildcard
                # Wildcard pattern (_)
                body = lower_statement_block(arm[:body])
                CppAst::Nodes::WildcardMatchArmStatement.new(
                  var_name: "_unused",
                  body: body
                )
              else
                # Constructor pattern
                lower_constructor_arm(arm)
              end
            end

            def lower_constructor_arm(arm)
              pattern = arm[:pattern]
              case_name = pattern[:name]
              var_name = context.sanitize_identifier(case_name.downcase)
              raw_bindings = Array(pattern[:bindings] || pattern[:fields]).compact
              bindings = uniquify_wildcard_bindings(raw_bindings)
              sanitized_bindings = bindings.map { |name| context.sanitize_identifier(name) }

              body_block = lower_statement_block(arm[:body])

              # Add structured binding if there are bindings
              block_with_binding = if sanitized_bindings.any?
                                     add_structured_binding(body_block, sanitized_bindings, var_name)
                                   else
                                     body_block
                                   end

              CppAst::Nodes::MatchArmStatement.new(
                case_name: case_name,
                var_name: var_name,
                body: block_with_binding
              )
            end

            MATCH_SCRUTINEE_SHARED_CALLS = MLC::Backends::Cpp::Rules::Expressions::MatchRule::MATCH_SCRUTINEE_SHARED_CALLS

            def build_visit_value(scrutinee, scrutinee_type, scrutinee_node: nil)
              if scrutinee_node.is_a?(MLC::SemanticIR::CallExpr) &&
                 scrutinee_node.callee.is_a?(MLC::SemanticIR::VarExpr) &&
                 MATCH_SCRUTINEE_SHARED_CALLS.include?(scrutinee_node.callee.name)
                src = scrutinee.to_source
                return context.factory.raw_expression(code: "(*#{src})")
              end

              if scrutinee_node && context.checker.var_expr?(scrutinee_node)
                vt = context.lookup_var_type(scrutinee_node.name)
                scrutinee_type = vt unless vt.nil?
              end

              return scrutinee unless scrutinee_type

              scrutinee_src = scrutinee.to_source
              is_shared = shared_type?(scrutinee_type)
              needs_star = is_shared || ::MLC::Backends::Cpp::MatchScrutineeDeref.ast_sum_needs_star?(
                context.type_registry, scrutinee_type
              )
              inner = is_shared ? scrutinee_type.type_args&.first : scrutinee_type
              inner_type_name = extract_type_name(inner)
              is_wrapper = inner_type_name && context.cyclic_sum_types.include?(inner_type_name)

              base = needs_star ? "(*#{scrutinee_src})" : scrutinee_src
              base = "#{base}._" if is_wrapper
              context.factory.raw_expression(code: base)
            end

            def extract_type_name(type)
              return nil unless type
              return type.name if type.respond_to?(:name) && type.name
              return extract_type_name(type.base_type) if type.respond_to?(:type_args) && type.respond_to?(:base_type)

              nil
            end

            def shared_type?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)

              type.base_type.respond_to?(:name) && type.base_type.name == "Shared"
            end

            def uniquify_wildcard_bindings(bindings)
              counter = 0
              bindings.map do |b|
                if b == "_"
                  name = "_w#{counter}"
                  counter += 1
                  name
                else
                  b
                end
              end
            end

            def lower_statement_block(body_ir)
              if context.checker.block_expr?(body_ir)
                statements = body_ir.statements.map { |stmt| context.lower_statement(stmt) }
                context.factory.block_statement(
                  statements: statements,
                  statement_trailings: Array.new(statements.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                stmt = context.lower_statement(body_ir)
                context.factory.block_statement(
                  statements: [stmt],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              end
            end

            def add_structured_binding(body_block, bindings, var_name)
              # Create structured binding declaration: auto [field1, field2, ...] = var_name;
              binding_list = bindings.join(", ")
              binding_stmt = CppAst::Nodes::RawStatement.new(
                code: "auto [#{binding_list}] = #{var_name};"
              )

              # Prepend binding to body statements
              new_statements = [binding_stmt] + body_block.statements
              new_trailings = ["\n"] + body_block.statement_trailings

              context.factory.block_statement(
                statements: new_statements,
                statement_trailings: new_trailings,
                lbrace_suffix: body_block.lbrace_suffix,
                rbrace_prefix: body_block.rbrace_prefix
              )
            end
          end
        end
      end
    end
  end
end

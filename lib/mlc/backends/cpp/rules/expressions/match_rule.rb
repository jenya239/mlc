# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR match expressions to C++ switch/if chains or std::visit
          # Handles two main strategies:
          # 1. Regex patterns -> if-else chain in IIFE
          # 2. Sum type patterns -> std::visit with overloaded lambdas
          class MatchRule < ExpressionRule
            def applies?(node)
              context.checker.match_expr?(node)
            end

            def apply(node)
              # Lower scrutinee expression
              scrutinee = context.lower_expression(node.scrutinee)

              # Check if any arms have regex patterns
              has_regex = node.arms.any? { |arm| arm[:pattern][:kind] == :regex }

              # Check if any arms have guard clauses
              has_guards = node.arms.any? { |arm| arm[:guard] }

              # Check if any arms have nested patterns
              has_nested = node.arms.any? { |arm| has_nested_pattern?(arm[:pattern]) }

              # Check if any arms have wildcard patterns
              # Wildcard patterns generate auto&& which incorrectly matches all types in std::visit
              has_wildcard = node.arms.any? { |arm| arm[:pattern][:kind] == :wildcard }

              # Check if any arms have literal patterns
              # Literal patterns need equality checks, not std::visit
              has_literal = node.arms.any? do |arm|
                pattern = arm[:pattern]
                if pattern.is_a?(Hash)
                  pattern[:kind] == :literal
                elsif pattern.respond_to?(:kind)
                  pattern.kind == :literal
                else
                  false
                end
              end

              if has_regex
                # Generate if-else chain for regex matching
                lower_match_with_regex(node, scrutinee)
              elsif has_guards || has_nested || has_wildcard || has_literal
                # Generate if-else chain for guard clauses, nested patterns, wildcards, or literals
                # (std::visit doesn't support fallthrough, nested destructuring, proper wildcard semantics, or literal matching)
                lower_match_with_guards(node, scrutinee)
              else
                # Generate MatchExpression with std::visit
                arms = node.arms.map { |arm| lower_match_arm(arm) }

                CppAst::Nodes::MatchExpression.new(
                  value: scrutinee,
                  arms: arms,
                  arm_separators: Array.new([arms.size - 1, 0].max, ",\n")
                )
              end
            end

            # Check if pattern contains nested constructor patterns or or-patterns
            def has_nested_pattern?(pattern)
              return true if pattern[:kind] == :or

              return false unless pattern[:kind] == :constructor

              Array(pattern[:fields]).any? do |field|
                field.is_a?(Hash) && field[:kind] == :constructor
              end
            end

            private

            # Lower match with regex patterns to IIFE with if-else chain
            def lower_match_with_regex(match_expr, scrutinee)
              # Generate an IIFE (Immediately Invoked Function Expression) lambda
              # that contains if-else chain for regex matching:
              # [&]() {
              #   if (regex1.test(scrutinee)) return value1;
              #   if (regex2.test(scrutinee)) return value2;
              #   return default_value;
              # }()

              statements = []

              match_expr.arms.each do |arm|
                pattern = arm[:pattern]
                body = context.lower_expression(arm[:body])

                case pattern[:kind]
                when :regex
                  statements.concat(build_regex_arm_statements(pattern, body, scrutinee))

                when :wildcard, :var
                  # Default case - just return
                  statements << context.factory.return_statement(expression: body)

                else
                  # Other patterns not yet supported in regex match
                  # Treat as wildcard for now
                  statements << context.factory.return_statement(expression: body)
                end
              end

              # Build body string from statements
              body_str = statements.map(&:to_source).join(" ")

              # Create IIFE lambda: [&]() { ... }()
              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: ""
              )

              context.factory.function_call(
                callee: lambda_expr,
                arguments: [],
                argument_separators: []
              )
            end

            # Build if statements for regex arm
            def build_regex_arm_statements(pattern, body, scrutinee)
              regex_pattern = pattern[:pattern]
              regex_flags = pattern[:flags] || ""
              bindings = pattern[:bindings] || []

              # Create regex object
              pattern_string = build_MLC_string(regex_pattern)
              func_name = regex_flags.include?("i") ? "mlc::regex_i" : "mlc::regex"
              regex_obj = context.factory.function_call(
                callee: context.factory.identifier(name: func_name),
                arguments: [pattern_string],
                argument_separators: []
              )

              if bindings.empty?
                # No capture groups - use test()
                test_member = context.factory.member_access(
                  object: regex_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "test")
                )

                test_result = context.factory.function_call(
                  callee: test_member,
                  arguments: [scrutinee],
                  argument_separators: []
                )

                return_stmt = context.factory.return_statement(expression: body)
                if_stmt = context.factory.if_statement(
                  condition: test_result,
                  then_statement: return_stmt,
                  else_statement: nil
                )
                [if_stmt]
              else
                # Has capture groups - use match() and extract captures
                # Generate: if (auto match_opt = regex.match(text)) { auto match = *match_opt; auto user = match.get(1).text(); ... return body; }

                regex_src = regex_obj.to_source
                scrutinee_src = scrutinee.to_source
                body_src = body.to_source

                # Build capture variable declarations
                capture_decls = []
                bindings.each_with_index do |binding, idx|
                  next if binding == "_"  # Skip wildcards
                  # Generate: auto user = match.get(1).text();
                  capture_decls << "auto #{binding} = match.get(#{idx}).text();"
                end

                # Build if statement with match and captures
                if_body = [
                  "auto match = *match_opt;",
                  *capture_decls,
                  "return #{body_src};"
                ].join(" ")

                # Create raw if statement as string
                if_str = "if (auto match_opt = #{regex_src}.match(#{scrutinee_src})) { #{if_body} }"

                # Add as raw statement
                [context.factory.raw_statement(code: if_str)]
              end
            end

            # Lower match arm to MatchArm node
            def lower_match_arm(arm)
              pattern = arm[:pattern]
              body = context.lower_expression(arm[:body])

              case pattern[:kind]
              when :constructor
                # Generate MatchArm with constructor pattern
                case_name = pattern[:name]
                bindings = pattern[:bindings] || pattern[:fields] || []

                CppAst::Nodes::MatchArm.new(
                  case_name: case_name,
                  bindings: bindings.reject { |f| f == "_" },  # Filter out wildcards
                  body: body
                )

              when :wildcard, :var
                # For wildcard or variable patterns, generate catch-all arm
                var_name = pattern[:kind] == :var ? pattern[:name] : "_unused"

                CppAst::Nodes::WildcardMatchArm.new(
                  var_name: var_name,
                  body: body
                )

              when :literal
                # Literal patterns need special handling
                # For now, treat as wildcard with a check
                CppAst::Nodes::WildcardMatchArm.new(
                  var_name: "_v",
                  body: body
                )

              when :regex
                # Regex pattern matching in std::visit context
                # Generate wildcard arm (actual matching handled elsewhere)
                CppAst::Nodes::WildcardMatchArm.new(
                  var_name: "_text",
                  body: body
                )

              else
                raise "Unknown pattern kind: #{pattern[:kind]}"
              end
            end

            # Helper to build mlc::String(...) for regex patterns
            def build_MLC_string(value)
              context.factory.function_call(
                callee: context.factory.identifier(name: "mlc::String"),
                arguments: [context.cpp_string_literal(value)],
                argument_separators: []
              )
            end

            # Lower match with guard clauses to IIFE with if-else chain
            # std::visit doesn't support fallthrough, so we use if-else chain instead
            #
            # Example MLC:
            #   match value
            #     | Some(x) if x > 0 => x * 2
            #     | Some(x) => 0
            #     | None => -1
            #
            # Generated C++:
            #   [&]() {
            #     if (std::holds_alternative<Some>(value)) {
            #       auto x = std::get<Some>(value).value;
            #       if (x > 0) return x * 2;
            #     }
            #     if (std::holds_alternative<Some>(value)) {
            #       auto x = std::get<Some>(value).value;
            #       return 0;
            #     }
            #     if (std::holds_alternative<None>(value)) {
            #       return -1;
            #     }
            #     throw std::runtime_error("non-exhaustive match");
            #   }()
            def lower_match_with_guards(match_expr, scrutinee)
              statements = []
              scrutinee_src = scrutinee.to_source

              match_expr.arms.each do |arm|
                pattern = arm[:pattern]
                guard = arm[:guard]
                body = context.lower_expression(arm[:body])
                body_src = body.to_source

                case pattern[:kind]
                when :constructor
                  statements << build_constructor_guard_arm(pattern, guard, body_src, scrutinee_src)

                when :or
                  # Or-pattern: generate (cond1 || cond2 || ...) condition
                  statements << build_or_pattern_arm(pattern, guard, body_src, scrutinee_src)

                when :literal
                  # Literal pattern: check equality
                  condition = build_pattern_condition(pattern, scrutinee_src)
                  if guard
                    guard_expr = context.lower_expression(guard)
                    guard_src = guard_expr.to_source
                    statements << context.factory.raw_statement(
                      code: "if (#{condition} && #{guard_src}) { return #{body_src}; }"
                    )
                  else
                    statements << context.factory.raw_statement(
                      code: "if (#{condition}) { return #{body_src}; }"
                    )
                  end

                when :wildcard, :var
                  # Wildcard/var with optional guard
                  if guard
                    guard_expr = context.lower_expression(guard)
                    guard_src = guard_expr.to_source
                    statements << context.factory.raw_statement(
                      code: "if (#{guard_src}) { return #{body_src}; }"
                    )
                  else
                    # Final wildcard - just return
                    statements << context.factory.return_statement(expression: body)
                  end

                else
                  # Unknown pattern kind - treat as wildcard
                  statements << context.factory.return_statement(expression: body)
                end
              end

              # Build IIFE
              body_str = statements.map(&:to_source).join(" ")
              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: ""
              )

              context.factory.function_call(
                callee: lambda_expr,
                arguments: [],
                argument_separators: []
              )
            end

            # Build if statement for constructor pattern with optional guard
            def build_constructor_guard_arm(pattern, guard, body_src, scrutinee_src)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []

              # Build std::holds_alternative check
              holds_check = "std::holds_alternative<#{case_name}>(#{scrutinee_src})"

              # Build binding declarations and nested pattern checks
              binding_decls = []
              nested_checks = []
              temp_var_counter = 0

              if bindings.any?
                # First extract the variant to a temporary variable
                temp_var = "_v_#{case_name.downcase}"
                binding_decls << "auto #{temp_var} = std::get<#{case_name}>(#{scrutinee_src});"

                # Use structured binding to extract all fields at once
                non_wildcard_bindings = bindings.reject { |b| b == "_" || (b.is_a?(Hash) && b[:kind] == :constructor) }
                if non_wildcard_bindings.any?
                  binding_list = bindings.map { |b| b.is_a?(Hash) ? "_" : (b == "_" ? "_" : b) }.join(", ")
                  binding_decls << "auto [#{binding_list}] = #{temp_var};"
                end

                # Handle nested patterns separately
                bindings.each_with_index do |binding, idx|
                  if binding.is_a?(Hash) && binding[:kind] == :constructor
                    # Nested pattern - generate temp var and nested check
                    nested_temp_var = "_nested_#{temp_var_counter}"
                    temp_var_counter += 1

                    # Extract nested value using structured binding access
                    if bindings.length == 1
                      # Single field - use .field0
                      binding_decls << "auto #{nested_temp_var} = #{temp_var}.field0;"
                    else
                      # Multiple fields - use .fieldN
                      binding_decls << "auto #{nested_temp_var} = #{temp_var}.field#{idx};"
                    end

                    # Build nested pattern check
                    nested_check = build_nested_pattern_check(binding, nested_temp_var)
                    nested_checks << nested_check
                  end
                end
              end

              binding_str = binding_decls.join(" ")

              # Build the inner body with nested checks
              if nested_checks.any?
                # Wrap body in nested if-statements for nested pattern checks
                inner_return = guard ? "if (#{context.lower_expression(guard).to_source}) { return #{body_src}; }" : "return #{body_src};"
                nested_body = nested_checks.reverse.reduce(inner_return) do |acc, check|
                  "if (#{check[:condition]}) { #{check[:bindings]} #{acc} }"
                end
                inner_body = "#{binding_str} #{nested_body}"
              elsif guard
                guard_expr = context.lower_expression(guard)
                guard_src = guard_expr.to_source
                inner_body = "#{binding_str} if (#{guard_src}) { return #{body_src}; }"
              else
                inner_body = "#{binding_str} return #{body_src};"
              end

              context.factory.raw_statement(
                code: "if (#{holds_check}) { #{inner_body} }"
              )
            end

            # Build nested pattern check for nested constructor patterns
            def build_nested_pattern_check(pattern, scrutinee_var)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []

              condition = "std::holds_alternative<#{case_name}>(#{scrutinee_var})"

              binding_decls = []
              # Extract variant to temp variable
              temp_var = "_v_nested_#{case_name.downcase}"
              binding_decls << "auto #{temp_var} = std::get<#{case_name}>(#{scrutinee_var});"

              # Use structured binding
              non_wildcard_bindings = bindings.reject { |b| b == "_" || b.is_a?(Hash) }
              if non_wildcard_bindings.any?
                binding_list = bindings.map { |b| b.is_a?(Hash) ? "_" : (b == "_" ? "_" : b) }.join(", ")
                binding_decls << "auto [#{binding_list}] = #{temp_var};"
              end

              { condition: condition, bindings: binding_decls.join(" ") }
            end

            # Build or-pattern arm with (cond1 || cond2 || ...) condition
            def build_or_pattern_arm(pattern, guard, body_src, scrutinee_src)
              alternatives = pattern[:alternatives] || []

              # Build OR condition from all alternatives
              conditions = alternatives.map do |alt|
                build_pattern_condition(alt, scrutinee_src)
              end

              or_condition = conditions.join(" || ")

              if guard
                guard_expr = context.lower_expression(guard)
                guard_src = guard_expr.to_source
                inner_body = "if (#{guard_src}) { return #{body_src}; }"
              else
                inner_body = "return #{body_src};"
              end

              context.factory.raw_statement(
                code: "if (#{or_condition}) { #{inner_body} }"
              )
            end

            # Build condition check for a single pattern
            def build_pattern_condition(pattern, scrutinee_src)
              case pattern[:kind]
              when :constructor
                case_name = pattern[:name]
                "std::holds_alternative<#{case_name}>(#{scrutinee_src})"
              when :literal
                value = pattern[:value]
                literal_src = format_literal_value(value)
                "#{scrutinee_src} == #{literal_src}"
              when :wildcard
                "true"
              when :var
                "true"
              else
                "true"
              end
            end

            # Format literal value for C++ code generation
            def format_literal_value(value)
              case value
              when String
                # String literal: create mlc::String("...")
                cpp_str = context.cpp_string_literal(value)
                "mlc::String(#{cpp_str})"
              when TrueClass, FalseClass
                # Boolean: use C++ true/false
                value.to_s
              else
                # Numeric literals: use as-is
                value.to_s
              end
            end
          end
        end
      end
    end
  end
end

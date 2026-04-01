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
              has_nested = node.arms.any? { |arm| nested_pattern?(arm[:pattern]) }

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

              # Check if any arms have array patterns
              # Array patterns need size/element checks, not std::visit
              has_array = node.arms.any? { |arm| arm[:pattern][:kind] == :array }

              # Check if any arms have tuple patterns
              # Tuple patterns need destructuring with std::get<N>
              has_tuple = node.arms.any? { |arm| arm[:pattern][:kind] == :tuple }

              # Check if scrutinee is an Option type (requires special handling)
              is_option_type = option_type?(node.scrutinee&.type)

              if has_regex
                # Generate if-else chain for regex matching
                lower_match_with_regex(node, scrutinee)
              elsif has_guards || has_nested || has_wildcard || has_literal || has_array || has_tuple || is_option_type
                # Generate if-else chain for guard clauses, nested patterns, wildcards, literals,
                # array patterns, tuple patterns, or Option types (std::optional needs special handling)
                lower_match_with_guards(node, scrutinee)
              else
                # Generate MatchExpression with std::visit
                scrutinee_type = node.scrutinee&.type
                match_return_type = node.respond_to?(:type) && node.type ? context.map_type(node.type) : nil
                # Dereference shared_ptr before std::visit; use ._ for wrapper struct
                visit_value = build_visit_value(scrutinee, scrutinee_type, scrutinee_node: node.scrutinee)
                arms = node.arms.map { |arm| lower_match_arm(arm, scrutinee_type: scrutinee_type, return_type: match_return_type) }

                CppAst::Nodes::MatchExpression.new(
                  value: visit_value,
                  arms: arms,
                  arm_separators: Array.new([arms.size - 1, 0].max, ",\n")
                )
              end
            end

            # Check if pattern contains nested constructor patterns or or-patterns
            def nested_pattern?(pattern)
              return true if pattern[:kind] == :or

              return false unless pattern[:kind] == :constructor

              Array(pattern[:fields]).any? do |field|
                field.is_a?(Hash) && field[:kind] == :constructor
              end
            end

            # Replace duplicate "_" wildcards with unique names (_w0, _w1, ...)
            # C++ structured bindings require unique names for each binding.
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

            # Calls that return Shared<sum> but IR may lose Shared wrapper for match scrutinee.
            MATCH_SCRUTINEE_SHARED_CALLS = %w[decl_inner find_field_val param_typ].freeze

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
              needs_star = is_shared || MatchScrutineeDeref.ast_sum_needs_star?(context.type_registry, scrutinee_type)
              inner = is_shared ? scrutinee_type.type_args&.first : scrutinee_type
              inner_type_name = inner ? extract_type_name(inner) : nil
              is_wrapper = inner_type_name && context.cyclic_sum_types.include?(inner_type_name)

              base = needs_star ? "(*#{scrutinee_src})" : scrutinee_src
              base = "#{base}._" if is_wrapper
              context.factory.raw_expression(code: base)
            end

            def shared_type?(type)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)

              base_name = extract_type_name(type.base_type)
              base_name == "Shared"
            end

            # Check if type is stdlib Option<T> (std::optional)
            # Returns false if user defined their own Option sum type
            def option_type?(type)
              return false unless type

              base_name = case type
                          when MLC::SemanticIR::GenericType
                            extract_type_name(type.base_type)
                          when MLC::SemanticIR::Type
                            type.name
                          end

              return false unless base_name == "Option"

              # If Option is defined in the type registry, it's a user-defined sum type
              # User-defined sum types use std::variant, not std::optional
              return false if type_registry&.has_type?("Option")

              # Otherwise it's stdlib Option which maps to std::optional
              true
            end

            # Extract type name from various type representations
            def extract_type_name(type)
              case type
              when MLC::SemanticIR::Type
                type.name
              when MLC::SemanticIR::GenericType
                extract_type_name(type.base_type)
              else
                type.to_s
              end
            end

            private

            # Resolve the C++ type name for a variant in a generic sum type.
            # For GenericType scrutinee (e.g. Result<i32, string>), returns
            # namespace-qualified template type (e.g. "mlc::result::Ok<int>").
            # Returns nil for non-generic or non-namespaced types.
            def resolve_variant_cpp_type(variant_name, scrutinee_type)
              return nil unless scrutinee_type.is_a?(MLC::SemanticIR::GenericType)

              base_name = extract_type_name(scrutinee_type.base_type)
              return nil unless base_name

              type_info = context.type_registry&.lookup(base_name)
              return nil unless type_info

              ns = type_info.namespace

              # Find which variant index this name corresponds to
              variants = type_info.core_ir_type.respond_to?(:variants) ? type_info.core_ir_type.variants : []
              variant = variants.find { |v| v[:name] == variant_name }
              return nil unless variant

              type_args = scrutinee_type.type_args

              # Build template args using only this variant's own type params (per-variant params).
              sum_type = type_info.core_ir_type
              all_param_names = collect_sum_type_var_names(sum_type)
              used_names = collect_variant_type_var_names(variant[:fields] || [])
              qualified = ns ? "#{ns}::#{variant_name}" : variant_name
              if used_names.any?
                cpp_args = used_names.map do |vname|
                  idx = all_param_names.index(vname)
                  idx ? context.map_type(type_args[idx]) : vname
                end
                "#{qualified}<#{cpp_args.join(", ")}>"
              else
                qualified
              end
            end

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

              # Create IIFE lambda: [&]() -> ReturnType { ... }()
              ret_type = nil
              if match_expr.respond_to?(:type) && match_expr.type
                mapped = context.map_type(match_expr.type)
                ret_type = mapped unless mapped.nil? || mapped.empty? || mapped == "auto" || mapped == "int" || mapped == "bool" || mapped == "float" || mapped == "double" || mapped.include?("function<")
              end
              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: "",
                return_type: ret_type
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
              pattern_string = build_mlc_string(regex_pattern)
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
                # Generate: if (auto match_opt = regex.match(text)) { auto match = *match_opt; ... return body; }

                regex_src = regex_obj.to_source
                scrutinee_src = scrutinee.to_source
                body_src = body.to_source

                # Build capture variable declarations
                capture_decls = []
                bindings.each_with_index do |binding, idx|
                  next if binding == "_" # Skip wildcards

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
            def lower_match_arm(arm, scrutinee_type: nil, return_type: nil)
              pattern = arm[:pattern]
              body = context.lower_expression(arm[:body])

              case pattern[:kind]
              when :constructor
                # Generate MatchArm with constructor pattern
                case_name = pattern[:name]
                bindings = pattern[:bindings] || pattern[:fields] || []

                cpp_param_type = resolve_variant_cpp_type(case_name, scrutinee_type)

                sanitized_bindings = uniquify_wildcard_bindings(bindings)

                CppAst::Nodes::MatchArm.new(
                  case_name: case_name,
                  bindings: sanitized_bindings,
                  body: body,
                  cpp_param_type: cpp_param_type,
                  return_type: return_type
                )

              when :wildcard, :var
                # For wildcard or variable patterns, generate catch-all arm
                var_name = pattern[:kind] == :var ? pattern[:name] : "_unused"

                CppAst::Nodes::WildcardMatchArm.new(
                  var_name: var_name,
                  body: body,
                  return_type: return_type
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
            def build_mlc_string(value)
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
            def qualified_case_name(case_name, scrutinee_type)
              return case_name unless scrutinee_type

              base = shared_type?(scrutinee_type) ? scrutinee_type.type_args&.first : scrutinee_type
              base_name = base ? extract_type_name(base) : nil
              return case_name unless base_name

              info = context.type_registry&.lookup(base_name)
              return case_name unless info

              ns = info.namespace
              ns ||= info.cpp_name[/\A([^:]+)::/, 1] if info.cpp_name&.include?("::")
              return case_name unless ns

              "#{ns}::#{case_name}"
            end

            def variant_src_for(scrutinee_src, scrutinee_type)
              return scrutinee_src unless scrutinee_type

              needs_star = shared_type?(scrutinee_type) ||
                           ::MLC::Backends::Cpp::MatchScrutineeDeref.ast_sum_needs_star?(
                             context.type_registry, scrutinee_type
                           )
              base = needs_star ? "(*#{scrutinee_src})" : scrutinee_src
              inner = shared_type?(scrutinee_type) ? scrutinee_type.type_args&.first : scrutinee_type
              inner_name = inner ? extract_type_name(inner) : nil
              inner_name && context.cyclic_sum_types.include?(inner_name) ? "#{base}._" : base
            end

            def lower_match_with_guards(match_expr, scrutinee)
              statements = []
              scrutinee_src = scrutinee.to_source
              sn = match_expr.scrutinee
              scrutinee_type = sn&.type
              if sn && context.checker.var_expr?(sn)
                vt = context.lookup_var_type(sn.name)
                scrutinee_type = vt unless vt.nil?
              end
              variant_src = if sn.is_a?(MLC::SemanticIR::CallExpr) &&
                               sn.callee.is_a?(MLC::SemanticIR::VarExpr) &&
                               MATCH_SCRUTINEE_SHARED_CALLS.include?(sn.callee.name)
                              "(*#{scrutinee_src})"
                            else
                              variant_src_for(scrutinee_src, scrutinee_type)
                            end

              match_expr.arms.each do |arm|
                pattern = arm[:pattern]
                guard = arm[:guard]
                body = context.lower_expression(arm[:body])
                body_src = body&.to_source
                body_src = ";" if body_src.nil? || body_src.empty?

                case pattern[:kind]
                when :constructor
                  statements << build_constructor_guard_arm(pattern, guard, body_src, scrutinee_src, scrutinee_type)

                when :or
                  statements << build_or_pattern_arm(pattern, guard, body_src, variant_src, scrutinee_type)

                when :literal
                  condition = build_pattern_condition(pattern, scrutinee_src, scrutinee_type)
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

                when :array
                  # Array pattern: check size and elements
                  statements << build_array_pattern_arm(pattern, guard, body_src, scrutinee_src)

                when :tuple
                  # Tuple pattern: destructure with std::get<N>
                  statements << build_tuple_pattern_arm(pattern, guard, body_src, scrutinee_src)

                when :wildcard, :var
                  # Wildcard/var with optional guard
                  var_name = pattern[:kind] == :var ? pattern[:name] : nil
                  binding_str = (var_name && var_name != "_") ? "auto #{var_name} = #{scrutinee_src}; " : ""
                  if guard
                    guard_expr = context.lower_expression(guard)
                    guard_src = guard_expr.to_source
                    statements << context.factory.raw_statement(
                      code: "{ #{binding_str}if (#{guard_src}) { return #{body_src}; } }"
                    )
                  else
                    # Final wildcard/var - declare binding if needed, then return
                    if binding_str.empty?
                      statements << context.factory.return_statement(expression: body)
                    else
                      statements << context.factory.raw_statement(
                        code: "{ #{binding_str}return #{body_src}; }"
                      )
                    end
                  end

                else
                  # Unknown pattern kind - treat as wildcard
                  statements << context.factory.return_statement(expression: body)
                end
              end

              # Build IIFE with explicit return type so `return {}` resolves correctly.
              body_str = statements.map(&:to_source).join(" ")
              ret_type = nil
              if match_expr.respond_to?(:type) && match_expr.type
                mapped = context.map_type(match_expr.type)
                ret_type = mapped unless mapped.nil? || mapped.empty? || mapped == "auto" || mapped == "int" || mapped == "bool" || mapped == "float" || mapped == "double" || mapped.include?("function<")
              end
              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: "",
                return_type: ret_type
              )

              context.factory.function_call(
                callee: lambda_expr,
                arguments: [],
                argument_separators: []
              )
            end

            # Build if statement for constructor pattern with optional guard
            def build_constructor_guard_arm(pattern, guard, body_src, scrutinee_src, scrutinee_type = nil)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []

              # Special handling for Option patterns (Some/None) only if scrutinee is Option<T> (generic)
              # User-defined "type Option = Some(x) | None" should use std::variant handling
              if ["Some", "None"].include?(case_name) && option_type?(scrutinee_type)
                return build_option_pattern_arm(case_name, bindings, guard, body_src, scrutinee_src)
              end

              variant_src = variant_src_for(scrutinee_src, scrutinee_type)
              qcase = qualified_case_name(case_name, scrutinee_type)

              holds_check = "std::holds_alternative<#{qcase}>(#{variant_src})"
              binding_decls, nested_checks = build_binding_extractions(case_name, bindings, variant_src, scrutinee_type)

              binding_str = binding_decls.join(" ")

              # Build the inner body with nested checks
              inner_return = if guard
                               guard_src = context.lower_expression(guard).to_source
                               "if (#{guard_src}) { return #{body_src}; }"
                             else
                               "return #{body_src};"
                             end

              if nested_checks.any?
                nested_body = nested_checks.reverse.reduce(inner_return) do |acc, check|
                  "if (#{check[:condition]}) { #{check[:bindings]} #{acc} }"
                end
                inner_body = "#{binding_str} #{nested_body}"
              else
                inner_body = "#{binding_str} #{inner_return}"
              end

              context.factory.raw_statement(
                code: "if (#{holds_check}) { #{inner_body} }"
              )
            end

            def build_binding_extractions(case_name, bindings, scrutinee_src, scrutinee_type = nil)
              return [[], []] if bindings.empty?

              binding_decls = []
              nested_checks = []
              temp_var_counter = 0
              qcase = qualified_case_name(case_name, scrutinee_type)

              temp_var = "_v_#{case_name.downcase}"
              binding_decls << "auto #{temp_var} = std::get<#{qcase}>(#{scrutinee_src});"

              non_wildcard_bindings = bindings.reject { |b| b == "_" || (b.is_a?(Hash) && b[:kind] == :constructor) }
              binding_decls << structured_binding_decl(bindings, temp_var) if non_wildcard_bindings.any?

              bindings.each_with_index do |binding, idx|
                next unless binding.is_a?(Hash) && binding[:kind] == :constructor

                nested_temp_var = "_nested_#{temp_var_counter}"
                temp_var_counter += 1

                binding_decls << nested_binding_extract(bindings, temp_var, nested_temp_var, idx)
                nested_checks << build_nested_pattern_check(binding, nested_temp_var)
              end

              [binding_decls, nested_checks]
            end

            def structured_binding_decl(bindings, temp_var)
              sanitized = uniquify_wildcard_bindings(bindings.map { |b| b.is_a?(Hash) ? "_" : b })
              binding_list = sanitized.join(", ")
              "auto [#{binding_list}] = #{temp_var};"
            end

            def nested_binding_extract(bindings, temp_var, nested_temp_var, idx)
              if bindings.length == 1
                # Single field - use .field0
                "auto #{nested_temp_var} = #{temp_var}.field0;"
              else
                # Multiple fields - use .fieldN
                "auto #{nested_temp_var} = #{temp_var}.field#{idx};"
              end
            end

            # Build if statement for Option pattern (Some/None) with std::optional
            # Some(x) -> if (opt.has_value()) { auto x = *opt; ... }
            # None -> if (!opt.has_value()) { ... }
            def build_option_pattern_arm(case_name, bindings, guard, body_src, scrutinee_src)
              holds_check = option_holds_check(case_name, scrutinee_src)
              binding_str = option_bindings(case_name, bindings, scrutinee_src)
              inner_body = option_inner_body(binding_str, guard, body_src)

              context.factory.raw_statement(
                code: "if (#{holds_check}) { #{inner_body} }"
              )
            end

            def option_holds_check(case_name, scrutinee_src)
              case_name == "Some" ? "#{scrutinee_src}.has_value()" : "!#{scrutinee_src}.has_value()"
            end

            def option_bindings(case_name, bindings, scrutinee_src)
              return "" unless case_name == "Some" && bindings.any?

              binding_decls = bindings.filter_map do |binding|
                next if binding == "_"
                next if binding.is_a?(Hash) && binding[:kind] == :constructor

                "auto #{binding} = *#{scrutinee_src};"
              end

              binding_decls.join(" ")
            end

            def option_inner_body(binding_str, guard, body_src)
              guard_src = context.lower_expression(guard).to_source if guard

              return "#{binding_str} return #{body_src};" unless guard_src

              "#{binding_str} if (#{guard_src}) { return #{body_src}; }"
            end

            # Build nested pattern check for nested constructor patterns
            def build_nested_pattern_check(pattern, scrutinee_var)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []

              condition = "std::holds_alternative<#{case_name}>(#{scrutinee_var})"

              binding_decls = nested_binding_decls(case_name, bindings, scrutinee_var)

              { condition: condition, bindings: binding_decls.join(" ") }
            end

            def nested_binding_decls(case_name, bindings, scrutinee_var)
              decls = []
              temp_var = "_v_nested_#{case_name.downcase}"
              decls << "auto #{temp_var} = std::get<#{case_name}>(#{scrutinee_var});"

              non_wildcard_bindings = bindings.reject { |b| b == "_" || b.is_a?(Hash) }
              if non_wildcard_bindings.any?
                binding_list = uniquify_wildcard_bindings(bindings.map { |b| b.is_a?(Hash) ? "_" : b }).join(", ")
                decls << "auto [#{binding_list}] = #{temp_var};"
              end

              decls
            end

            def build_or_pattern_arm(pattern, guard, body_src, scrutinee_src, scrutinee_type = nil)
              alternatives = pattern[:alternatives] || []

              conditions = alternatives.map do |alt|
                build_pattern_condition(alt, scrutinee_src, scrutinee_type)
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

            # Build if statement for array pattern with optional guard
            # Generates: if (array.size() == N) { auto e0 = array[0]; ... return body; }
            def build_array_pattern_arm(pattern, guard, body_src, scrutinee_src)
              elements = pattern[:elements] || []

              # Build size check condition
              size_check = "#{scrutinee_src}.size() == #{elements.length}"

              # Build element bindings and checks
              bindings = []
              element_checks = []

              elements.each_with_index do |elem_pattern, index|
                case elem_pattern[:kind]
                when :var
                  # Variable binding: auto x = array[index];
                  var_name = elem_pattern[:name]
                  bindings << "auto #{var_name} = #{scrutinee_src}[#{index}];"
                when :wildcard
                  # Wildcard: no binding needed
                  next
                when :literal
                  # Literal: check equality
                  value = elem_pattern[:value]
                  literal_src = format_literal_value(value)
                  element_checks << "#{scrutinee_src}[#{index}] == #{literal_src}"
                else
                  # Other patterns: extract to temp var and recursively check
                  temp_var = "_elem_#{index}"
                  bindings << "auto #{temp_var} = #{scrutinee_src}[#{index}];"
                  # For nested patterns, would need recursive handling
                  # For now, treat as wildcard
                end
              end

              # Combine all conditions
              all_checks = [size_check] + element_checks
              condition = all_checks.join(" && ")

              # Build inner body with bindings
              if guard
                guard_expr = context.lower_expression(guard)
                guard_src = guard_expr.to_source
                inner_body = "#{bindings.join(' ')} if (#{guard_src}) { return #{body_src}; }"
              else
                inner_body = "#{bindings.join(' ')} return #{body_src};"
              end

              context.factory.raw_statement(
                code: "if (#{condition}) { #{inner_body} }"
              )
            end

            # Build if statement for tuple pattern with optional guard
            # Generates: { auto x = std::get<0>(tuple); auto y = std::get<1>(tuple); ... return body; }
            # For tuples, there's no runtime check needed - just extract elements
            def build_tuple_pattern_arm(pattern, guard, body_src, scrutinee_src)
              elements = pattern[:elements] || []

              # Build element bindings using std::get<N>
              bindings = []
              element_checks = []

              elements.each_with_index do |elem_pattern, index|
                case elem_pattern[:kind]
                when :var
                  # Variable binding: auto x = std::get<N>(tuple);
                  var_name = elem_pattern[:name]
                  bindings << "auto #{var_name} = std::get<#{index}>(#{scrutinee_src});"
                when :wildcard
                  # Wildcard: no binding needed
                  next
                when :literal
                  # Literal: check equality
                  value = elem_pattern[:value]
                  literal_src = format_literal_value(value)
                  element_checks << "std::get<#{index}>(#{scrutinee_src}) == #{literal_src}"
                else
                  # Other patterns: extract to temp var
                  temp_var = "_elem_#{index}"
                  bindings << "auto #{temp_var} = std::get<#{index}>(#{scrutinee_src});"
                end
              end

              # Build inner body with bindings
              if element_checks.any?
                # Has literal checks - wrap in if
                condition = element_checks.join(" && ")
                if guard
                  guard_expr = context.lower_expression(guard)
                  guard_src = guard_expr.to_source
                  inner_body = "#{bindings.join(' ')} if (#{guard_src}) { return #{body_src}; }"
                else
                  inner_body = "#{bindings.join(' ')} return #{body_src};"
                end
                context.factory.raw_statement(
                  code: "if (#{condition}) { #{inner_body} }"
                )
              elsif guard
                guard_expr = context.lower_expression(guard)
                guard_src = guard_expr.to_source
                # Tuple always matches, just check guard
                context.factory.raw_statement(
                  code: "{ #{bindings.join(' ')} if (#{guard_src}) { return #{body_src}; } }"
                )
              else
                # Tuple always matches and no guard - just return
                context.factory.raw_statement(
                  code: "{ #{bindings.join(' ')} return #{body_src}; }"
                )
              end
            end

            def build_pattern_condition(pattern, scrutinee_src, scrutinee_type = nil)
              case pattern[:kind]
              when :constructor
                case_name = pattern[:name]
                qcase = qualified_case_name(case_name, scrutinee_type)
                "std::holds_alternative<#{qcase}>(#{scrutinee_src})"
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

            # Collect TypeVariable names from all variants of a SumType in declaration order.
            def collect_sum_type_var_names(sum_type)
              seen = []
              sum_type.variants.each do |v|
                collect_variant_type_var_names_into(v[:fields] || [], seen)
              end
              seen
            end

            # Collect TypeVariable names used by a specific variant's fields.
            def collect_variant_type_var_names(fields)
              seen = []
              collect_variant_type_var_names_into(fields, seen)
              seen
            end

            def collect_variant_type_var_names_into(fields, seen)
              Array(fields).each do |f|
                collect_type_var_names_from_type(f[:type], seen) if f[:type]
              end
            end

            def collect_type_var_names_from_type(type, seen)
              if type.is_a?(MLC::SemanticIR::TypeVariable)
                seen << type.name unless seen.include?(type.name)
              elsif type.respond_to?(:type_args)
                type.type_args.each { |a| collect_type_var_names_from_type(a, seen) }
              end
            end
          end
        end
      end
    end
  end
end

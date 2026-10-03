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
              is_cyclic_option = !context.cyclic_option_sum_name(node.scrutinee&.type).nil?

              if has_regex
                # Generate if-else chain for regex matching
                lower_match_with_regex(node, scrutinee)
              elsif has_guards || has_nested || has_wildcard || has_literal || has_array || has_tuple || is_option_type || is_cyclic_option
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
                field.is_a?(Hash) && %i[constructor array].include?(field[:kind])
              end
            end

            # Value arms: `return expr;`. Unit if / void arms: C++ statement (no return).
            def lower_match_arm_body_return(body)
              code = if context.checker.block_expr?(body) && void_match_arm_block?(body)
                       lower_unit_block_expr_as_statement(body)
                     elsif context.should_lower_as_statement?(body)
                       context.lower_statement(body).to_source.strip
                     else
                       expr = context.lower_expression(body)
                       src = expr&.to_source
                       src = ";" if src.nil? || src.empty?
                       "return #{src};"
                     end
              normalize_match_arm_body_code(code)
            end

            def normalize_match_arm_body_code(code)
              return code if code.empty?
              return code if code.start_with?("return ") || code.end_with?(";")

              "#{code};"
            end

            def arm_body_for_iife(match_expr, body)
              code = lower_match_arm_body_return(body)
              return code unless void_iife_return_type?(match_expr)

              terminate_void_iife_arm(code)
            end

            def void_iife_return_type?(match_expr)
              match_iife_return_type(match_expr) == "void"
            end

            def match_iife_return_type(match_expr)
              return nil unless match_expr.respond_to?(:type) && match_expr.type

              mapped = context.map_type(match_expr.type)
              return nil if mapped.nil? || mapped.empty? || mapped == "auto"
              return nil if %w[int bool float double].include?(mapped)
              return nil if mapped.include?("function<")

              mapped
            end

            def terminate_void_iife_arm(code)
              stripped = code.strip
              return stripped if stripped.empty?
              return stripped if stripped.start_with?("return ")
              return stripped if stripped.end_with?("return;", "return")

              "#{stripped} return;"
            end

            def void_match_arm_block?(block_expr)
              return true if block_expr.type.is_a?(::MLC::SemanticIR::UnitType)
              return false unless block_expr.result

              void_match_arm_result?(block_expr.result)
            end

            def void_match_arm_result?(expression)
              return true if noop_match_arm_result?(expression)
              return true if context.should_lower_as_statement?(expression)
              return true if expression.respond_to?(:type) &&
                             expression.type.is_a?(::MLC::SemanticIR::UnitType)

              false
            end

            def noop_match_arm_result?(expression)
              return true if context.checker.unit_literal?(expression)
              return true if expression.is_a?(::MLC::SemanticIR::TupleExpr) && expression.elements.empty?

              false
            end

            def lower_unit_block_expr_as_statement(block_expr)
              parts = block_expr.statements.map { |statement| context.lower_statement(statement).to_source.strip }
              if block_expr.result && !noop_match_arm_result?(block_expr.result)
                parts << lower_match_arm_result_statement(block_expr.result)
              end
              parts.reject(&:empty?).join(" ")
            end

            def lower_match_arm_result_statement(expression)
              if context.should_lower_as_statement?(expression)
                context.lower_statement(expression).to_source.strip
              else
                context.lower_expression(expression).to_source.strip
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
              needs_star = is_shared || MatchScrutineeDeref.ast_sum_needs_star?(
                context.type_registry, scrutinee_type, scrutinee_node: scrutinee_node
              )
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
                arm_body_code = arm_body_for_iife(match_expr, arm[:body])

                case pattern[:kind]
                when :regex
                  body = context.lower_expression(arm[:body])
                  statements.concat(build_regex_arm_statements(pattern, body, scrutinee))

                when :wildcard, :var
                  statements << context.factory.raw_statement(code: arm_body_code)

                else
                  statements << context.factory.raw_statement(code: arm_body_code)
                end
              end

              build_match_iife(match_expr, statements)
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
                raw_name = case_name.downcase
                source_name = CppAst::Nodes::MatchArm::CPP_KEYWORDS.include?(raw_name) ? "#{raw_name}_" : raw_name

                CppAst::Nodes::MatchArm.new(
                  case_name: case_name,
                  bindings: sanitized_bindings,
                  body: body,
                  cpp_param_type: cpp_param_type,
                  return_type: return_type,
                  binding_prefix: payload_binding_prefix(case_name, sanitized_bindings, source_name)
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

            def variant_src_for(scrutinee_src, scrutinee_type, scrutinee_node: nil)
              return scrutinee_src unless scrutinee_type

              needs_star = shared_type?(scrutinee_type) ||
                           ::MLC::Backends::Cpp::MatchScrutineeDeref.ast_sum_needs_star?(
                             context.type_registry, scrutinee_type, scrutinee_node: scrutinee_node
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
                              variant_src_for(scrutinee_src, scrutinee_type, scrutinee_node: sn)
                            end

              match_expr.arms.each do |arm|
                pattern = arm[:pattern]
                guard = arm[:guard]
                arm_body_code = arm_body_for_iife(match_expr, arm[:body])

                case pattern[:kind]
                when :constructor
                  statements << build_constructor_guard_arm(pattern, guard, arm_body_code, scrutinee_src, scrutinee_type)

                when :or
                  statements << build_or_pattern_arm(pattern, guard, arm_body_code, variant_src, scrutinee_type)

                when :literal
                  condition = build_pattern_condition(pattern, scrutinee_src, scrutinee_type)
                  if guard
                    guard_expr = context.lower_expression(guard)
                    guard_src = guard_expr.to_source
                    statements << context.factory.raw_statement(
                      code: "if (#{condition} && #{guard_src}) { #{arm_body_code} }"
                    )
                  else
                    statements << context.factory.raw_statement(
                      code: "if (#{condition}) { #{arm_body_code} }"
                    )
                  end

                when :array
                  # Array pattern: check size and elements
                  statements << build_array_pattern_arm(pattern, guard, arm_body_code, scrutinee_src)

                when :tuple
                  # Tuple pattern: destructure with std::get<N>
                  statements << build_tuple_pattern_arm(pattern, guard, arm_body_code, scrutinee_src)

                when :wildcard, :var
                  # Wildcard/var with optional guard
                  var_name = pattern[:kind] == :var ? pattern[:name] : nil
                  binding_str = (var_name && var_name != "_") ? "auto #{var_name} = #{scrutinee_src}; " : ""
                  if guard
                    guard_expr = context.lower_expression(guard)
                    guard_src = guard_expr.to_source
                    statements << context.factory.raw_statement(
                      code: "{ #{binding_str}if (#{guard_src}) { #{arm_body_code} } }"
                    )
                  else
                    statements << context.factory.raw_statement(
                      code: binding_str.empty? ? arm_body_code : "{ #{binding_str}#{arm_body_code} }"
                    )
                  end

                else
                  # Unknown pattern kind - treat as wildcard
                  arm_body_code = arm_body_for_iife(match_expr, arm[:body])
                  statements << context.factory.raw_statement(code: arm_body_code)
                end
              end

              if context.cyclic_option_sum_name(scrutinee_type) || cyclic_option_nested_match?(match_expr) ||
                 cyclic_array_nested_match?(match_expr)
                statements << context.factory.raw_statement(code: "std::abort();")
              end

              build_match_iife(match_expr, statements)
            end

            def build_match_iife(match_expr, statements)
              body_str = statements.map(&:to_source).join(" ")
              ret_type = match_iife_return_type(match_expr)
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

            def next_variant_binding_name(case_name)
              @variant_binding_counts ||= Hash.new(0)
              @variant_binding_counts[case_name] += 1
              count = @variant_binding_counts[case_name]
              base_name = "_v_#{case_name.downcase}"
              count == 1 ? base_name : "#{base_name}_#{count}"
            end

            # Build if statement for constructor pattern with optional guard
            def build_constructor_guard_arm(pattern, guard, arm_body_code, scrutinee_src, scrutinee_type = nil)
              @variant_binding_counts = Hash.new(0)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []

              # Special handling for Option patterns (Some/None) only if scrutinee is Option<T> (generic)
              # User-defined "type Option = Some(x) | None" should use std::variant handling
              if ["Some", "None"].include?(case_name) && context.cyclic_option_sum_name(scrutinee_type)
                return build_cyclic_option_pattern_arm(case_name, bindings, guard, arm_body_code, scrutinee_src)
              end

              if ["Some", "None"].include?(case_name) && option_type?(scrutinee_type)
                return build_option_pattern_arm(case_name, bindings, guard, arm_body_code, scrutinee_src)
              end

              variant_src = variant_src_for(scrutinee_src, scrutinee_type)
              qcase = qualified_case_name(case_name, scrutinee_type)

              holds_check = "std::holds_alternative<#{qcase}>(#{variant_src})"
              binding_decls, nested_checks = build_binding_extractions(case_name, bindings, variant_src, scrutinee_type)

              binding_str = binding_decls.join(" ")

              # Build the inner body with nested checks
              inner_return = if guard
                               guard_src = context.lower_expression(guard).to_source
                               "if (#{guard_src}) { #{arm_body_code} }"
                             else
                               arm_body_code
                             end

              if nested_checks.any?
                nested_body = nested_checks.reverse.reduce(inner_return) do |acc, check|
                  apply_nested_pattern_check(check, acc)
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

              temp_var = next_variant_binding_name(case_name)
              binding_decls << "auto #{temp_var} = std::get<#{qcase}>(#{scrutinee_src});"

              non_wildcard_bindings = bindings.reject { |binding|
                binding == "_" || (binding.is_a?(Hash) && %i[constructor array].include?(binding[:kind]))
              }
              binding_decls << structured_binding_decl(case_name, bindings, temp_var) if non_wildcard_bindings.any?

              bindings.each_with_index do |binding, index|
                if binding.is_a?(Hash) && binding[:kind] == :array &&
                   cyclic_array_pattern_simple?(binding) &&
                   (sum_name = cyclic_array_field_sum_name(case_name, index))
                  member_name = payload_member_name(cyclic_variant_fields(case_name), index)
                  nested_checks << build_cyclic_array_pattern_check(binding, "#{temp_var}.#{member_name}", sum_name)
                  next
                end
                next unless binding.is_a?(Hash) && binding[:kind] == :constructor

                nested_temp_var = "_nested_#{temp_var_counter}"
                temp_var_counter += 1
                if cyclic_option_field_sum_name(case_name, index)
                  member_name = payload_member_name(cyclic_variant_fields(case_name), index)
                  binding_decls << "auto #{nested_temp_var} = #{temp_var}.#{member_name};"
                  nested_checks << build_cyclic_option_nested_check(binding, nested_temp_var)
                else
                  boxed_child = cyclic_boxed_indexes(case_name).include?(index)
                  binding_decls << nested_binding_extract(case_name, bindings, temp_var, nested_temp_var, index)
                  nested_checks << build_nested_pattern_check(binding, nested_temp_var, boxed_child)
                end
              end

              [binding_decls, nested_checks]
            end

            def structured_binding_decl(constructor_name, bindings, temp_var)
              prefix = payload_binding_prefix(constructor_name, bindings, temp_var)
              return prefix.strip if prefix

              sanitized = uniquify_wildcard_bindings(bindings.map { |binding| binding.is_a?(Hash) ? "_" : binding })
              binding_list = sanitized.join(", ")
              "auto [#{binding_list}] = #{temp_var};"
            end

            def nested_binding_extract(constructor_name, bindings, temp_var, nested_temp_var, index)
              if cyclic_boxed_indexes(constructor_name).include?(index)
                member_name = payload_member_name(cyclic_variant_fields(constructor_name), index)
                return "auto #{nested_temp_var} = *#{temp_var}.#{member_name};"
              end

              if bindings.length == 1
                "auto #{nested_temp_var} = #{temp_var}.field0;"
              else
                "auto #{nested_temp_var} = #{temp_var}.field#{index};"
              end
            end

            # Build if statement for Option pattern (Some/None) with std::optional
            # Some(x) -> if (opt.has_value()) { auto x = *opt; ... }
            # None -> if (!opt.has_value()) { ... }
            def build_option_pattern_arm(case_name, bindings, guard, arm_body_code, scrutinee_src)
              holds_check = option_holds_check(case_name, scrutinee_src)
              binding_str = option_bindings(case_name, bindings, scrutinee_src)
              inner_body = option_inner_body(binding_str, guard, arm_body_code)

              context.factory.raw_statement(
                code: "if (#{holds_check}) { #{inner_body} }"
              )
            end

            def build_cyclic_option_pattern_arm(case_name, bindings, guard, arm_body_code, scrutinee_src)
              holds_check = cyclic_option_holds_check(case_name, scrutinee_src)
              binding_str = cyclic_option_bindings(case_name, bindings, scrutinee_src)
              inner_body = option_inner_body(binding_str, guard, arm_body_code)

              context.factory.raw_statement(
                code: "if (#{holds_check}) { #{inner_body} }"
              )
            end

            def cyclic_option_holds_check(case_name, scrutinee_src)
              if case_name == "Some"
                "(#{scrutinee_src}.has_value() && *#{scrutinee_src})"
              else
                "!#{scrutinee_src}.has_value()"
              end
            end

            def cyclic_option_bindings(case_name, bindings, scrutinee_src)
              return "" unless case_name == "Some" && bindings.any?

              bindings.filter_map { |binding|
                next if binding == "_"
                next if binding.is_a?(Hash)

                "const auto& #{binding} = *(*#{scrutinee_src});"
              }.join(" ")
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

            def option_inner_body(binding_str, guard, arm_body_code)
              guard_src = context.lower_expression(guard).to_source if guard

              return "#{binding_str} #{arm_body_code}" unless guard_src

              "#{binding_str} if (#{guard_src}) { #{arm_body_code} }"
            end

            # Build nested pattern check for nested constructor patterns
            def build_nested_pattern_check(pattern, scrutinee_var, wrapper_scrutinee = false)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []
              member_suffix = wrapper_scrutinee ? "._" : ""
              variant_access = "#{scrutinee_var}#{member_suffix}"
              condition = "std::holds_alternative<#{case_name}>(#{variant_access})"
              binding_declarations, nested_checks = build_binding_extractions(case_name, bindings, variant_access)
              captured_bindings = binding_declarations.join(" ")
              check = { condition: condition, bindings: captured_bindings }
              if nested_checks.any?
                check[:bindings] = ""
                check[:wrap] = lambda { |body|
                  inner = nested_checks.reverse.reduce(body) do |accumulated, nested|
                    apply_nested_pattern_check(nested, accumulated)
                  end
                  "#{captured_bindings} #{inner}"
                }
              end
              check
            end

            def nested_binding_decls(case_name, bindings, scrutinee_var, member_suffix = "")
              decls = []
              temp_var = "_v_nested_#{case_name.downcase}"
              decls << "auto #{temp_var} = std::get<#{case_name}>(#{scrutinee_var}#{member_suffix});"

              non_wildcard_bindings = bindings.reject { |binding| binding == "_" || binding.is_a?(Hash) }
              if non_wildcard_bindings.any?
                prefix = payload_binding_prefix(case_name, bindings, temp_var)
                decls << if prefix
                           prefix.strip
                         else
                           binding_list = uniquify_wildcard_bindings(bindings.map { |binding| binding.is_a?(Hash) ? "_" : binding }).join(", ")
                           "auto [#{binding_list}] = #{temp_var};"
                         end
              end

              decls
            end

            def constructor_has_cyclic_array_field?(pattern)
              return false unless pattern.is_a?(Hash)

              if pattern[:kind] == :or
                return Array(pattern[:alternatives]).any? { |alternative| constructor_has_cyclic_array_field?(alternative) }
              end

              return false unless pattern[:kind] == :constructor

              bindings = pattern[:bindings] || pattern[:fields] || []
              bindings.each_with_index.any? do |binding, index|
                binding.is_a?(Hash) && binding[:kind] == :array &&
                  cyclic_array_pattern_simple?(binding) &&
                  cyclic_array_field_sum_name(pattern[:name], index)
              end
            end

            def cyclic_array_nested_match?(match_expr)
              match_expr.arms.any? { |arm| constructor_has_cyclic_array_field?(arm[:pattern]) }
            end

            def cyclic_option_nested_match?(match_expr)
              match_expr.arms.any? do |arm|
                pattern = arm[:pattern]
                next false unless pattern[:kind] == :constructor

                bindings = pattern[:bindings] || pattern[:fields] || []
                bindings.each_with_index.any? do |binding, index|
                  binding.is_a?(Hash) && binding[:kind] == :constructor &&
                    cyclic_option_field_sum_name(pattern[:name], index)
                end
              end
            end

            def cyclic_option_field_sum_name(constructor_name, index)
              fields = cyclic_variant_fields(constructor_name)
              return nil unless fields && fields[index]

              field_type = fields[index][:type]
              return nil unless field_type.is_a?(MLC::SemanticIR::GenericType)

              base = field_type.base_type.respond_to?(:name) ? field_type.base_type.name : nil
              return nil unless base == "Option" && field_type.type_args&.length == 1

              argument = field_type.type_args.first
              return nil unless argument.is_a?(MLC::SemanticIR::Type) &&
                                !argument.is_a?(MLC::SemanticIR::GenericType) &&
                                !argument.is_a?(MLC::SemanticIR::ArrayType) &&
                                Array(context.cyclic_sum_types).include?(argument.name)

              argument.name
            end

            def apply_nested_pattern_check(check, body)
              inner = check[:wrap] ? check[:wrap].call(body) : "#{check[:bindings]} #{body}"
              "if (#{check[:condition]}) { #{inner} }"
            end

            def cyclic_option_payload_constructor(bindings)
              return nil unless bindings.length == 1

              binding = bindings.first
              return nil unless binding.is_a?(Hash) && binding[:kind] == :constructor

              binding
            end

            def wrap_cyclic_payload_constructor(pattern, variant_access, body)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []
              holds = "std::holds_alternative<#{case_name}>(#{variant_access})"
              binding_declarations, nested_checks = build_binding_extractions(case_name, bindings, variant_access)
              inner = body
              if nested_checks.any?
                inner = nested_checks.reverse.reduce(inner) do |accumulated, check|
                  apply_nested_pattern_check(check, accumulated)
                end
              end
              "if (#{holds}) { #{binding_declarations.join(' ')} #{inner} }"
            end

            def build_cyclic_option_nested_check(pattern, scrutinee_var)
              case_name = pattern[:name]
              bindings = pattern[:bindings] || pattern[:fields] || []
              if case_name == "Some"
                condition = "(#{scrutinee_var}.has_value() && *#{scrutinee_var})"
                payload = cyclic_option_payload_constructor(bindings)
                if payload
                  {
                    condition: condition,
                    bindings: "",
                    wrap: lambda { |body|
                      wrap_cyclic_payload_constructor(payload, "(*(*#{scrutinee_var}))._", body)
                    }
                  }
                else
                  binding_text = bindings.filter_map { |binding|
                    next if binding == "_" || binding.is_a?(Hash)

                    "const auto& #{binding} = *(*#{scrutinee_var});"
                  }.join(" ")
                  { condition: condition, bindings: binding_text }
                end
              else
                { condition: "!#{scrutinee_var}.has_value()", bindings: "" }
              end
            end

            def cyclic_array_field_sum_name(constructor_name, index)
              fields = cyclic_variant_fields(constructor_name)
              return nil unless fields && fields[index]

              field_type = fields[index][:type]
              return nil unless field_type.is_a?(MLC::SemanticIR::ArrayType)

              element = field_type.element_type
              return nil unless element.is_a?(MLC::SemanticIR::Type) &&
                                !element.is_a?(MLC::SemanticIR::GenericType) &&
                                !element.is_a?(MLC::SemanticIR::ArrayType) &&
                                Array(context.cyclic_sum_types).include?(element.name)

              element.name
            end

            def cyclic_array_name_element?(element)
              return true if element == "_" || element.is_a?(String)
              return false unless element.is_a?(Hash)

              %i[wildcard var].include?(element[:kind])
            end

            def cyclic_array_nullary_constructor?(element)
              return false unless element.is_a?(Hash) && element[:kind] == :constructor

              (element[:fields] || element[:bindings] || []).empty?
            end

            def cyclic_array_constructor_pattern?(element)
              return false unless element.is_a?(Hash) && element[:kind] == :constructor

              fields = element[:fields] || element[:bindings] || []
              fields.all? { |field|
                cyclic_array_name_element?(field) ||
                  cyclic_array_constructor_pattern?(field) ||
                  (field.is_a?(Hash) && field[:kind] == :array && cyclic_array_pattern_simple?(field))
              }
            end

            def cyclic_array_field_constructor?(element)
              return false unless element.is_a?(Hash) && element[:kind] == :constructor

              fields = element[:fields] || element[:bindings] || []
              return false if fields.empty?

              cyclic_array_constructor_pattern?(element)
            end

            def cyclic_array_or_element?(element)
              return false unless element.is_a?(Hash) && element[:kind] == :or

              Array(element[:alternatives]).all? { |alternative|
                cyclic_array_name_element?(alternative) ||
                  cyclic_array_nullary_constructor?(alternative) ||
                  cyclic_array_field_constructor?(alternative)
              }
            end

            def wrap_cyclic_array_or_element(element, variant_access, sum_access, body)
              clauses = []
              Array(element[:alternatives]).each_with_index do |alternative, index|
                clause = if cyclic_array_nullary_constructor?(alternative)
                           "if (std::holds_alternative<#{alternative[:name]}>(#{variant_access})) { #{body} }"
                         elsif alternative.is_a?(Hash) && alternative[:kind] == :var && alternative[:name] != "_"
                           "if (true) { const auto& #{alternative[:name]} = #{sum_access}; #{body} }"
                         elsif cyclic_array_name_element?(alternative)
                           "if (true) { #{body} }"
                         else
                           wrap_cyclic_payload_constructor(alternative, variant_access, body)
                         end
                clauses << (index.zero? ? clause : clause.sub(/\Aif/, "else if"))
                break if cyclic_array_name_element?(alternative)
              end
              clauses.join(" ")
            end

            def cyclic_array_pattern_simple?(pattern)
              elements = Array(pattern[:elements])
              elements.all? { |element|
                cyclic_array_name_element?(element) ||
                  cyclic_array_nullary_constructor?(element) ||
                  cyclic_array_field_constructor?(element) ||
                  cyclic_array_or_element?(element)
              }
            end

            def build_cyclic_array_pattern_check(pattern, scrutinee_var, sum_name)
              elements = pattern[:elements] || []
              binding_text = elements.each_with_index.filter_map { |element, index|
                next unless element.is_a?(Hash) && element[:kind] == :var

                "const auto& #{element[:name]} = (*#{scrutinee_var}[#{index}]);"
              }.join(" ")
              rest_name = pattern[:rest]
              size_operator = rest_name && !rest_name.empty? ? ">=" : "=="
              if rest_name && !rest_name.empty?
                binding_text = "#{binding_text} const auto #{rest_name} = mlc::Array<std::shared_ptr<#{sum_name}>>(#{scrutinee_var}.cbegin() + #{elements.length}, #{scrutinee_var}.cend());"
              end
              conditions = ["#{scrutinee_var}.size() #{size_operator} #{elements.length}"]
              structured_elements = []
              elements.each_with_index do |element, index|
                if cyclic_array_nullary_constructor?(element)
                  conditions << "std::holds_alternative<#{element[:name]}>((*#{scrutinee_var}[#{index}])._)"
                elsif cyclic_array_field_constructor?(element)
                  structured_elements << [:constructor, element, index]
                elsif cyclic_array_or_element?(element)
                  structured_elements << [:or, element, index]
                end
              end
              check = { condition: conditions.join(" && "), bindings: binding_text }
              if structured_elements.any?
                captured_bindings = binding_text
                check[:bindings] = ""
                check[:wrap] = lambda { |body|
                  structured_elements.reverse.reduce("#{captured_bindings} #{body}") do |accumulated, (kind, element, index)|
                    variant_access = "(*#{scrutinee_var}[#{index}])._"
                    if kind == :or
                      wrap_cyclic_array_or_element(
                        element,
                        variant_access,
                        "(*#{scrutinee_var}[#{index}])",
                        accumulated
                      )
                    else
                      wrap_cyclic_payload_constructor(element, variant_access, accumulated)
                    end
                  end
                }
              end
              check
            end

            def cyclic_variant_fields(constructor_name)
              Array(context.cyclic_sum_types).each do |sum_name|
                sum_type = context.type_registry&.lookup(sum_name)&.core_ir_type
                next unless sum_type.respond_to?(:variants)

                variant = Array(sum_type.variants).find { |candidate| candidate[:name].to_s == constructor_name }
                return Array(variant[:fields]) if variant
              end
              nil
            end

            def cyclic_boxed_indexes(constructor_name)
              fields = cyclic_variant_fields(constructor_name)
              return [] unless fields

              names = Array(context.cyclic_sum_types)
              fields.each_index.select do |index|
                field_type = fields[index][:type]
                field_type.is_a?(MLC::SemanticIR::Type) &&
                  !field_type.is_a?(MLC::SemanticIR::GenericType) &&
                  names.include?(field_type.name)
              end
            end

            def payload_member_name(fields, index)
              field = fields && fields[index]
              name = field.is_a?(Hash) ? field[:name] : nil
              return "field#{index}" if name.nil? || name.to_s.empty?

              name.to_s
            end

            def payload_binding_prefix(constructor_name, bindings, source_name)
              boxed_indexes = cyclic_boxed_indexes(constructor_name)
              return nil if boxed_indexes.empty?

              fields = cyclic_variant_fields(constructor_name)
              sanitized = uniquify_wildcard_bindings(bindings.map { |binding| binding.is_a?(Hash) ? "_" : binding })
              sanitized.each_with_index.filter_map { |name, index|
                binding = bindings[index]
                next if binding.is_a?(Hash) && binding[:kind] == :constructor

                access = "#{source_name}.#{payload_member_name(fields, index)}"
                value = boxed_indexes.include?(index) ? "(*#{access})" : access
                "const auto& #{context.sanitize_identifier(name)} = #{value};"
              }.join(" ") + " "
            end

            def build_or_pattern_arm(pattern, guard, arm_body_code, scrutinee_src, scrutinee_type = nil)
              alternatives = pattern[:alternatives] || []

              has_payload = alternatives.any? do |alt|
                alt[:kind] == :constructor && (alt[:bindings] || alt[:fields] || []).reject { |b| b == "_" }.any?
              end

              unless has_payload
                conditions = alternatives.map do |alt|
                  build_pattern_condition(alt, scrutinee_src, scrutinee_type)
                end
                or_condition = conditions.join(" || ")
                inner_body = if guard
                               guard_src = context.lower_expression(guard).to_source
                               "if (#{guard_src}) { #{arm_body_code} }"
                             else
                               arm_body_code
                             end
                return context.factory.raw_statement(code: "if (#{or_condition}) { #{inner_body} }")
              end

              inner_return = if guard
                               guard_src = context.lower_expression(guard).to_source
                               "if (#{guard_src}) { #{arm_body_code} }"
                             else
                               arm_body_code
                             end

              branches = alternatives.map.with_index do |alt, idx|
                @variant_binding_counts = Hash.new(0)
                case_name = alt[:name]
                bindings = alt[:bindings] || alt[:fields] || []
                qcase = qualified_case_name(case_name, scrutinee_type)
                holds_check = "std::holds_alternative<#{qcase}>(#{scrutinee_src})"
                binding_decls, nested_checks = build_binding_extractions(case_name, bindings, scrutinee_src, scrutinee_type)
                inner = inner_return
                if nested_checks.any?
                  inner = nested_checks.reverse.reduce(inner) do |accumulated, check|
                    apply_nested_pattern_check(check, accumulated)
                  end
                end
                prefix = idx == 0 ? "if" : "} else if"
                "#{prefix} (#{holds_check}) { #{binding_decls.join(' ')} #{inner}"
              end

              context.factory.raw_statement(code: branches.join(" ") + " }")
            end

            # Build if statement for array pattern with optional guard
            # Generates: if (array.size() == N) { auto e0 = array[0]; ... return body; }
            def build_array_pattern_arm(pattern, guard, arm_body_code, scrutinee_src)
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
                inner_body = "#{bindings.join(' ')} if (#{guard_src}) { #{arm_body_code} }"
              else
                inner_body = "#{bindings.join(' ')} #{arm_body_code}"
              end

              context.factory.raw_statement(
                code: "if (#{condition}) { #{inner_body} }"
              )
            end

            # Build if statement for tuple pattern with optional guard
            # Generates: { auto x = std::get<0>(tuple); auto y = std::get<1>(tuple); ... return body; }
            # For tuples, there's no runtime check needed - just extract elements
            def build_tuple_pattern_arm(pattern, guard, arm_body_code, scrutinee_src)
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
                  inner_body = "#{bindings.join(' ')} if (#{guard_src}) { #{arm_body_code} }"
                else
                  inner_body = "#{bindings.join(' ')} #{arm_body_code}"
                end
                context.factory.raw_statement(
                  code: "if (#{condition}) { #{inner_body} }"
                )
              elsif guard
                guard_expr = context.lower_expression(guard)
                guard_src = guard_expr.to_source
                # Tuple always matches, just check guard
                context.factory.raw_statement(
                  code: "{ #{bindings.join(' ')} if (#{guard_src}) { #{arm_body_code} } }"
                )
              else
                context.factory.raw_statement(
                  code: "{ #{bindings.join(' ')} #{arm_body_code} }"
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

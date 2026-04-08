#include "expr.hpp"

namespace expr {

mlc::String parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept;

mlc::String parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept;

mlc::String ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept;

mlc::String index_subscript(mlc::String object_code, mlc::String index_code) noexcept;

mlc::String array_literal(mlc::String argument_list) noexcept;

mlc::String question_try_result(mlc::String inner_code) noexcept;

mlc::String block_as_immediate_invoked_function_expression(mlc::String body_statements_and_return) noexcept;

mlc::String while_loop_immediate_invoked_function_expression(mlc::String condition_code, mlc::String body_statements) noexcept;

mlc::String for_loop_immediate_invoked_function_expression(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept;

mlc::String std_visit_match_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept;

mlc::String match_arm_wild_or_unit_return(mlc::String return_expression_code) noexcept;

mlc::String match_arm_binding_identifier(mlc::String parameter_cpp_safe, mlc::String return_expression_code) noexcept;

mlc::String match_arm_constructed_value(mlc::String const_reference_parameter, mlc::String binding_prefix, mlc::String return_expression_code) noexcept;

mlc::String record_initializer(mlc::String type_resolved, mlc::String values_comma_separated) noexcept;

mlc::String record_update_lazy_closure(mlc::String initializer_and_assignments) noexcept;

mlc::String function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept;

mlc::String constructor_call_braces(mlc::String constructor_code, mlc::String argument_list) noexcept;

mlc::String field_access(mlc::String object_code, mlc::String access_operator, mlc::String field_cpp_safe) noexcept;

mlc::String lambda_with_return(mlc::String header_code, mlc::String return_expression_code) noexcept;

mlc::String assignment_statement(mlc::String left_code, mlc::String right_code) noexcept;

mlc::String expression_operation_statement(mlc::String operation, mlc::String left_code, mlc::String right_code) noexcept;

mlc::String suffix_semicolon_newline(mlc::String expression_code) noexcept;

mlc::String try_unwrap_result_block(mlc::String result_variable, mlc::String inner_code, mlc::String success_line) noexcept;

mlc::String auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept;

mlc::String constexpr_auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept;

mlc::String return_line(mlc::String expression_code) noexcept;

mlc::String if_brace_block(mlc::String condition_code, mlc::String inner_statements) noexcept;

mlc::String else_brace_block(mlc::String inner_statements) noexcept;

mlc::String else_fragment_raw(mlc::String raw_else_suffix) noexcept;

mlc::String while_loop_statement(mlc::String condition_code, mlc::String body_statements) noexcept;

mlc::String for_loop_statement(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept;

mlc::String break_line() noexcept;

mlc::String continue_line() noexcept;

mlc::String let_from_try_ok_field0(mlc::String binding_cpp_safe, mlc::String try_variable) noexcept;

mlc::String discard_try_ok_field0_statement(mlc::String try_variable) noexcept;

mlc::String return_try_ok_field0_statement(mlc::String try_variable) noexcept;

mlc::String if_always_true_block(mlc::String inner_statements) noexcept;

mlc::String fragment_with_newline(mlc::String fragment) noexcept;

mlc::String tuple_destructure_binding(mlc::String binding_names_csv, mlc::String source_name) noexcept;

mlc::String record_pattern_field_binding(mlc::String field_cpp_safe, mlc::String variant_lower_name) noexcept;

mlc::String record_update_base_prefix(mlc::String expression_code) noexcept;

mlc::String mutable_member_assignment(mlc::String receiver, mlc::String member_cpp_safe, mlc::String value_code) noexcept;

mlc::String pointer_dereference_parenthesized(mlc::String expression_code) noexcept;

mlc::String visit_subject_for_match(mlc::String expression_code, bool needs_dereference) noexcept;

mlc::String make_shared_call(mlc::String element_type, mlc::String argument_code) noexcept;

mlc::String runtime_to_string_call(mlc::String expression_code) noexcept;

mlc::String empty_map_initializer() noexcept;

mlc::String block_immediate_function_return_fragment(mlc::String return_expression_code) noexcept;

mlc::String append_trailing_expression_statement(mlc::String statements_code, mlc::String expression_code) noexcept;

mlc::String hash_map_empty_instantiation(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept;

mlc::String typed_array_empty(mlc::String element_type_cpp) noexcept;

mlc::String array_string_fallback_empty() noexcept;

mlc::String match_lambda_const_reference_parameter(mlc::String resolved_type_name, mlc::String generic_angle_bracket_argument, mlc::String binding_name_cpp_safe) noexcept;

mlc::String dot_member_access(mlc::String receiver_code, mlc::String member_code) noexcept;

mlc::String comma_separated_pair(mlc::String first_code, mlc::String second_code) noexcept;

mlc::String prefix_with_optional_suffix(mlc::String prefix, mlc::String optional_suffix) noexcept;

mlc::String main_program_parameter_list() noexcept;

mlc::String noexcept_function_prototype(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept;

mlc::String noexcept_function_body_open(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept;

mlc::String inline_noexcept_forwarding_call(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::String parameter_list, mlc::String callee_code, mlc::String call_argument_list) noexcept;

mlc::String noexcept_one_line_returning_function(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list, mlc::String return_expression_code) noexcept;

mlc::String parameter_declaration_item(mlc::String type_expression_cpp, mlc::String binding_name_cpp) noexcept;

mlc::String block_close_newline() noexcept;

mlc::String cpp_array_type_element(mlc::String inner_type_cpp) noexcept;

mlc::String cpp_shared_pointer_type(mlc::String inner_type_cpp) noexcept;

mlc::String cpp_template_single_type_argument(mlc::String template_name_cpp, mlc::String type_argument_cpp) noexcept;

mlc::String cpp_template_two_type_arguments(mlc::String template_name_cpp, mlc::String first_type_argument_cpp, mlc::String second_type_argument_cpp) noexcept;

mlc::String cpp_std_function_type(mlc::String return_type_cpp, mlc::String parameter_types_csv) noexcept;

mlc::String cpp_lvalue_reference_suffix(mlc::String inner_type_cpp) noexcept;

mlc::String struct_empty_definition(mlc::String resolved_struct_name) noexcept;

mlc::String struct_with_inline_members_definition(mlc::String resolved_struct_name, mlc::String member_declarations_concatenated) noexcept;

mlc::String struct_forward_declaration_line(mlc::String template_prefix_before_keyword, mlc::String resolved_struct_name) noexcept;

mlc::String variant_using_alias_definition_line(mlc::String template_prefix_before_using, mlc::String resolved_alias_name, mlc::String variant_alternatives_csv) noexcept;

mlc::String concept_requires_clause_line(mlc::String requires_predicates_joined) noexcept;

mlc::String concept_trait_constraint_on_type_parameter(mlc::String trait_name_cpp_safe, mlc::String type_parameter_name) noexcept;

mlc::String struct_tuple_field_declaration(mlc::String element_type_cpp, mlc::String field_index_text) noexcept;

mlc::String struct_named_field_declaration(mlc::String field_type_cpp, mlc::String field_name_cpp_safe) noexcept;

mlc::String include_guard_ifndef_line(mlc::String guard_macro) noexcept;

mlc::String include_guard_define_line(mlc::String guard_macro) noexcept;

mlc::String include_guard_endif_comment_line(mlc::String guard_macro) noexcept;

mlc::String namespace_open_line(mlc::String namespace_identifier) noexcept;

mlc::String namespace_close_comment_line(mlc::String namespace_identifier) noexcept;

mlc::String implementation_define_main_as_user_main_line() noexcept;

mlc::String implementation_include_quotefile_line(mlc::String header_filename) noexcept;

mlc::String bootstrap_host_main_calling_namespaced_user_main(mlc::String qualified_namespace) noexcept;

mlc::String cpp_template_typename_header_line(mlc::String template_parameter_list) noexcept;

mlc::String concept_requires_expression_method_returns_convertible(mlc::String method_name, mlc::String destination_type_cpp) noexcept;

mlc::String trait_concept_requires_definition_line(mlc::String template_header, mlc::String trait_cpp_safe, mlc::String self_parameter_name, mlc::String requires_expressions_semicolon_separated) noexcept;

mlc::String user_main_arguments_copy_into_runtime_statement() noexcept;

mlc::String static_assert_concept_for_type_line(mlc::String concept_cpp_safe, mlc::String type_cpp, mlc::String diagnostic_message_inside_cpp_string_literal) noexcept;

mlc::String parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept{return mlc::String("(") + left_code + mlc::String(" ") + operation + mlc::String(" ") + right_code + mlc::String(")");}

mlc::String parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept{return mlc::String("(") + operation + inner_code + mlc::String(")");}

mlc::String ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{return mlc::String("(") + condition_code + mlc::String(" ? ") + then_code + mlc::String(" : ") + else_code + mlc::String(")");}

mlc::String index_subscript(mlc::String object_code, mlc::String index_code) noexcept{return object_code + mlc::String("[") + index_code + mlc::String("]");}

mlc::String array_literal(mlc::String argument_list) noexcept{return argument_list.length() == 0 ? mlc::String("{}") : mlc::String("mlc::Array{") + argument_list + mlc::String("}");}

mlc::String question_try_result(mlc::String inner_code) noexcept{return mlc::String("({ auto __q = ") + inner_code + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })");}

mlc::String block_as_immediate_invoked_function_expression(mlc::String body_statements_and_return) noexcept{return mlc::String("[&]() {\n") + body_statements_and_return + mlc::String("}()");}

mlc::String while_loop_immediate_invoked_function_expression(mlc::String condition_code, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nwhile (") + condition_code + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String for_loop_immediate_invoked_function_expression(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nfor (auto ") + variable_cpp_safe + mlc::String(" : ") + iterator_code + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String std_visit_match_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept{return mlc::String("std::visit(overloaded{") + arm_lambdas + mlc::String("\n}, ") + visit_subject + mlc::String(")");}

mlc::String match_arm_wild_or_unit_return(mlc::String return_expression_code) noexcept{return mlc::String("[&](const auto& __v) { return ") + return_expression_code + mlc::String("; }");}

mlc::String match_arm_binding_identifier(mlc::String parameter_cpp_safe, mlc::String return_expression_code) noexcept{return mlc::String("[&](const auto& ") + parameter_cpp_safe + mlc::String(") { return ") + return_expression_code + mlc::String("; }");}

mlc::String match_arm_constructed_value(mlc::String const_reference_parameter, mlc::String binding_prefix, mlc::String return_expression_code) noexcept{return mlc::String("[&](") + const_reference_parameter + mlc::String(") { ") + binding_prefix + mlc::String("return ") + return_expression_code + mlc::String("; }");}

mlc::String record_initializer(mlc::String type_resolved, mlc::String values_comma_separated) noexcept{return type_resolved + mlc::String("{") + values_comma_separated + mlc::String("}");}

mlc::String record_update_lazy_closure(mlc::String initializer_and_assignments) noexcept{return mlc::String("[&]() { ") + initializer_and_assignments + mlc::String("return __upd; }()");}

mlc::String function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept{return callee_code + mlc::String("(") + argument_list + mlc::String(")");}

mlc::String constructor_call_braces(mlc::String constructor_code, mlc::String argument_list) noexcept{return constructor_code + mlc::String("{") + argument_list + mlc::String("}");}

mlc::String field_access(mlc::String object_code, mlc::String access_operator, mlc::String field_cpp_safe) noexcept{return object_code + access_operator + field_cpp_safe;}

mlc::String lambda_with_return(mlc::String header_code, mlc::String return_expression_code) noexcept{return header_code + mlc::String(" { return ") + return_expression_code + mlc::String("; }");}

mlc::String assignment_statement(mlc::String left_code, mlc::String right_code) noexcept{return left_code + mlc::String(" = ") + right_code + mlc::String(";\n");}

mlc::String expression_operation_statement(mlc::String operation, mlc::String left_code, mlc::String right_code) noexcept{return mlc::String("(") + left_code + mlc::String(" ") + operation + mlc::String(" ") + right_code + mlc::String(");\n");}

mlc::String suffix_semicolon_newline(mlc::String expression_code) noexcept{return expression_code + mlc::String(";\n");}

mlc::String try_unwrap_result_block(mlc::String result_variable, mlc::String inner_code, mlc::String success_line) noexcept{return mlc::String("auto ") + result_variable + mlc::String(" = ") + inner_code + mlc::String(";\n") + mlc::String("if (std::get_if<1>(&") + result_variable + mlc::String(")) return *std::get_if<1>(&") + result_variable + mlc::String(");\n") + success_line;}

mlc::String auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept{return mlc::String("auto ") + binding_cpp_safe + mlc::String(" = ") + value_code + mlc::String(";\n");}

mlc::String constexpr_auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept{return mlc::String("constexpr auto ") + binding_cpp_safe + mlc::String(" = ") + value_code + mlc::String(";\n");}

mlc::String return_line(mlc::String expression_code) noexcept{return mlc::String("return ") + expression_code + mlc::String(";\n");}

mlc::String if_brace_block(mlc::String condition_code, mlc::String inner_statements) noexcept{return mlc::String("if (") + condition_code + mlc::String(") {\n") + inner_statements + mlc::String("}");}

mlc::String else_brace_block(mlc::String inner_statements) noexcept{return mlc::String(" else {\n") + inner_statements + mlc::String("}");}

mlc::String else_fragment_raw(mlc::String raw_else_suffix) noexcept{return mlc::String(" else ") + raw_else_suffix;}

mlc::String while_loop_statement(mlc::String condition_code, mlc::String body_statements) noexcept{return mlc::String("while (") + condition_code + mlc::String(") {\n") + body_statements + mlc::String("}\n");}

mlc::String for_loop_statement(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept{return mlc::String("for (auto ") + variable_cpp_safe + mlc::String(" : ") + iterator_code + mlc::String(") {\n") + body_statements + mlc::String("}\n");}

mlc::String break_line() noexcept{return mlc::String("break;\n");}

mlc::String continue_line() noexcept{return mlc::String("continue;\n");}

mlc::String let_from_try_ok_field0(mlc::String binding_cpp_safe, mlc::String try_variable) noexcept{return mlc::String("auto ") + binding_cpp_safe + mlc::String(" = std::get<0>(") + try_variable + mlc::String(").field0;\n");}

mlc::String discard_try_ok_field0_statement(mlc::String try_variable) noexcept{return mlc::String("/* discard */ (void)std::get<0>(") + try_variable + mlc::String(").field0;\n");}

mlc::String return_try_ok_field0_statement(mlc::String try_variable) noexcept{return mlc::String("return std::get<0>(") + try_variable + mlc::String(").field0;\n");}

mlc::String if_always_true_block(mlc::String inner_statements) noexcept{return mlc::String("if (true) {\n") + inner_statements + mlc::String("}\n");}

mlc::String fragment_with_newline(mlc::String fragment) noexcept{return fragment + mlc::String("\n");}

mlc::String tuple_destructure_binding(mlc::String binding_names_csv, mlc::String source_name) noexcept{return mlc::String("auto [") + binding_names_csv + mlc::String("] = ") + source_name + mlc::String("; ");}

mlc::String record_pattern_field_binding(mlc::String field_cpp_safe, mlc::String variant_lower_name) noexcept{return mlc::String("const auto& ") + field_cpp_safe + mlc::String(" = ") + variant_lower_name + mlc::String(".") + field_cpp_safe + mlc::String("; ");}

mlc::String record_update_base_prefix(mlc::String expression_code) noexcept{return mlc::String("auto __upd = ") + expression_code + mlc::String("; ");}

mlc::String mutable_member_assignment(mlc::String receiver, mlc::String member_cpp_safe, mlc::String value_code) noexcept{return receiver + mlc::String(".") + member_cpp_safe + mlc::String(" = ") + value_code + mlc::String("; ");}

mlc::String pointer_dereference_parenthesized(mlc::String expression_code) noexcept{return mlc::String("(*") + expression_code + mlc::String(")");}

mlc::String visit_subject_for_match(mlc::String expression_code, bool needs_dereference) noexcept{return needs_dereference ? pointer_dereference_parenthesized(expression_code) : expression_code;}

mlc::String make_shared_call(mlc::String element_type, mlc::String argument_code) noexcept{return mlc::String("std::make_shared<") + element_type + mlc::String(">(") + argument_code + mlc::String(")");}

mlc::String runtime_to_string_call(mlc::String expression_code) noexcept{return mlc::String("mlc::to_string(") + expression_code + mlc::String(")");}

mlc::String empty_map_initializer() noexcept{return mlc::String("{}");}

mlc::String block_immediate_function_return_fragment(mlc::String return_expression_code) noexcept{return mlc::String("return ") + return_expression_code + mlc::String(";\n");}

mlc::String append_trailing_expression_statement(mlc::String statements_code, mlc::String expression_code) noexcept{return statements_code + expression_code + mlc::String(";\n");}

mlc::String hash_map_empty_instantiation(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept{return mlc::String("mlc::HashMap<") + key_type_cpp + mlc::String(", ") + value_type_cpp + mlc::String(">()");}

mlc::String typed_array_empty(mlc::String element_type_cpp) noexcept{return mlc::String("mlc::Array<") + element_type_cpp + mlc::String(">{}");}

mlc::String array_string_fallback_empty() noexcept{return mlc::String("mlc::Array<mlc::String>{}");}

mlc::String match_lambda_const_reference_parameter(mlc::String resolved_type_name, mlc::String generic_angle_bracket_argument, mlc::String binding_name_cpp_safe) noexcept{return mlc::String("const ") + resolved_type_name + generic_angle_bracket_argument + mlc::String("& ") + binding_name_cpp_safe;}

mlc::String dot_member_access(mlc::String receiver_code, mlc::String member_code) noexcept{return receiver_code + mlc::String(".") + member_code;}

mlc::String comma_separated_pair(mlc::String first_code, mlc::String second_code) noexcept{return first_code + mlc::String(", ") + second_code;}

mlc::String prefix_with_optional_suffix(mlc::String prefix, mlc::String optional_suffix) noexcept{return optional_suffix.length() > 0 ? prefix + optional_suffix : prefix;}

mlc::String main_program_parameter_list() noexcept{return mlc::String("int argc, char** argv");}

mlc::String noexcept_function_prototype(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept{return return_type_cpp + mlc::String(" ") + name_cpp + mlc::String("(") + parameter_list + mlc::String(") noexcept;\n");}

mlc::String noexcept_function_body_open(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept{return return_type_cpp + mlc::String(" ") + name_cpp + mlc::String("(") + parameter_list + mlc::String(") noexcept{\n");}

mlc::String inline_noexcept_forwarding_call(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::String parameter_list, mlc::String callee_code, mlc::String call_argument_list) noexcept{return mlc::String("inline ") + return_type_cpp + mlc::String(" ") + method_name_cpp + mlc::String("(") + parameter_list + mlc::String(") noexcept { return ") + callee_code + mlc::String("(") + call_argument_list + mlc::String("); }\n");}

mlc::String noexcept_one_line_returning_function(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list, mlc::String return_expression_code) noexcept{return return_type_cpp + mlc::String(" ") + name_cpp + mlc::String("(") + parameter_list + mlc::String(") noexcept { return ") + return_expression_code + mlc::String("; }\n");}

mlc::String parameter_declaration_item(mlc::String type_expression_cpp, mlc::String binding_name_cpp) noexcept{return type_expression_cpp + mlc::String(" ") + binding_name_cpp;}

mlc::String block_close_newline() noexcept{return mlc::String("}\n");}

mlc::String cpp_array_type_element(mlc::String inner_type_cpp) noexcept{return mlc::String("mlc::Array<") + inner_type_cpp + mlc::String(">");}

mlc::String cpp_shared_pointer_type(mlc::String inner_type_cpp) noexcept{return mlc::String("std::shared_ptr<") + inner_type_cpp + mlc::String(">");}

mlc::String cpp_template_single_type_argument(mlc::String template_name_cpp, mlc::String type_argument_cpp) noexcept{return template_name_cpp + mlc::String("<") + type_argument_cpp + mlc::String(">");}

mlc::String cpp_template_two_type_arguments(mlc::String template_name_cpp, mlc::String first_type_argument_cpp, mlc::String second_type_argument_cpp) noexcept{return template_name_cpp + mlc::String("<") + comma_separated_pair(first_type_argument_cpp, second_type_argument_cpp) + mlc::String(">");}

mlc::String cpp_std_function_type(mlc::String return_type_cpp, mlc::String parameter_types_csv) noexcept{return mlc::String("std::function<") + return_type_cpp + mlc::String("(") + parameter_types_csv + mlc::String(")>");}

mlc::String cpp_lvalue_reference_suffix(mlc::String inner_type_cpp) noexcept{return inner_type_cpp + mlc::String("&");}

mlc::String struct_empty_definition(mlc::String resolved_struct_name) noexcept{return mlc::String("struct ") + resolved_struct_name + mlc::String(" {};\n");}

mlc::String struct_with_inline_members_definition(mlc::String resolved_struct_name, mlc::String member_declarations_concatenated) noexcept{return mlc::String("struct ") + resolved_struct_name + mlc::String(" {") + member_declarations_concatenated + mlc::String("};\n");}

mlc::String struct_forward_declaration_line(mlc::String template_prefix_before_keyword, mlc::String resolved_struct_name) noexcept{return template_prefix_before_keyword + mlc::String("struct ") + resolved_struct_name + mlc::String(";\n");}

mlc::String variant_using_alias_definition_line(mlc::String template_prefix_before_using, mlc::String resolved_alias_name, mlc::String variant_alternatives_csv) noexcept{return template_prefix_before_using + mlc::String("using ") + resolved_alias_name + mlc::String(" = std::variant<") + variant_alternatives_csv + mlc::String(">;\n");}

mlc::String concept_requires_clause_line(mlc::String requires_predicates_joined) noexcept{return mlc::String("requires ") + requires_predicates_joined + mlc::String("\n");}

mlc::String concept_trait_constraint_on_type_parameter(mlc::String trait_name_cpp_safe, mlc::String type_parameter_name) noexcept{return trait_name_cpp_safe + mlc::String("<") + type_parameter_name + mlc::String(">");}

mlc::String struct_tuple_field_declaration(mlc::String element_type_cpp, mlc::String field_index_text) noexcept{return element_type_cpp + mlc::String(" field") + field_index_text + mlc::String(";");}

mlc::String struct_named_field_declaration(mlc::String field_type_cpp, mlc::String field_name_cpp_safe) noexcept{return field_type_cpp + mlc::String(" ") + field_name_cpp_safe + mlc::String(";");}

mlc::String include_guard_ifndef_line(mlc::String guard_macro) noexcept{return mlc::String("#ifndef ") + guard_macro + mlc::String("\n");}

mlc::String include_guard_define_line(mlc::String guard_macro) noexcept{return mlc::String("#define ") + guard_macro + mlc::String("\n");}

mlc::String include_guard_endif_comment_line(mlc::String guard_macro) noexcept{return mlc::String("#endif // ") + guard_macro + mlc::String("\n");}

mlc::String namespace_open_line(mlc::String namespace_identifier) noexcept{return mlc::String("namespace ") + namespace_identifier + mlc::String(" {\n");}

mlc::String namespace_close_comment_line(mlc::String namespace_identifier) noexcept{return mlc::String("} // namespace ") + namespace_identifier + mlc::String("\n");}

mlc::String implementation_define_main_as_user_main_line() noexcept{return mlc::String("#define main mlc_user_main\n");}

mlc::String implementation_include_quotefile_line(mlc::String header_filename) noexcept{return mlc::String("#include \"") + header_filename + mlc::String("\"\n");}

mlc::String bootstrap_host_main_calling_namespaced_user_main(mlc::String qualified_namespace) noexcept{return mlc::String("\n#undef main\n\n") + mlc::String("static void mlc_cli_set_args(int argc, char** argv) {\n") + mlc::String("  std::vector<mlc::String> arguments;\n") + mlc::String("  arguments.reserve(argc > 0 ? argc - 1 : 0);\n") + mlc::String("  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n") + mlc::String("  mlc::io::set_args(std::move(arguments));\n") + mlc::String("}\n\n") + mlc::String("int main(int argc, char** argv) {\n") + mlc::String("  mlc_cli_set_args(argc, argv);\n") + mlc::String("  return ::") + qualified_namespace + mlc::String("::mlc_user_main(argc, argv);\n") + mlc::String("}\n");}

mlc::String cpp_template_typename_header_line(mlc::String template_parameter_list) noexcept{return mlc::String("template<typename ") + template_parameter_list + mlc::String(">\n");}

mlc::String concept_requires_expression_method_returns_convertible(mlc::String method_name, mlc::String destination_type_cpp) noexcept{return mlc::String("{ ") + method_name + mlc::String("(self) } -> std::convertible_to<") + destination_type_cpp + mlc::String(">");}

mlc::String trait_concept_requires_definition_line(mlc::String template_header, mlc::String trait_cpp_safe, mlc::String self_parameter_name, mlc::String requires_expressions_semicolon_separated) noexcept{return template_header + mlc::String("concept ") + trait_cpp_safe + mlc::String(" = requires(const ") + self_parameter_name + mlc::String("& self) { ") + requires_expressions_semicolon_separated + mlc::String("; };\n");}

mlc::String user_main_arguments_copy_into_runtime_statement() noexcept{return mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");}

mlc::String static_assert_concept_for_type_line(mlc::String concept_cpp_safe, mlc::String type_cpp, mlc::String diagnostic_message_inside_cpp_string_literal) noexcept{return mlc::String("static_assert(") + concept_cpp_safe + mlc::String("<") + type_cpp + mlc::String(">, \"") + diagnostic_message_inside_cpp_string_literal + mlc::String("\");\n");}

} // namespace expr

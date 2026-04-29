#define main mlc_user_main
#include "expr.hpp"

namespace expr {

mlc::String parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept{
return ((((((mlc::String("(", 1) + left_code) + mlc::String(" ", 1)) + operation) + mlc::String(" ", 1)) + right_code) + mlc::String(")", 1));
}
mlc::String parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept{
return (((mlc::String("(", 1) + operation) + inner_code) + mlc::String(")", 1));
}
mlc::String ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{
return ((((((mlc::String("(", 1) + condition_code) + mlc::String(" ? ", 3)) + then_code) + mlc::String(" : ", 3)) + else_code) + mlc::String(")", 1));
}
mlc::String index_subscript(mlc::String object_code, mlc::String index_code) noexcept{
return (((object_code + mlc::String("[", 1)) + index_code) + mlc::String("]", 1));
}
mlc::String array_literal(mlc::String argument_list) noexcept{
if ((argument_list.length() == 0)) {
return mlc::String("{}", 2);
} else {
return ((mlc::String("mlc::Array{", 11) + argument_list) + mlc::String("}", 1));
}
}
mlc::String question_try_result(mlc::String inner_code) noexcept{
return ((mlc::String("({ auto __q = ", 14) + inner_code) + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })", 85));
}
mlc::String block_as_immediate_invoked_function_expression(mlc::String body_statements_and_return) noexcept{
return ((mlc::String("[&]() {\n", 8) + body_statements_and_return) + mlc::String("}()", 3));
}
mlc::String while_loop_immediate_invoked_function_expression(mlc::String condition_code, mlc::String body_statements) noexcept{
return ((((mlc::String("[&]() {\nwhile (", 15) + condition_code) + mlc::String(") {\n", 4)) + body_statements) + mlc::String("}\n}()", 5));
}
mlc::String for_loop_immediate_invoked_function_expression(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept{
return ((((((mlc::String("[&]() {\nfor (auto ", 18) + variable_cpp_safe) + mlc::String(" : ", 3)) + iterator_code) + mlc::String(") {\n", 4)) + body_statements) + mlc::String("}\n}()", 5));
}
mlc::String std_visit_match_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept{
return ((((mlc::String("std::visit(overloaded{", 22) + arm_lambdas) + mlc::String("\n}, ", 4)) + visit_subject) + mlc::String(")", 1));
}
mlc::String match_arm_wild_or_unit_return(mlc::String return_expression_code) noexcept{
return ((mlc::String("[&](const auto& __v) { return ", 30) + return_expression_code) + mlc::String("; }", 3));
}
mlc::String match_arm_binding_identifier(mlc::String parameter_cpp_safe, mlc::String return_expression_code) noexcept{
return ((((mlc::String("[&](const auto& ", 16) + parameter_cpp_safe) + mlc::String(") { return ", 11)) + return_expression_code) + mlc::String("; }", 3));
}
mlc::String match_arm_constructed_value(mlc::String const_reference_parameter, mlc::String binding_prefix, mlc::String return_expression_code) noexcept{
return ((((((mlc::String("[&](", 4) + const_reference_parameter) + mlc::String(") { ", 4)) + binding_prefix) + mlc::String("return ", 7)) + return_expression_code) + mlc::String("; }", 3));
}
mlc::String record_initializer(mlc::String type_resolved, mlc::String values_comma_separated) noexcept{
return (((type_resolved + mlc::String("{", 1)) + values_comma_separated) + mlc::String("}", 1));
}
mlc::String record_update_lazy_closure(mlc::String initializer_and_assignments) noexcept{
return ((mlc::String("[&]() { ", 8) + initializer_and_assignments) + mlc::String("return __upd; }()", 17));
}
mlc::String function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept{
return (((callee_code + mlc::String("(", 1)) + argument_list) + mlc::String(")", 1));
}
mlc::String constructor_call_braces(mlc::String constructor_code, mlc::String argument_list) noexcept{
return (((constructor_code + mlc::String("{", 1)) + argument_list) + mlc::String("}", 1));
}
mlc::String field_access(mlc::String object_code, mlc::String access_operator, mlc::String field_cpp_safe) noexcept{
return ((object_code + access_operator) + field_cpp_safe);
}
mlc::String lambda_with_return(mlc::String header_code, mlc::String return_expression_code) noexcept{
return (((header_code + mlc::String(" { return ", 10)) + return_expression_code) + mlc::String("; }", 3));
}
mlc::String assignment_statement(mlc::String left_code, mlc::String right_code) noexcept{
return (((left_code + mlc::String(" = ", 3)) + right_code) + mlc::String(";\n", 2));
}
mlc::String expression_operation_statement(mlc::String operation, mlc::String left_code, mlc::String right_code) noexcept{
return ((((((mlc::String("(", 1) + left_code) + mlc::String(" ", 1)) + operation) + mlc::String(" ", 1)) + right_code) + mlc::String(");\n", 3));
}
mlc::String suffix_semicolon_newline(mlc::String expression_code) noexcept{
return (expression_code + mlc::String(";\n", 2));
}
mlc::String try_unwrap_result_block(mlc::String result_variable, mlc::String inner_code, mlc::String success_line) noexcept{
return ((((((((((mlc::String("auto ", 5) + result_variable) + mlc::String(" = ", 3)) + inner_code) + mlc::String(";\n", 2)) + mlc::String("if (std::get_if<1>(&", 20)) + result_variable) + mlc::String(")) return *std::get_if<1>(&", 27)) + result_variable) + mlc::String(");\n", 3)) + success_line);
}
mlc::String auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept{
return ((((mlc::String("auto ", 5) + binding_cpp_safe) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2));
}
mlc::String constexpr_auto_binding_statement(mlc::String binding_cpp_safe, mlc::String value_code) noexcept{
return ((((mlc::String("constexpr auto ", 15) + binding_cpp_safe) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2));
}
mlc::String return_line(mlc::String expression_code) noexcept{
return ((mlc::String("return ", 7) + expression_code) + mlc::String(";\n", 2));
}
mlc::String if_brace_block(mlc::String condition_code, mlc::String inner_statements) noexcept{
return ((((mlc::String("if (", 4) + condition_code) + mlc::String(") {\n", 4)) + inner_statements) + mlc::String("}", 1));
}
mlc::String else_brace_block(mlc::String inner_statements) noexcept{
return ((mlc::String(" else {\n", 8) + inner_statements) + mlc::String("}", 1));
}
mlc::String else_fragment_raw(mlc::String raw_else_suffix) noexcept{
return (mlc::String(" else ", 6) + raw_else_suffix);
}
mlc::String while_loop_statement(mlc::String condition_code, mlc::String body_statements) noexcept{
return ((((mlc::String("while (", 7) + condition_code) + mlc::String(") {\n", 4)) + body_statements) + mlc::String("}\n", 2));
}
mlc::String for_loop_statement(mlc::String variable_cpp_safe, mlc::String iterator_code, mlc::String body_statements) noexcept{
return ((((((mlc::String("for (auto ", 10) + variable_cpp_safe) + mlc::String(" : ", 3)) + iterator_code) + mlc::String(") {\n", 4)) + body_statements) + mlc::String("}\n", 2));
}
mlc::String break_line() noexcept{
return mlc::String("break;\n", 7);
}
mlc::String continue_line() noexcept{
return mlc::String("continue;\n", 10);
}
mlc::String let_from_try_ok_field0(mlc::String binding_cpp_safe, mlc::String try_variable) noexcept{
return ((((mlc::String("auto ", 5) + binding_cpp_safe) + mlc::String(" = std::get<0>(", 15)) + try_variable) + mlc::String(").field0;\n", 10));
}
mlc::String discard_try_ok_field0_statement(mlc::String try_variable) noexcept{
return ((mlc::String("/* discard */ (void)std::get<0>(", 32) + try_variable) + mlc::String(").field0;\n", 10));
}
mlc::String return_try_ok_field0_statement(mlc::String try_variable) noexcept{
return ((mlc::String("return std::get<0>(", 19) + try_variable) + mlc::String(").field0;\n", 10));
}
mlc::String if_always_true_block(mlc::String inner_statements) noexcept{
return ((mlc::String("if (true) {\n", 12) + inner_statements) + mlc::String("}\n", 2));
}
mlc::String fragment_with_newline(mlc::String fragment) noexcept{
return (fragment + mlc::String("\n", 1));
}
mlc::String tuple_destructure_binding(mlc::String binding_names_csv, mlc::String source_name) noexcept{
return ((((mlc::String("auto [", 6) + binding_names_csv) + mlc::String("] = ", 4)) + source_name) + mlc::String("; ", 2));
}
mlc::String record_pattern_field_binding(mlc::String field_cpp_safe, mlc::String variant_lower_name) noexcept{
return ((((((mlc::String("const auto& ", 12) + field_cpp_safe) + mlc::String(" = ", 3)) + variant_lower_name) + mlc::String(".", 1)) + field_cpp_safe) + mlc::String("; ", 2));
}
mlc::String record_update_base_prefix(mlc::String expression_code) noexcept{
return ((mlc::String("auto __upd = ", 13) + expression_code) + mlc::String("; ", 2));
}
mlc::String mutable_member_assignment(mlc::String receiver, mlc::String member_cpp_safe, mlc::String value_code) noexcept{
return (((((receiver + mlc::String(".", 1)) + member_cpp_safe) + mlc::String(" = ", 3)) + value_code) + mlc::String("; ", 2));
}
mlc::String pointer_dereference_parenthesized(mlc::String expression_code) noexcept{
return ((mlc::String("(*", 2) + expression_code) + mlc::String(")", 1));
}
mlc::String visit_subject_for_match(mlc::String expression_code, bool needs_dereference) noexcept{
if (needs_dereference) {
return pointer_dereference_parenthesized(expression_code);
} else {
return expression_code;
}
}
mlc::String make_shared_call(mlc::String element_type, mlc::String argument_code) noexcept{
return ((((mlc::String("std::make_shared<", 17) + element_type) + mlc::String(">(", 2)) + argument_code) + mlc::String(")", 1));
}
mlc::String runtime_to_string_call(mlc::String expression_code) noexcept{
return ((mlc::String("mlc::to_string(", 15) + expression_code) + mlc::String(")", 1));
}
mlc::String empty_map_initializer() noexcept{
return mlc::String("{}", 2);
}
mlc::String block_immediate_function_return_fragment(mlc::String return_expression_code) noexcept{
return ((mlc::String("return ", 7) + return_expression_code) + mlc::String(";\n", 2));
}
mlc::String append_trailing_expression_statement(mlc::String statements_code, mlc::String expression_code) noexcept{
return ((statements_code + expression_code) + mlc::String(";\n", 2));
}
mlc::String hash_map_empty_instantiation(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept{
return ((((mlc::String("mlc::HashMap<", 13) + key_type_cpp) + mlc::String(", ", 2)) + value_type_cpp) + mlc::String(">()", 3));
}
mlc::String typed_array_empty(mlc::String element_type_cpp) noexcept{
return ((mlc::String("mlc::Array<", 11) + element_type_cpp) + mlc::String(">{}", 3));
}
mlc::String array_string_fallback_empty() noexcept{
return mlc::String("mlc::Array<mlc::String>{}", 25);
}
mlc::String match_lambda_const_reference_parameter(mlc::String resolved_type_name, mlc::String generic_angle_bracket_argument, mlc::String binding_name_cpp_safe) noexcept{
return ((((mlc::String("const ", 6) + resolved_type_name) + generic_angle_bracket_argument) + mlc::String("& ", 2)) + binding_name_cpp_safe);
}
mlc::String dot_member_access(mlc::String receiver_code, mlc::String member_code) noexcept{
return ((receiver_code + mlc::String(".", 1)) + member_code);
}
mlc::String comma_separated_pair(mlc::String first_code, mlc::String second_code) noexcept{
return ((first_code + mlc::String(", ", 2)) + second_code);
}
mlc::String prefix_with_optional_suffix(mlc::String prefix, mlc::String optional_suffix) noexcept{
if ((optional_suffix.length() > 0)) {
return (prefix + optional_suffix);
} else {
return prefix;
}
}
mlc::String main_program_parameter_list() noexcept{
return mlc::String("int argc, char** argv", 21);
}
mlc::String noexcept_function_prototype(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept{
return (((((return_type_cpp + mlc::String(" ", 1)) + name_cpp) + mlc::String("(", 1)) + parameter_list) + mlc::String(") noexcept;\n", 12));
}
mlc::String noexcept_function_body_open(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list) noexcept{
return (((((return_type_cpp + mlc::String(" ", 1)) + name_cpp) + mlc::String("(", 1)) + parameter_list) + mlc::String(") noexcept{\n", 12));
}
mlc::String inline_noexcept_forwarding_call(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::String parameter_list, mlc::String callee_code, mlc::String call_argument_list) noexcept{
return ((((((((((mlc::String("inline ", 7) + return_type_cpp) + mlc::String(" ", 1)) + method_name_cpp) + mlc::String("(", 1)) + parameter_list) + mlc::String(") noexcept { return ", 20)) + callee_code) + mlc::String("(", 1)) + call_argument_list) + mlc::String("); }\n", 5));
}
mlc::String noexcept_one_line_returning_function(mlc::String return_type_cpp, mlc::String name_cpp, mlc::String parameter_list, mlc::String return_expression_code) noexcept{
return (((((((return_type_cpp + mlc::String(" ", 1)) + name_cpp) + mlc::String("(", 1)) + parameter_list) + mlc::String(") noexcept { return ", 20)) + return_expression_code) + mlc::String("; }\n", 4));
}
mlc::String parameter_declaration_item(mlc::String type_expression_cpp, mlc::String binding_name_cpp) noexcept{
return ((type_expression_cpp + mlc::String(" ", 1)) + binding_name_cpp);
}
mlc::String block_close_newline() noexcept{
return mlc::String("}\n", 2);
}
mlc::String cpp_array_type_element(mlc::String inner_type_cpp) noexcept{
return ((mlc::String("mlc::Array<", 11) + inner_type_cpp) + mlc::String(">", 1));
}
mlc::String cpp_shared_pointer_type(mlc::String inner_type_cpp) noexcept{
return ((mlc::String("std::shared_ptr<", 16) + inner_type_cpp) + mlc::String(">", 1));
}
mlc::String cpp_template_single_type_argument(mlc::String template_name_cpp, mlc::String type_argument_cpp) noexcept{
return (((template_name_cpp + mlc::String("<", 1)) + type_argument_cpp) + mlc::String(">", 1));
}
mlc::String cpp_template_two_type_arguments(mlc::String template_name_cpp, mlc::String first_type_argument_cpp, mlc::String second_type_argument_cpp) noexcept{
return (((template_name_cpp + mlc::String("<", 1)) + comma_separated_pair(first_type_argument_cpp, second_type_argument_cpp)) + mlc::String(">", 1));
}
mlc::String cpp_std_function_type(mlc::String return_type_cpp, mlc::String parameter_types_csv) noexcept{
return ((((mlc::String("std::function<", 14) + return_type_cpp) + mlc::String("(", 1)) + parameter_types_csv) + mlc::String(")>", 2));
}
mlc::String cpp_lvalue_reference_suffix(mlc::String inner_type_cpp) noexcept{
return (inner_type_cpp + mlc::String("&", 1));
}
mlc::String struct_empty_definition(mlc::String resolved_struct_name) noexcept{
return ((mlc::String("struct ", 7) + resolved_struct_name) + mlc::String(" {};\n", 5));
}
mlc::String struct_with_inline_members_definition(mlc::String resolved_struct_name, mlc::String member_declarations_concatenated) noexcept{
return ((((mlc::String("struct ", 7) + resolved_struct_name) + mlc::String(" {", 2)) + member_declarations_concatenated) + mlc::String("};\n", 3));
}
mlc::String struct_forward_declaration_line(mlc::String template_prefix_before_keyword, mlc::String resolved_struct_name) noexcept{
return (((template_prefix_before_keyword + mlc::String("struct ", 7)) + resolved_struct_name) + mlc::String(";\n", 2));
}
mlc::String variant_using_alias_definition_line(mlc::String template_prefix_before_using, mlc::String resolved_alias_name, mlc::String variant_alternatives_csv) noexcept{
return (((((template_prefix_before_using + mlc::String("using ", 6)) + resolved_alias_name) + mlc::String(" = std::variant<", 16)) + variant_alternatives_csv) + mlc::String(">;\n", 3));
}
mlc::String concept_requires_clause_line(mlc::String requires_predicates_joined) noexcept{
return ((mlc::String("requires ", 9) + requires_predicates_joined) + mlc::String("\n", 1));
}
mlc::String concept_trait_constraint_on_type_parameter(mlc::String trait_name_cpp_safe, mlc::String type_parameter_name) noexcept{
return (((trait_name_cpp_safe + mlc::String("<", 1)) + type_parameter_name) + mlc::String(">", 1));
}
mlc::String struct_tuple_field_declaration(mlc::String element_type_cpp, mlc::String field_index_text) noexcept{
return (((element_type_cpp + mlc::String(" field", 6)) + field_index_text) + mlc::String(";", 1));
}
mlc::String struct_named_field_declaration(mlc::String field_type_cpp, mlc::String field_name_cpp_safe) noexcept{
return (((field_type_cpp + mlc::String(" ", 1)) + field_name_cpp_safe) + mlc::String(";", 1));
}
mlc::String standard_translation_unit_runtime_headers() noexcept{
return mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n", 39);
}
mlc::String include_guard_ifndef_line(mlc::String guard_macro) noexcept{
return ((mlc::String("#ifndef ", 8) + guard_macro) + mlc::String("\n", 1));
}
mlc::String include_guard_define_line(mlc::String guard_macro) noexcept{
return ((mlc::String("#define ", 8) + guard_macro) + mlc::String("\n", 1));
}
mlc::String include_guard_endif_comment_line(mlc::String guard_macro) noexcept{
return ((mlc::String("#endif // ", 10) + guard_macro) + mlc::String("\n", 1));
}
mlc::String namespace_open_line(mlc::String namespace_identifier) noexcept{
return ((mlc::String("namespace ", 10) + namespace_identifier) + mlc::String(" {\n", 3));
}
mlc::String namespace_close_comment_line(mlc::String namespace_identifier) noexcept{
return ((mlc::String("} // namespace ", 15) + namespace_identifier) + mlc::String("\n", 1));
}
mlc::String implementation_define_main_as_user_main_line() noexcept{
return mlc::String("#define main mlc_user_main\n", 27);
}
mlc::String implementation_include_quotefile_line(mlc::String header_filename) noexcept{
return ((mlc::String("#include \"", 10) + header_filename) + mlc::String("\"\n", 2));
}
mlc::String bootstrap_host_main_calling_namespaced_user_main(mlc::String qualified_namespace) noexcept{
return ((((((((((((mlc::String("\n#undef main\n\n", 14) + mlc::String("static void mlc_cli_set_args(int argc, char** argv) {\n", 54)) + mlc::String("  std::vector<mlc::String> arguments;\n", 38)) + mlc::String("  arguments.reserve(argc > 0 ? argc - 1 : 0);\n", 46)) + mlc::String("  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n", 70)) + mlc::String("  mlc::io::set_args(std::move(arguments));\n", 43)) + mlc::String("}\n\n", 3)) + mlc::String("int main(int argc, char** argv) {\n", 34)) + mlc::String("  mlc_cli_set_args(argc, argv);\n", 32)) + mlc::String("  return ::", 11)) + qualified_namespace) + mlc::String("::mlc_user_main(argc, argv);\n", 29)) + mlc::String("}\n", 2));
}
mlc::String cpp_template_typename_header_line(mlc::String template_parameter_list) noexcept{
return ((mlc::String("template<typename ", 18) + template_parameter_list) + mlc::String(">\n", 2));
}
mlc::String concept_requires_expression_method_returns_convertible(mlc::String method_name, mlc::String destination_type_cpp) noexcept{
return ((((mlc::String("{ ", 2) + method_name) + mlc::String("(self) } -> std::convertible_to<", 32)) + destination_type_cpp) + mlc::String(">", 1));
}
mlc::String trait_concept_requires_definition_line(mlc::String template_header, mlc::String trait_cpp_safe, mlc::String self_parameter_name, mlc::String requires_expressions_semicolon_separated) noexcept{
return (((((((template_header + mlc::String("concept ", 8)) + trait_cpp_safe) + mlc::String(" = requires(const ", 18)) + self_parameter_name) + mlc::String("& self) { ", 10)) + requires_expressions_semicolon_separated) + mlc::String("; };\n", 5));
}
mlc::String user_main_arguments_copy_into_runtime_statement() noexcept{
return mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n", 68);
}
mlc::String static_assert_concept_for_type_line(mlc::String concept_cpp_safe, mlc::String type_cpp, mlc::String diagnostic_message_inside_cpp_string_literal) noexcept{
return ((((((mlc::String("static_assert(", 14) + concept_cpp_safe) + mlc::String("<", 1)) + type_cpp) + mlc::String(">, \"", 4)) + diagnostic_message_inside_cpp_string_literal) + mlc::String("\");\n", 4));
}

} // namespace expr

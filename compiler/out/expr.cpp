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

} // namespace expr

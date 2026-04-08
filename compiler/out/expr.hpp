#ifndef EXPR_HPP
#define EXPR_HPP

#include "mlc.hpp"
#include <variant>

namespace expr {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

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

} // namespace expr

#endif // EXPR_HPP

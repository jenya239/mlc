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

} // namespace expr

#endif // EXPR_HPP

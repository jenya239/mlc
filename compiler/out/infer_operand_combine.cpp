#define main mlc_user_main
#include "infer_operand_combine.hpp"

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "binary_diagnostics.hpp"
#include "method_receiver_diagnostics.hpp"
#include "type_diagnostics.hpp"

namespace infer_operand_combine {

using namespace ast;
using namespace infer_result;
using namespace registry;
using namespace semantic_type_structure;
using namespace binary_diagnostics;
using namespace method_receiver_diagnostics;
using namespace type_diagnostics;
using namespace ast_tokens;

infer_result::InferResult infer_binary_from_operand_results(mlc::String operation, infer_result::InferResult left_result, infer_result::InferResult right_result, ast::Span source_span) noexcept{
  auto merged = infer_result::InferResult_absorb(left_result, right_result);
  auto operand_errors = binary_diagnostics::infer_binary_operand_diagnostics(operation, left_result.inferred_type, right_result.inferred_type, source_span);
  return infer_result::InferResult{semantic_type_structure::binary_operation_result_type(operation, left_result.inferred_type), ast::diagnostics_append(merged.errors, operand_errors)};
}
infer_result::InferResult infer_unary_from_inner_result(mlc::String operation, infer_result::InferResult inner_result, ast::Span source_span) noexcept{
  auto minus_errors = type_diagnostics::unary_minus_diagnostic(operation, inner_result.inferred_type, source_span);
  auto bang_errors = type_diagnostics::unary_bang_diagnostic(operation, inner_result.inferred_type, source_span);
  auto result_type = ((operation == mlc::String("!", 1)) ? (std::make_shared<registry::Type>(registry::TBool{})) : (inner_result.inferred_type));
  return infer_result::InferResult{result_type, ast::diagnostics_append(ast::diagnostics_append(inner_result.errors, minus_errors), bang_errors)};
}
infer_result::InferResult infer_index_from_operand_results(infer_result::InferResult object_result, infer_result::InferResult index_result, ast::Span bracket_source_span) noexcept{
  auto element_type = [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = object_result.inferred_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [element_type_value] = tArray; return element_type_value;
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
  auto extra_not_array = type_diagnostics::index_not_array_diagnostic(object_result.inferred_type, bracket_source_span);
  auto extra_bad_index = type_diagnostics::index_not_i32_diagnostic(index_result.inferred_type, bracket_source_span);
  auto merged = infer_result::InferResult_absorb(object_result, index_result);
  return infer_result::InferResult{element_type, ast::diagnostics_append(ast::diagnostics_append(merged.errors, extra_not_array), extra_bad_index)};
}
bool expr_is_empty_array_literal(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return (elements.length() == 0);
}
return false;
std::abort();
}();
}
infer_result::InferResult infer_merge_unknown_else_array_inner(std::shared_ptr<registry::Type> then_element_type, infer_result::InferResult else_result) noexcept{
  return [&]() -> infer_result::InferResult {
auto __match_subject = else_result.inferred_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [else_element_type] = tArray; return (semantic_type_structure::type_is_unknown(else_element_type) ? (infer_result::InferResult{std::make_shared<registry::Type>(registry::TArray{then_element_type}), else_result.errors}) : (else_result));
}
return else_result;
std::abort();
}();
}
infer_result::InferResult conditional_else_result_coerced_for_empty_unknown_array(infer_result::InferResult then_result, infer_result::InferResult else_result, std::shared_ptr<ast::Expr> else_expression) noexcept{
  if ((!expr_is_empty_array_literal(else_expression)))   {
    return else_result;
  } else   {
    return [&]() -> infer_result::InferResult {
auto __match_subject = then_result.inferred_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [then_element_type] = tArray; return infer_merge_unknown_else_array_inner(then_element_type, else_result);
}
return else_result;
std::abort();
}();
  }
}
infer_result::InferResult infer_conditional_from_branch_results(infer_result::InferResult condition_result, infer_result::InferResult then_result, infer_result::InferResult else_result, std::shared_ptr<ast::Expr> else_expression) noexcept{
  auto else_coerced = conditional_else_result_coerced_for_empty_unknown_array(then_result, else_result, else_expression);
  auto merged = infer_result::InferResult_absorb(infer_result::InferResult_absorb(then_result, condition_result), else_coerced);
  auto branches_mismatch = (((!semantic_type_structure::type_is_unknown(then_result.inferred_type)) && (!semantic_type_structure::type_is_unknown(else_coerced.inferred_type))) && (!semantic_type_structure::types_structurally_equal(then_result.inferred_type, else_coerced.inferred_type)));
  auto branch_errors = type_diagnostics::if_branch_mismatch_diagnostic(branches_mismatch, then_result.inferred_type, else_coerced.inferred_type, else_expression);
  return infer_result::InferResult{(branches_mismatch ? (then_result.inferred_type) : (merged.inferred_type)), ast::diagnostics_append(merged.errors, branch_errors)};
}
infer_result::InferResult infer_field_from_object_result(infer_result::InferResult object_result, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept{
  auto resolved_field_type = registry::field_type_from_object(object_result.inferred_type, field_name, registry);
  auto attached = type_diagnostics::infer_expr_field_diagnostics(object_result.inferred_type, field_name, field_source_span, registry);
  return infer_result::InferResult{resolved_field_type, ast::diagnostics_append(object_result.errors, attached)};
}
infer_result::InferResult infer_method_from_object_and_arguments(infer_result::InferResult object_result, infer_result::InferResult with_arguments, mlc::String method_name, ast::Span method_span, int argument_count, registry::TypeRegistry registry) noexcept{
  auto receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(object_result.inferred_type, method_name, method_span);
  auto arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, argument_count, method_span);
  auto builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
  auto inferred_type = (semantic_type_structure::type_is_unknown(builtin_type) ? (registry::method_return_type_from_object(object_result.inferred_type, method_name, registry)) : (builtin_type));
  return infer_result::InferResult{inferred_type, ast::diagnostics_append(ast::diagnostics_append(with_arguments.errors, receiver_errors), arity_errors)};
}

} // namespace infer_operand_combine

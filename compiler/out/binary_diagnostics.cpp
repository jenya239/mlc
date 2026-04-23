#include "binary_diagnostics.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace binary_diagnostics {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{return operation == mlc::String("+") && !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_plus(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("incompatible operand types for +: ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{};}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("numeric operands required for ") + operation + mlc::String(", got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("==") || operation == mlc::String("!=") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_structurally_equal(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("incompatible types for ") + operation + mlc::String(": ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("<") || operation == mlc::String(">") || operation == mlc::String("<=") || operation == mlc::String(">=") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_ordering(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("ordering requires i32 or string operands of the same kind, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("&&") || operation == mlc::String("||") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_logical_and_or(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("logical operators require bool operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("&") || operation == mlc::String("|") || operation == mlc::String("^") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("bitwise ") + operation + mlc::String(" requires i32 operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("<<") || operation == mlc::String(">>") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("shift requires i32 operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> from_addition = infer_binary_diagnostic_for_addition(operation, left_type, right_type, source_span);
return from_addition.size() > 0 ? from_addition : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_int_arithmetic = infer_binary_diagnostic_for_int_arithmetic(operation, left_type, right_type, source_span);
  return from_int_arithmetic.size() > 0 ? from_int_arithmetic : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_equality = infer_binary_diagnostic_for_equality(operation, left_type, right_type, source_span);
  return from_equality.size() > 0 ? from_equality : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_ordering = infer_binary_diagnostic_for_ordering(operation, left_type, right_type, source_span);
  return from_ordering.size() > 0 ? from_ordering : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_logical = infer_binary_diagnostic_for_logical(operation, left_type, right_type, source_span);
  return from_logical.size() > 0 ? from_logical : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_bitwise = infer_binary_diagnostic_for_bitwise(operation, left_type, right_type, source_span);
  return from_bitwise.size() > 0 ? from_bitwise : infer_binary_diagnostic_for_shift(operation, left_type, right_type, source_span);
 }();
 }();
 }();
 }();
 }();
}

} // namespace binary_diagnostics

#ifndef INFER_QUESTION_EXPRESSION_HPP
#define INFER_QUESTION_EXPRESSION_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
#include "result_option_method_types.hpp"
namespace infer_question_expression {

std::shared_ptr<registry::Type> ok_type_from_type_arguments(mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;
infer_result::InferResult infer_result_for_non_result_type(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept;
mlc::String named_or_described_type_name(std::shared_ptr<registry::Type> type_value) noexcept;
mlc::String from_trait_name_for_error_type(std::shared_ptr<registry::Type> inner_error_type) noexcept;
bool expected_error_implements_from(registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_error_type, std::shared_ptr<registry::Type> inner_error_type) noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;
mlc::Array<ast::Diagnostic> question_error_type_mismatch_diagnostics(registry::TypeRegistry registry, std::shared_ptr<registry::Type> inner_result_type, std::shared_ptr<registry::Type> expected_return_type, ast::Span question_span) noexcept;
infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span, std::shared_ptr<registry::Type> expected_return_type, registry::TypeRegistry registry) noexcept;

} // namespace infer_question_expression

#endif // INFER_QUESTION_EXPRESSION_HPP

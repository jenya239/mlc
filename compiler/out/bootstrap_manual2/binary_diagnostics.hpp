#ifndef BINARY_DIAGNOSTICS_HPP
#define BINARY_DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace binary_diagnostics {

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

} // namespace binary_diagnostics

#endif // BINARY_DIAGNOSTICS_HPP

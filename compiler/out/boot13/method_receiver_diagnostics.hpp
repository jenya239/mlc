#ifndef METHOD_RECEIVER_DIAGNOSTICS_HPP
#define METHOD_RECEIVER_DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"

namespace method_receiver_diagnostics {

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_method_receiver_array_hof(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept;

} // namespace method_receiver_diagnostics

#endif // METHOD_RECEIVER_DIAGNOSTICS_HPP

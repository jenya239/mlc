#ifndef EXTERN_DEDUP_LINT_HPP
#define EXTERN_DEDUP_LINT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "diagnostic_codes.hpp"
namespace extern_dedup_lint {

mlc::String header_import_extern_marker() noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;
mlc::String ffi_binding_key(mlc::String c_name, mlc::String header) noexcept;
mlc::String span_site_label(ast::Span source_span) noexcept;
mlc::String encode_binding(int arity, mlc::String site_label) noexcept;
int decode_arity(mlc::String encoded) noexcept;
mlc::String decode_label(mlc::String encoded) noexcept;
mlc::String mismatch_message(mlc::String c_name, mlc::String header, int later_arity, int earlier_arity, mlc::String earlier_label) noexcept;
mlc::HashMap<mlc::String, mlc::String> maybe_record_stub_from_extern_body(mlc::HashMap<mlc::String, mlc::String> binding_by_key, int parameters_length, std::shared_ptr<ast::Expr> body) noexcept;
mlc::HashMap<mlc::String, mlc::String> record_declaration(mlc::HashMap<mlc::String, mlc::String> binding_by_key, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<ast::Diagnostic> error_for_conflict(mlc::String c_name, mlc::String header, int later_arity, int earlier_arity, mlc::String earlier_label, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> diagnostics_from_extern_body(int parameters_length, std::shared_ptr<ast::Expr> body, mlc::HashMap<mlc::String, mlc::String> binding_by_key) noexcept;
mlc::Array<ast::Diagnostic> diagnostics_for_declaration(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, mlc::String> binding_by_key) noexcept;
mlc::Array<ast::Diagnostic> extern_dedup_diagnostics(ast::Program program) noexcept;

} // namespace extern_dedup_lint

#endif // EXTERN_DEDUP_LINT_HPP

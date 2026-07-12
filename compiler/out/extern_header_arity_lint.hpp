#ifndef EXTERN_HEADER_ARITY_LINT_HPP
#define EXTERN_HEADER_ARITY_LINT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "diagnostic_codes.hpp"
namespace extern_header_arity_lint {

mlc::String header_import_extern_marker() noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;
mlc::String arity_mismatch_message(mlc::String c_name, int user_arity, int header_arity) noexcept;
mlc::Array<ast::Diagnostic> warning_for_arity_mismatch(mlc::String c_name, int user_arity, int header_arity, ast::Span source_span) noexcept;
mlc::HashMap<mlc::String, int> maybe_record_stub_from_extern_body(mlc::HashMap<mlc::String, int> arity_by_c_name, int parameters_length, std::shared_ptr<ast::Expr> body) noexcept;
mlc::HashMap<mlc::String, int> record_header_stub_arity(mlc::HashMap<mlc::String, int> arity_by_c_name, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::HashMap<mlc::String, int> build_header_import_arity_index(ast::Program program) noexcept;
mlc::Array<ast::Diagnostic> diagnostics_from_extern_body(int parameters_length, std::shared_ptr<ast::Expr> body, mlc::HashMap<mlc::String, int> arity_by_c_name) noexcept;
mlc::Array<ast::Diagnostic> diagnostics_for_user_extern_against_header(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, int> arity_by_c_name) noexcept;
mlc::Array<ast::Diagnostic> extern_header_arity_diagnostics(ast::Program program) noexcept;

} // namespace extern_header_arity_lint

#endif // EXTERN_HEADER_ARITY_LINT_HPP

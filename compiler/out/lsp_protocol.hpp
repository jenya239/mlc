#ifndef LSP_PROTOCOL_HPP
#define LSP_PROTOCOL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace lsp_protocol {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool lsp_string_starts_with(mlc::String text, mlc::String prefix) noexcept;

int lsp_parse_content_length_header(mlc::String header_line) noexcept;

mlc::String lsp_extract_json_string_field(mlc::String message_body, mlc::String field_name) noexcept;

int lsp_extract_json_number_field(mlc::String message_body, mlc::String field_name) noexcept;

bool lsp_has_json_field(mlc::String message_body, mlc::String field_name) noexcept;

mlc::String lsp_build_jsonrpc_result(int response_id, mlc::String result_json) noexcept;

mlc::String lsp_build_initialize_result() noexcept;

mlc::String lsp_build_hover_result(mlc::String type_text) noexcept;

mlc::String lsp_build_location(mlc::String document_uri, ast::Span definition_span) noexcept;

mlc::String lsp_format_outgoing_message(mlc::String message_body) noexcept;

mlc::String lsp_build_single_diagnostic_json(ast::Diagnostic diagnostic) noexcept;

mlc::String lsp_build_diagnostics_json_array(mlc::Array<ast::Diagnostic> diagnostics) noexcept;

mlc::String lsp_build_publish_diagnostics_notification(mlc::String document_uri, mlc::Array<ast::Diagnostic> diagnostics) noexcept;

} // namespace lsp_protocol

#endif // LSP_PROTOCOL_HPP

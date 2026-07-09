#ifndef SERVER_HPP
#define SERVER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "lsp_protocol.hpp"
#include "symbols.hpp"
#include "hover.hpp"
#include "diagnostics.hpp"
namespace server {

struct LspServerState {
  mlc::HashMap<mlc::String, mlc::String> open_documents;
};
struct LspDispatchOutcome {
  bool continue_loop;
  bool mark_shutdown;
};
mlc::String lsp_uri_to_file_path(mlc::String document_uri) noexcept;
mlc::String lsp_read_message_body() noexcept;
void lsp_write_message(mlc::String message_body) noexcept;
void lsp_handle_initialize(mlc::String message_body) noexcept;
mlc::String lsp_response_initialize(mlc::String message_body) noexcept;
void lsp_handle_shutdown(mlc::String message_body) noexcept;
mlc::String lsp_response_shutdown(mlc::String message_body) noexcept;
void lsp_handle_text_document_did_open(mlc::String message_body, LspServerState& state) noexcept;
void lsp_store_open_document(LspServerState& state, mlc::String document_uri, mlc::String document_text) noexcept;
mlc::Array<mlc::String> lsp_document_open_diagnostics(mlc::String document_uri, mlc::String document_text) noexcept;
void lsp_publish_diagnostics_for_document(mlc::String document_uri, mlc::String document_text) noexcept;
mlc::String lsp_response_publish_diagnostics(mlc::String document_uri, mlc::Array<ast::Diagnostic> document_diagnostics) noexcept;
mlc::String lsp_read_document_source(LspServerState state, mlc::String document_uri, mlc::String source_file) noexcept;
void lsp_handle_hover(mlc::String message_body, LspServerState state) noexcept;
mlc::String lsp_response_hover(mlc::String message_body, LspServerState state) noexcept;
void lsp_handle_definition(mlc::String message_body, LspServerState state) noexcept;
mlc::String lsp_response_definition(mlc::String message_body, LspServerState state) noexcept;
mlc::Array<mlc::String> lsp_responses_did_open(mlc::String message_body, LspServerState& state) noexcept;
mlc::Array<mlc::String> lsp_process_messages_for_test(mlc::Array<mlc::String> messages) noexcept;
LspDispatchOutcome lsp_dispatch_message(mlc::String message_body, LspServerState& state) noexcept;
int run_lsp_server() noexcept;

} // namespace server

#endif // SERVER_HPP

#include "server.hpp"

#include "ast.hpp"
#include "lsp_protocol.hpp"
#include "symbols.hpp"
#include "hover.hpp"
#include "diagnostics.hpp"

namespace server {

using namespace ast;
using namespace lsp_protocol;
using namespace symbols;
using namespace hover;
using namespace diagnostics;
using namespace ast_tokens;

mlc::String lsp_uri_to_file_path(mlc::String document_uri) noexcept;

mlc::String lsp_read_message_body() noexcept;

void lsp_write_message(mlc::String message_body) noexcept;

void lsp_handle_initialize(mlc::String message_body) noexcept;

mlc::String lsp_response_initialize(mlc::String message_body) noexcept;

void lsp_handle_shutdown(mlc::String message_body) noexcept;

mlc::String lsp_response_shutdown(mlc::String message_body) noexcept;

void lsp_handle_text_document_did_open(mlc::String message_body, server::LspServerState& state) noexcept;

void lsp_store_open_document(server::LspServerState& state, mlc::String document_uri, mlc::String document_text) noexcept;

mlc::Array<mlc::String> lsp_document_open_diagnostics(mlc::String document_uri, mlc::String document_text) noexcept;

void lsp_publish_diagnostics_for_document(mlc::String document_uri, mlc::String document_text) noexcept;

mlc::String lsp_response_publish_diagnostics(mlc::String document_uri, mlc::Array<ast::Diagnostic> document_diagnostics) noexcept;

mlc::String lsp_read_document_source(server::LspServerState state, mlc::String document_uri, mlc::String source_file) noexcept;

void lsp_handle_hover(mlc::String message_body, server::LspServerState state) noexcept;

mlc::String lsp_response_hover(mlc::String message_body, server::LspServerState state) noexcept;

void lsp_handle_definition(mlc::String message_body, server::LspServerState state) noexcept;

mlc::String lsp_response_definition(mlc::String message_body, server::LspServerState state) noexcept;

mlc::Array<mlc::String> lsp_responses_did_open(mlc::String message_body, server::LspServerState& state) noexcept;

mlc::Array<mlc::String> lsp_process_messages_for_test(mlc::Array<mlc::String> messages) noexcept;

server::LspDispatchOutcome lsp_dispatch_message(mlc::String message_body, server::LspServerState& state) noexcept;

int run_lsp_server() noexcept;

mlc::String lsp_uri_to_file_path(mlc::String document_uri) noexcept{
return lsp_protocol::lsp_string_starts_with(document_uri, mlc::String("file://")) ? document_uri.substring(7, document_uri.length() - 7) : document_uri;
}

mlc::String lsp_read_message_body() noexcept{
int content_length = -1;
while (true){
{
mlc::String header_line = mlc::io::read_line().trim();
if (header_line == mlc::String("")){
{
break;
}
}
int parsed_length = lsp_protocol::lsp_parse_content_length_header(header_line);
if (parsed_length >= 0){
{
content_length = parsed_length;
}
}
}
}
return content_length < 0 ? mlc::String("") : mlc::io::read_line();
}

void lsp_write_message(mlc::String message_body) noexcept{
int _write_result = mlc::io::print(lsp_protocol::lsp_format_outgoing_message(message_body));
}

void lsp_handle_initialize(mlc::String message_body) noexcept{return lsp_write_message(lsp_response_initialize(message_body));}

mlc::String lsp_response_initialize(mlc::String message_body) noexcept{return lsp_protocol::lsp_build_jsonrpc_result(lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id")), lsp_protocol::lsp_build_initialize_result());}

void lsp_handle_shutdown(mlc::String message_body) noexcept{return lsp_write_message(lsp_response_shutdown(message_body));}

mlc::String lsp_response_shutdown(mlc::String message_body) noexcept{return lsp_protocol::lsp_build_jsonrpc_result(lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id")), mlc::String("null"));}

void lsp_handle_text_document_did_open(mlc::String message_body, server::LspServerState& state) noexcept{
mlc::String document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri"));
mlc::String document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text"));
lsp_store_open_document(state, document_uri, document_text);
return lsp_publish_diagnostics_for_document(document_uri, document_text);
}

void lsp_store_open_document(server::LspServerState& state, mlc::String document_uri, mlc::String document_text) noexcept{
if (document_uri.length() > 0 && document_text.length() > 0){
state.open_documents.set(document_uri, document_text);
}
}

mlc::Array<mlc::String> lsp_document_open_diagnostics(mlc::String document_uri, mlc::String document_text) noexcept{return mlc::Array<mlc::String>{lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri)))};}

void lsp_publish_diagnostics_for_document(mlc::String document_uri, mlc::String document_text) noexcept{
if (document_uri.length() > 0 && document_text.length() > 0){
lsp_write_message(lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri))));
}
}

mlc::String lsp_response_publish_diagnostics(mlc::String document_uri, mlc::Array<ast::Diagnostic> document_diagnostics) noexcept{return lsp_protocol::lsp_build_publish_diagnostics_notification(document_uri, document_diagnostics);}

mlc::String lsp_read_document_source(server::LspServerState state, mlc::String document_uri, mlc::String source_file) noexcept{return state.open_documents.has(document_uri) ? state.open_documents.get(document_uri) : mlc::file::read_to_string(source_file);}

void lsp_handle_hover(mlc::String message_body, server::LspServerState state) noexcept{return lsp_write_message(lsp_response_hover(message_body, state));}

mlc::String lsp_response_hover(mlc::String message_body, server::LspServerState state) noexcept{
int response_id = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id"));
mlc::String document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri"));
int position_line = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("line"));
int position_character = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("character"));
mlc::String source_file = lsp_uri_to_file_path(document_uri);
mlc::String source_text = lsp_read_document_source(state, document_uri, source_file);
mlc::String hover_type = hover::resolve_hover_type_in_source(source_text, source_file, position_line, position_character);
return lsp_protocol::lsp_build_jsonrpc_result(response_id, lsp_protocol::lsp_build_hover_result(hover_type));
}

void lsp_handle_definition(mlc::String message_body, server::LspServerState state) noexcept{return lsp_write_message(lsp_response_definition(message_body, state));}

mlc::String lsp_response_definition(mlc::String message_body, server::LspServerState state) noexcept{
int response_id = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id"));
mlc::String document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri"));
int position_line = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("line"));
int position_character = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("character"));
mlc::String source_file = lsp_uri_to_file_path(document_uri);
mlc::String source_text = lsp_read_document_source(state, document_uri, source_file);
ast::Span definition_span = symbols::resolve_definition_in_source(source_text, source_file, position_line, position_character);
return lsp_protocol::lsp_build_jsonrpc_result(response_id, lsp_protocol::lsp_build_location(document_uri, definition_span));
}

mlc::Array<mlc::String> lsp_responses_did_open(mlc::String message_body, server::LspServerState& state) noexcept{
mlc::String document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri"));
mlc::String document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text"));
lsp_store_open_document(state, document_uri, document_text);
return document_uri.length() > 0 && document_text.length() > 0 ? lsp_document_open_diagnostics(document_uri, document_text) : mlc::Array<mlc::String>{};
}

mlc::Array<mlc::String> lsp_process_messages_for_test(mlc::Array<mlc::String> messages) noexcept{
server::LspServerState state = server::LspServerState{mlc::HashMap<mlc::String, mlc::String>()};
mlc::Array<mlc::String> outgoing = {};
int index = 0;
while (index < messages.size()){
{
mlc::String message_body = messages[index];
mlc::String method_name = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method"));
if (method_name == mlc::String("initialize")){
{
outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_initialize(message_body)));
}
} else {
{
if (method_name == mlc::String("textDocument/didOpen")){
mlc::String document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri"));
mlc::String document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text"));
lsp_store_open_document(state, document_uri, document_text);
if (document_uri.length() > 0 && document_text.length() > 0){
outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri)))));
}
} else {
if (method_name == mlc::String("textDocument/definition")){
outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_definition(message_body, state)));
} else {
if (method_name == mlc::String("textDocument/hover")){
outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_hover(message_body, state)));
} else {
if (method_name == mlc::String("shutdown")){
outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_shutdown(message_body)));
} else {
if (method_name == mlc::String("exit")){
index = messages.size();
} else {
}
}
}
}
}
}
}
index = index + 1;
}
}
return outgoing;
}

server::LspDispatchOutcome lsp_dispatch_message(mlc::String message_body, server::LspServerState& state) noexcept{
mlc::String method_name = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method"));
return method_name == mlc::String("initialize") ? [&]() -> server::LspDispatchOutcome { 
  lsp_handle_initialize(message_body);
  return server::LspDispatchOutcome{true, false};
 }() : method_name == mlc::String("initialized") ? server::LspDispatchOutcome{true, false} : method_name == mlc::String("textDocument/didOpen") ? [&]() -> server::LspDispatchOutcome { 
  lsp_handle_text_document_did_open(message_body, state);
  return server::LspDispatchOutcome{true, false};
 }() : method_name == mlc::String("textDocument/definition") ? [&]() -> server::LspDispatchOutcome { 
  lsp_handle_definition(message_body, state);
  return server::LspDispatchOutcome{true, false};
 }() : method_name == mlc::String("textDocument/hover") ? [&]() -> server::LspDispatchOutcome { 
  lsp_handle_hover(message_body, state);
  return server::LspDispatchOutcome{true, false};
 }() : method_name == mlc::String("shutdown") ? [&]() -> server::LspDispatchOutcome { 
  lsp_handle_shutdown(message_body);
  return server::LspDispatchOutcome{true, true};
 }() : method_name == mlc::String("exit") ? server::LspDispatchOutcome{false, false} : server::LspDispatchOutcome{true, false};
}

int run_lsp_server() noexcept{
server::LspServerState state = server::LspServerState{mlc::HashMap<mlc::String, mlc::String>()};
bool shutdown_flag = false;
while (true){
{
mlc::String message_body = lsp_read_message_body();
if (message_body == mlc::String("")){
{
break;
}
}
server::LspDispatchOutcome outcome = lsp_dispatch_message(message_body, state);
if (outcome.mark_shutdown){
{
shutdown_flag = true;
}
}
if (!outcome.continue_loop){
{
break;
}
}
if (shutdown_flag && lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method")) == mlc::String("exit")){
{
break;
}
}
}
}
return 0;
}

} // namespace server

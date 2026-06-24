#include "lsp_protocol.hpp"

#include "ast.hpp"

namespace lsp_protocol {

using namespace ast;
using namespace ast_tokens;

bool lsp_string_starts_with(mlc::String text, mlc::String prefix) noexcept;

int lsp_parse_content_length_header(mlc::String header_line) noexcept;

mlc::String lsp_scan_quoted_json_value(mlc::String message_body, int value_start) noexcept;

mlc::String lsp_read_quoted_field_value(mlc::String message_body, int value_start) noexcept;

mlc::String lsp_extract_json_string_field(mlc::String message_body, mlc::String field_name) noexcept;

int lsp_extract_json_number_field(mlc::String message_body, mlc::String field_name) noexcept;

bool lsp_has_json_field(mlc::String message_body, mlc::String field_name) noexcept;

mlc::String lsp_build_jsonrpc_result(int response_id, mlc::String result_json) noexcept;

mlc::String lsp_build_initialize_result() noexcept;

mlc::String lsp_build_hover_result(mlc::String type_text) noexcept;

mlc::String lsp_build_location(mlc::String document_uri, ast::Span definition_span) noexcept;

mlc::String lsp_format_outgoing_message(mlc::String message_body) noexcept;

int lsp_diagnostic_severity_number(mlc::String severity) noexcept;

mlc::String lsp_build_single_diagnostic_json(ast::Diagnostic diagnostic) noexcept;

mlc::String lsp_build_diagnostics_json_array(mlc::Array<ast::Diagnostic> diagnostics) noexcept;

mlc::String lsp_build_publish_diagnostics_notification(mlc::String document_uri, mlc::Array<ast::Diagnostic> diagnostics) noexcept;

bool lsp_string_starts_with(mlc::String text, mlc::String prefix) noexcept{return text.length() >= prefix.length() && text.substring(0, prefix.length()) == prefix;}

int lsp_parse_content_length_header(mlc::String header_line) noexcept{
mlc::String prefix = mlc::String("Content-Length:");
return !lsp_string_starts_with(header_line, prefix) ? -1 : header_line.substring(prefix.length(), header_line.length() - prefix.length()).trim().to_i();
}

mlc::String lsp_scan_quoted_json_value(mlc::String message_body, int value_start) noexcept{
int scan_index = value_start;
while (scan_index < message_body.length() && message_body.char_at(scan_index) != mlc::String("\"")){
{
scan_index = scan_index + 1;
}
}
return message_body.substring(value_start, scan_index - value_start);
}

mlc::String lsp_read_quoted_field_value(mlc::String message_body, int value_start) noexcept{return value_start >= message_body.length() || message_body.char_at(value_start) != mlc::String("\"") ? mlc::String("") : lsp_scan_quoted_json_value(message_body, value_start + 1);}

mlc::String lsp_extract_json_string_field(mlc::String message_body, mlc::String field_name) noexcept{
mlc::String quoted_field = mlc::String("\"") + field_name + mlc::String("\"") + mlc::String(":");
mlc::String spaced_field = mlc::String("\"") + field_name + mlc::String("\"") + mlc::String(": ");
int start_index = message_body.index_of(quoted_field);
int field_prefix_length = quoted_field.length();
if (start_index < 0){
{
start_index = message_body.index_of(spaced_field);
field_prefix_length = spaced_field.length();
}
}
return start_index < 0 ? mlc::String("") : lsp_read_quoted_field_value(message_body, start_index + field_prefix_length);
}

int lsp_extract_json_number_field(mlc::String message_body, mlc::String field_name) noexcept{
mlc::String quoted_field = mlc::String("\"") + field_name + mlc::String("\"") + mlc::String(":");
mlc::String spaced_field = mlc::String("\"") + field_name + mlc::String("\"") + mlc::String(": ");
int start_index = message_body.index_of(quoted_field);
int field_prefix_length = quoted_field.length();
if (start_index < 0){
{
start_index = message_body.index_of(spaced_field);
field_prefix_length = spaced_field.length();
}
}
return start_index < 0 ? -1 : [&]() -> int { 
  int value_start = start_index + field_prefix_length;
  int scan_index = value_start;
  while (scan_index < message_body.length()){
{
mlc::String character = message_body.char_at(scan_index);
if (character == mlc::String(",") || character == mlc::String("}")){
{
break;
}
}
scan_index = scan_index + 1;
}
}
  return message_body.substring(value_start, scan_index - value_start).trim().to_i();
 }();
}

bool lsp_has_json_field(mlc::String message_body, mlc::String field_name) noexcept{return lsp_extract_json_string_field(message_body, field_name) != mlc::String("") || message_body.index_of(mlc::String("\"") + field_name + mlc::String("\"") + mlc::String(":")) >= 0;}

mlc::String lsp_build_jsonrpc_result(int response_id, mlc::String result_json) noexcept{return mlc::String("{\"jsonrpc\":\"2.0\",\"id\":") + mlc::to_string(response_id) + mlc::String(",\"result\":") + result_json + mlc::String("}");}

mlc::String lsp_build_initialize_result() noexcept{return mlc::String("{\"capabilities\":{\"definitionProvider\":true,\"hoverProvider\":true},\"serverInfo\":{\"name\":\"mlcc\",\"version\":\"0.1\"}}");}

mlc::String lsp_build_hover_result(mlc::String type_text) noexcept{return type_text.length() == 0 ? mlc::String("null") : mlc::String("{\"contents\":[{\"language\":\"mlc\",\"value\":\"") + type_text + mlc::String("\"}]}");}

mlc::String lsp_build_location(mlc::String document_uri, ast::Span definition_span) noexcept{
return definition_span.line <= 0 ? mlc::String("null") : [&]() -> mlc::String { 
  int start_line = definition_span.line - 1;
  int start_character = definition_span.column - 1;
  int end_character = start_character + 1;
  mlc::String location_json = mlc::String("{\"uri\":\"") + document_uri + mlc::String("\",\"range\":{\"start\":{\"line\":") + mlc::to_string(start_line) + mlc::String(",\"character\":") + mlc::to_string(start_character) + mlc::String("},\"end\":{\"line\":") + mlc::to_string(start_line) + mlc::String(",\"character\":") + mlc::to_string(end_character) + mlc::String("}}}");
  return location_json;
 }();
}

mlc::String lsp_format_outgoing_message(mlc::String message_body) noexcept{return mlc::String("Content-Length: ") + mlc::to_string(message_body.length()) + mlc::String("\r\n\r\n") + message_body;}

int lsp_diagnostic_severity_number(mlc::String severity) noexcept{return severity == mlc::String("warning") ? 2 : 1;}

mlc::String lsp_build_single_diagnostic_json(ast::Diagnostic diagnostic) noexcept{
int start_line = diagnostic.span.line > 0 ? diagnostic.span.line - 1 : 0;
int start_character = diagnostic.span.column > 0 ? diagnostic.span.column - 1 : 0;
int end_character = start_character + 1;
return mlc::String("{\"range\":{\"start\":{\"line\":") + mlc::to_string(start_line) + mlc::String(",\"character\":") + mlc::to_string(start_character) + mlc::String("},\"end\":{\"line\":") + mlc::to_string(start_line) + mlc::String(",\"character\":") + mlc::to_string(end_character) + mlc::String("}},\"severity\":") + mlc::to_string(lsp_diagnostic_severity_number(diagnostic.severity)) + mlc::String(",\"message\":\"") + diagnostic.message + mlc::String("\"}");
}

mlc::String lsp_build_diagnostics_json_array(mlc::Array<ast::Diagnostic> diagnostics) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < diagnostics.size()){
{
parts.push_back(lsp_build_single_diagnostic_json(diagnostics[index]));
index = index + 1;
}
}
return parts.join(mlc::String(","));
}

mlc::String lsp_build_publish_diagnostics_notification(mlc::String document_uri, mlc::Array<ast::Diagnostic> diagnostics) noexcept{return mlc::String("{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"") + document_uri + mlc::String("\",\"diagnostics\":[") + lsp_build_diagnostics_json_array(diagnostics) + mlc::String("]}}");}

} // namespace lsp_protocol

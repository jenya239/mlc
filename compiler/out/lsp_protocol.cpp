#define main mlc_user_main
#include "lsp_protocol.hpp"

#include "ast.hpp"

namespace lsp_protocol {

using namespace ast;
using namespace ast_tokens;

bool lsp_string_starts_with(mlc::String text, mlc::String prefix) noexcept{
  return ((text.length() >= prefix.length()) && (text.substring(0, prefix.length()) == prefix));
}
int lsp_parse_content_length_header(mlc::String header_line) noexcept{
  auto prefix = mlc::String("Content-Length:", 15);
  if ((!lsp_string_starts_with(header_line, prefix)))   {
    return (-1);
  } else   {
    return header_line.substring(prefix.length(), mlc::arith::checked_sub(header_line.length(), prefix.length())).trim().to_i();
  }
}
mlc::String lsp_scan_quoted_json_value(mlc::String message_body, int value_start) noexcept{
  auto scan_index = value_start;
  while (((scan_index < message_body.length()) && (message_body.char_at(scan_index) != mlc::String("\"", 1))))   {
    (scan_index = mlc::arith::checked_add(scan_index, 1));
  }
  return message_body.substring(value_start, mlc::arith::checked_sub(scan_index, value_start));
}
mlc::String lsp_read_quoted_field_value(mlc::String message_body, int value_start) noexcept{
  if (((value_start >= message_body.length()) || (message_body.char_at(value_start) != mlc::String("\"", 1))))   {
    return mlc::String("", 0);
  } else   {
    return lsp_scan_quoted_json_value(message_body, mlc::arith::checked_add(value_start, 1));
  }
}
mlc::String lsp_extract_json_string_field(mlc::String message_body, mlc::String field_name) noexcept{
  auto quoted_field = (((mlc::String("\"", 1) + field_name) + mlc::String("\"", 1)) + mlc::String(":", 1));
  auto spaced_field = (((mlc::String("\"", 1) + field_name) + mlc::String("\"", 1)) + mlc::String(": ", 2));
  auto quoted_start = message_body.index_of(quoted_field);
  auto spaced_start = message_body.index_of(spaced_field);
  if ((quoted_start >= 0))   {
    return lsp_read_quoted_field_value(message_body, (quoted_start + quoted_field.length()));
  } else if ((spaced_start >= 0))   {
    return lsp_read_quoted_field_value(message_body, (spaced_start + spaced_field.length()));
  } else   {
    return mlc::String("", 0);
  }
}
int lsp_scan_json_number_at(mlc::String message_body, int value_start) noexcept{
  auto scan_index = value_start;
  while ((scan_index < message_body.length()))   {
    auto character = message_body.char_at(scan_index);
    if (((character == mlc::String(",", 1)) || (character == mlc::String("}", 1))))     {
      break;
    }
    (scan_index = mlc::arith::checked_add(scan_index, 1));
  }
  return message_body.substring(value_start, mlc::arith::checked_sub(scan_index, value_start)).trim().to_i();
}
int lsp_extract_json_number_field(mlc::String message_body, mlc::String field_name) noexcept{
  auto quoted_field = (((mlc::String("\"", 1) + field_name) + mlc::String("\"", 1)) + mlc::String(":", 1));
  auto spaced_field = (((mlc::String("\"", 1) + field_name) + mlc::String("\"", 1)) + mlc::String(": ", 2));
  auto quoted_start = message_body.index_of(quoted_field);
  if ((quoted_start >= 0))   {
    return lsp_scan_json_number_at(message_body, (quoted_start + quoted_field.length()));
  } else   {
    auto spaced_start = message_body.index_of(spaced_field);
    if ((spaced_start < 0))     {
      return (-1);
    } else     {
      return lsp_scan_json_number_at(message_body, (spaced_start + spaced_field.length()));
    }
  }
}
bool lsp_has_json_field(mlc::String message_body, mlc::String field_name) noexcept{
  return ((lsp_extract_json_string_field(message_body, field_name) != mlc::String("", 0)) || (message_body.index_of((((mlc::String("\"", 1) + field_name) + mlc::String("\"", 1)) + mlc::String(":", 1))) >= 0));
}
mlc::String lsp_build_jsonrpc_result(int response_id, mlc::String result_json) noexcept{
  return ((((mlc::String("{\"jsonrpc\":\"2.0\",\"id\":", 22) + mlc::to_string(response_id)) + mlc::String(",\"result\":", 10)) + result_json) + mlc::String("}", 1));
}
mlc::String lsp_build_initialize_result() noexcept{
  return mlc::String("{\"capabilities\":{\"definitionProvider\":true,\"hoverProvider\":true},\"serverInfo\":{\"name\":\"mlcc\",\"version\":\"0.1\"}}", 110);
}
mlc::String lsp_build_hover_result(mlc::String type_text) noexcept{
  if ((type_text.length() == 0))   {
    return mlc::String("null", 4);
  } else   {
    return ((mlc::String("{\"contents\":[{\"language\":\"mlc\",\"value\":\"", 40) + type_text) + mlc::String("\"}]}", 4));
  }
}
mlc::String lsp_build_location(mlc::String document_uri, ast::Span definition_span) noexcept{
  if ((definition_span.line <= 0))   {
    return mlc::String("null", 4);
  } else   {
    auto start_line = mlc::arith::checked_sub(definition_span.line, 1);
    auto start_character = mlc::arith::checked_sub(definition_span.column, 1);
    auto end_character = mlc::arith::checked_add(start_character, 1);
    auto location_json = ((((((((((mlc::String("{\"uri\":\"", 8) + document_uri) + mlc::String("\",\"range\":{\"start\":{\"line\":", 27)) + mlc::to_string(start_line)) + mlc::String(",\"character\":", 13)) + mlc::to_string(start_character)) + mlc::String("},\"end\":{\"line\":", 16)) + mlc::to_string(start_line)) + mlc::String(",\"character\":", 13)) + mlc::to_string(end_character)) + mlc::String("}}}", 3));
    return location_json;
  }
}
mlc::String lsp_format_outgoing_message(mlc::String message_body) noexcept{
  return (((mlc::String("Content-Length: ", 16) + mlc::to_string(message_body.length())) + mlc::String("\r\n\r\n", 4)) + message_body);
}
int lsp_diagnostic_severity_number(mlc::String severity) noexcept{
  if ((severity == mlc::String("warning", 7)))   {
    return 2;
  } else   {
    return 1;
  }
}
int lsp_line_zero_based(int line) noexcept{
  if ((line > 0))   {
    return mlc::arith::checked_sub(line, 1);
  } else   {
    return 0;
  }
}
int lsp_column_zero_based(int column) noexcept{
  if ((column > 0))   {
    return mlc::arith::checked_sub(column, 1);
  } else   {
    return 0;
  }
}
mlc::String lsp_build_single_diagnostic_json(ast::Diagnostic diagnostic) noexcept{
  auto start_line = lsp_line_zero_based(diagnostic.span.line);
  auto start_character = lsp_column_zero_based(diagnostic.span.column);
  auto end_character = mlc::arith::checked_add(start_character, 1);
  return ((((((((((((mlc::String("{\"range\":{\"start\":{\"line\":", 26) + mlc::to_string(start_line)) + mlc::String(",\"character\":", 13)) + mlc::to_string(start_character)) + mlc::String("},\"end\":{\"line\":", 16)) + mlc::to_string(start_line)) + mlc::String(",\"character\":", 13)) + mlc::to_string(end_character)) + mlc::String("}},\"severity\":", 14)) + mlc::to_string(lsp_diagnostic_severity_number(diagnostic.severity))) + mlc::String(",\"message\":\"", 12)) + diagnostic.message) + mlc::String("\"}", 2));
}
mlc::String lsp_build_diagnostics_json_array(mlc::Array<ast::Diagnostic> diagnostics) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < diagnostics.length()))   {
    parts.push_back(lsp_build_single_diagnostic_json(diagnostics[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return parts.join(mlc::String(",", 1));
}
mlc::String lsp_build_publish_diagnostics_notification(mlc::String document_uri, mlc::Array<ast::Diagnostic> diagnostics) noexcept{
  return ((((mlc::String("{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"", 77) + document_uri) + mlc::String("\",\"diagnostics\":[", 17)) + lsp_build_diagnostics_json_array(diagnostics)) + mlc::String("]}}", 3));
}

} // namespace lsp_protocol

#define main mlc_user_main
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

mlc::String lsp_uri_to_file_path(mlc::String document_uri) noexcept{
  if (lsp_protocol::lsp_string_starts_with(document_uri, mlc::String("file://", 7)))   {
    return document_uri.substring(7, (document_uri.length() - 7));
  } else   {
    return document_uri;
  }
}
mlc::String lsp_read_message_body() noexcept{
  auto content_length = (-1);
  while (true)   {
    auto header_line = mlc::io::read_line().trim();
    if ((header_line == mlc::String("", 0)))     {
      break;
    }
    auto parsed_length = lsp_protocol::lsp_parse_content_length_header(header_line);
    if ((parsed_length >= 0))     {
      (content_length = parsed_length);
    }
  }
  if ((content_length < 0))   {
    return mlc::String("", 0);
  } else   {
    return mlc::io::read_line();
  }
}
void lsp_write_message(mlc::String message_body) noexcept{
  auto _write_result = mlc::io::print(lsp_protocol::lsp_format_outgoing_message(message_body));
}
void lsp_handle_initialize(mlc::String message_body) noexcept{
  return lsp_write_message(lsp_response_initialize(message_body));
}
mlc::String lsp_response_initialize(mlc::String message_body) noexcept{
  return lsp_protocol::lsp_build_jsonrpc_result(lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id", 2)), lsp_protocol::lsp_build_initialize_result());
}
void lsp_handle_shutdown(mlc::String message_body) noexcept{
  return lsp_write_message(lsp_response_shutdown(message_body));
}
mlc::String lsp_response_shutdown(mlc::String message_body) noexcept{
  return lsp_protocol::lsp_build_jsonrpc_result(lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id", 2)), mlc::String("null", 4));
}
void lsp_handle_text_document_did_open(mlc::String message_body, LspServerState& state) noexcept{
  auto document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri", 3));
  auto document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text", 4));
  lsp_store_open_document(state, document_uri, document_text);
  return lsp_publish_diagnostics_for_document(document_uri, document_text);
}
void lsp_store_open_document(LspServerState& state, mlc::String document_uri, mlc::String document_text) noexcept{
  if (((document_uri.length() > 0) && (document_text.length() > 0)))   {
    state.open_documents.set(document_uri, document_text);
  }
}
mlc::Array<mlc::String> lsp_document_open_diagnostics(mlc::String document_uri, mlc::String document_text) noexcept{
  return mlc::Array<mlc::String>{lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri)))};
}
void lsp_publish_diagnostics_for_document(mlc::String document_uri, mlc::String document_text) noexcept{
  if (((document_uri.length() > 0) && (document_text.length() > 0)))   {
    lsp_write_message(lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri))));
  }
}
mlc::String lsp_response_publish_diagnostics(mlc::String document_uri, mlc::Array<ast::Diagnostic> document_diagnostics) noexcept{
  return lsp_protocol::lsp_build_publish_diagnostics_notification(document_uri, document_diagnostics);
}
mlc::String lsp_read_document_source(LspServerState state, mlc::String document_uri, mlc::String source_file) noexcept{
  if (state.open_documents.has(document_uri))   {
    return state.open_documents.get(document_uri);
  } else   {
    return mlc::file::read_to_string(source_file);
  }
}
void lsp_handle_hover(mlc::String message_body, LspServerState state) noexcept{
  return lsp_write_message(lsp_response_hover(message_body, state));
}
mlc::String lsp_response_hover(mlc::String message_body, LspServerState state) noexcept{
  auto response_id = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id", 2));
  auto document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri", 3));
  auto position_line = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("line", 4));
  auto position_character = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("character", 9));
  auto source_file = lsp_uri_to_file_path(document_uri);
  auto source_text = lsp_read_document_source(state, document_uri, source_file);
  auto hover_type = hover::resolve_hover_type_in_source(source_text, source_file, position_line, position_character);
  return lsp_protocol::lsp_build_jsonrpc_result(response_id, lsp_protocol::lsp_build_hover_result(hover_type));
}
void lsp_handle_definition(mlc::String message_body, LspServerState state) noexcept{
  return lsp_write_message(lsp_response_definition(message_body, state));
}
mlc::String lsp_response_definition(mlc::String message_body, LspServerState state) noexcept{
  auto response_id = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("id", 2));
  auto document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri", 3));
  auto position_line = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("line", 4));
  auto position_character = lsp_protocol::lsp_extract_json_number_field(message_body, mlc::String("character", 9));
  auto source_file = lsp_uri_to_file_path(document_uri);
  auto source_text = lsp_read_document_source(state, document_uri, source_file);
  auto definition_span = symbols::resolve_definition_in_source(source_text, source_file, position_line, position_character);
  return lsp_protocol::lsp_build_jsonrpc_result(response_id, lsp_protocol::lsp_build_location(document_uri, definition_span));
}
mlc::Array<mlc::String> lsp_responses_did_open(mlc::String message_body, LspServerState& state) noexcept{
  auto document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri", 3));
  auto document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text", 4));
  lsp_store_open_document(state, document_uri, document_text);
  if (((document_uri.length() > 0) && (document_text.length() > 0)))   {
    return lsp_document_open_diagnostics(document_uri, document_text);
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> lsp_process_messages_for_test(mlc::Array<mlc::String> messages) noexcept{
  auto state = LspServerState{{}};
  auto outgoing = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < messages.length()))   {
    auto message_body = messages[index];
    auto method_name = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method", 6));
    if ((method_name == mlc::String("initialize", 10)))     {
      outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_initialize(message_body)));
    } else if ((method_name == mlc::String("textDocument/didOpen", 20)))     {
      auto document_uri = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("uri", 3));
      auto document_text = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("text", 4));
      lsp_store_open_document(state, document_uri, document_text);
      if (((document_uri.length() > 0) && (document_text.length() > 0)))       {
        outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_publish_diagnostics(document_uri, diagnostics::collect_diagnostics_in_source(document_text, lsp_uri_to_file_path(document_uri)))));
      }
    } else if ((method_name == mlc::String("textDocument/definition", 23)))     {
      outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_definition(message_body, state)));
    } else if ((method_name == mlc::String("textDocument/hover", 18)))     {
      outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_hover(message_body, state)));
    } else if ((method_name == mlc::String("shutdown", 8)))     {
      outgoing.push_back(lsp_protocol::lsp_format_outgoing_message(lsp_response_shutdown(message_body)));
    } else if ((method_name == mlc::String("exit", 4)))     {
      (index = messages.length());
    }
    (index = (index + 1));
  }
  return outgoing;
}
LspDispatchOutcome lsp_dispatch_message(mlc::String message_body, LspServerState& state) noexcept{
  auto method_name = lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method", 6));
  if ((method_name == mlc::String("initialize", 10)))   {
    lsp_handle_initialize(message_body);
    return LspDispatchOutcome{true, false};
  } else if ((method_name == mlc::String("initialized", 11)))   {
    return LspDispatchOutcome{true, false};
  } else if ((method_name == mlc::String("textDocument/didOpen", 20)))   {
    lsp_handle_text_document_did_open(message_body, state);
    return LspDispatchOutcome{true, false};
  } else if ((method_name == mlc::String("textDocument/definition", 23)))   {
    lsp_handle_definition(message_body, state);
    return LspDispatchOutcome{true, false};
  } else if ((method_name == mlc::String("textDocument/hover", 18)))   {
    lsp_handle_hover(message_body, state);
    return LspDispatchOutcome{true, false};
  } else if ((method_name == mlc::String("shutdown", 8)))   {
    lsp_handle_shutdown(message_body);
    return LspDispatchOutcome{true, true};
  } else if ((method_name == mlc::String("exit", 4)))   {
    return LspDispatchOutcome{false, false};
  } else   {
    return LspDispatchOutcome{true, false};
  }
}
int run_lsp_server() noexcept{
  auto state = LspServerState{{}};
  auto shutdown_flag = false;
  while (true)   {
    auto message_body = lsp_read_message_body();
    if ((message_body == mlc::String("", 0)))     {
      break;
    }
    auto outcome = lsp_dispatch_message(message_body, state);
    if (outcome.mark_shutdown)     {
      (shutdown_flag = true);
    }
    if ((!outcome.continue_loop))     {
      break;
    }
    if ((shutdown_flag && (lsp_protocol::lsp_extract_json_string_field(message_body, mlc::String("method", 6)) == mlc::String("exit", 4))))     {
      break;
    }
  }
  return 0;
}

} // namespace server

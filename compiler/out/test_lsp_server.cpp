#include "test_lsp_server.hpp"

#include "test_runner.hpp"
#include "lsp_protocol.hpp"
#include "symbols.hpp"
#include "hover.hpp"
#include "diagnostics.hpp"
#include "server.hpp"
#include "lsp_cli.hpp"

namespace test_lsp_server {

using namespace test_runner;
using namespace lsp_protocol;
using namespace symbols;
using namespace hover;
using namespace diagnostics;
using namespace server;
using namespace lsp_cli;

mlc::Array<test_runner::TestResult> lsp_server_tests() noexcept;

mlc::Array<test_runner::TestResult> lsp_server_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
mlc::String initialize_request = mlc::String("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"initialize\",\"params\":{\"processId\":null}}");
results.push_back(test_runner::assert_eq_str(mlc::String("lsp_extract_json_string_field method"), lsp_protocol::lsp_extract_json_string_field(initialize_request, mlc::String("method")), mlc::String("initialize")));
results.push_back(test_runner::assert_eq_int(mlc::String("lsp_extract_json_number_field id"), lsp_protocol::lsp_extract_json_number_field(initialize_request, mlc::String("id")), 1));
results.push_back(test_runner::assert_true(mlc::String("lsp_build_initialize_result has serverInfo"), lsp_protocol::lsp_build_initialize_result().contains(mlc::String("\"serverInfo\""))));
mlc::String response_body = lsp_protocol::lsp_build_jsonrpc_result(1, lsp_protocol::lsp_build_initialize_result());
results.push_back(test_runner::assert_true(mlc::String("lsp_build_jsonrpc_result has jsonrpc"), response_body.contains(mlc::String("\"jsonrpc\":\"2.0\""))));
mlc::String framed = lsp_protocol::lsp_format_outgoing_message(response_body);
results.push_back(test_runner::assert_true(mlc::String("lsp_format_outgoing_message Content-Length header"), framed.contains(mlc::String("Content-Length:"))));
results.push_back(test_runner::assert_true(mlc::String("lsp_format_outgoing_message body suffix"), framed.contains(response_body)));
results.push_back(test_runner::assert_eq_int(mlc::String("lsp_parse_content_length_header"), lsp_protocol::lsp_parse_content_length_header(mlc::String("Content-Length: 42")), 42));
results.push_back(test_runner::assert_true(mlc::String("lsp_build_initialize_result definitionProvider"), lsp_protocol::lsp_build_initialize_result().contains(mlc::String("\"definitionProvider\":true"))));
results.push_back(test_runner::assert_true(mlc::String("lsp_build_initialize_result hoverProvider"), lsp_protocol::lsp_build_initialize_result().contains(mlc::String("\"hoverProvider\":true"))));
mlc::String sample_source = mlc::String("fn greet() -> i32 = do\n  let answer = 42\n  answer\nend\n");
ast::Span definition_span = symbols::resolve_definition_in_source(sample_source, mlc::String("sample.mlc"), 1, 6);
results.push_back(test_runner::assert_eq_int(mlc::String("resolve_definition_in_source let binding line"), definition_span.line, 2));
results.push_back(test_runner::assert_eq_str(mlc::String("resolve_definition_in_source let binding file"), definition_span.file, mlc::String("sample.mlc")));
mlc::String location_json = lsp_protocol::lsp_build_location(mlc::String("file:///tmp/sample.mlc"), definition_span);
results.push_back(test_runner::assert_true(mlc::String("lsp_build_location has uri"), location_json.contains(mlc::String("file:///tmp/sample.mlc"))));
results.push_back(test_runner::assert_eq_str(mlc::String("resolve_hover_type_in_source let binding"), hover::resolve_hover_type_in_source(sample_source, mlc::String("sample.mlc"), 1, 6), mlc::String("i32")));
mlc::String hover_json = lsp_protocol::lsp_build_hover_result(mlc::String("i32"));
results.push_back(test_runner::assert_true(mlc::String("lsp_build_hover_result has type"), hover_json.contains(mlc::String("\"value\":\"i32\""))));
mlc::String bad_source = mlc::String("fn bad() -> i32 = \"x\"\n");
mlc::Array<ast::Diagnostic> diagnostics = diagnostics::collect_diagnostics_in_source(bad_source, mlc::String("bad.mlc"));
results.push_back(test_runner::assert_true(mlc::String("collect_diagnostics_in_source has errors"), diagnostics.size() > 0));
mlc::String publish_json = lsp_protocol::lsp_build_publish_diagnostics_notification(mlc::String("file:///bad.mlc"), diagnostics);
results.push_back(test_runner::assert_true(mlc::String("publishDiagnostics has method"), publish_json.contains(mlc::String("\"textDocument/publishDiagnostics\""))));
results.push_back(test_runner::assert_true(mlc::String("publishDiagnostics has error severity"), publish_json.contains(mlc::String("\"severity\":1"))));
results.push_back(test_runner::assert_true(mlc::String("lsp_usage_message mentions lsp"), lsp_cli::lsp_usage_message().contains(mlc::String("mlcc lsp"))));
mlc::String open_message = mlc::String("{\"method\":\"textDocument/didOpen\",\"params\":{\"uri\":\"file:///sample.mlc\",\"text\":\"fn greet() -> i32 = 42\\n\"}}");
mlc::String definition_message = mlc::String("{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"textDocument/definition\",\"params\":{\"textDocument\":{\"uri\":\"file:///sample.mlc\"},\"position\":{\"line\":0,\"character\":3}}}");
mlc::Array<mlc::String> integration_outputs = server::lsp_process_messages_for_test(mlc::Array<mlc::String>{open_message, definition_message});
results.push_back(test_runner::assert_true(mlc::String("lsp integration didOpen+definition produces output"), integration_outputs.size() > 0));
results.push_back(test_runner::assert_true(mlc::String("lsp integration definition response has result"), integration_outputs[integration_outputs.size() - 1].contains(mlc::String("\"result\":"))));
return results;
}

} // namespace test_lsp_server

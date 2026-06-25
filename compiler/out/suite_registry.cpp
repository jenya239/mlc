#include "suite_registry.hpp"

#include "test_runner.hpp"
#include "test_lexer.hpp"
#include "test_parser.hpp"
#include "test_checker.hpp"
#include "test_codegen.hpp"
#include "test_pipe_and_record_update.hpp"
#include "test_decl_gen.hpp"
#include "test_partial_application.hpp"
#include "test_merge_imports.hpp"
#include "test_pass.hpp"
#include "test_pass_manager.hpp"
#include "test_program_to_semantic.hpp"
#include "test_dump_flags.hpp"
#include "test_core_ir.hpp"
#include "test_driver.hpp"
#include "test_verify_ir.hpp"
#include "test_compile_commands.hpp"
#include "test_expr_visitor.hpp"
#include "test_visitor_pass_parity.hpp"
#include "test_cpp_printer.hpp"
#include "test_cpp_lexer.hpp"
#include "test_cpp_ast.hpp"
#include "test_verify_cpp_ast.hpp"
#include "test_cpp_parser.hpp"
#include "test_cpp_parser_structural.hpp"
#include "test_cpp_types.hpp"
#include "test_cpp_header_import.hpp"
#include "test_cpp_to_source.hpp"
#include "test_cpp_exprs.hpp"
#include "test_lsp_server.hpp"
#include "test_formatter.hpp"
#include "test_fuzz.hpp"
#include "test_golden_harness.hpp"
#include "test_record_field_default_validate.hpp"

namespace suite_registry {

using namespace test_runner;
using namespace test_lexer;
using namespace test_parser;
using namespace test_checker;
using namespace test_codegen;
using namespace test_pipe_and_record_update;
using namespace test_decl_gen;
using namespace test_partial_application;
using namespace test_merge_imports;
using namespace test_pass;
using namespace test_pass_manager;
using namespace test_program_to_semantic;
using namespace test_dump_flags;
using namespace test_core_ir;
using namespace test_driver;
using namespace test_verify_ir;
using namespace test_compile_commands;
using namespace test_expr_visitor;
using namespace test_visitor_pass_parity;
using namespace test_cpp_printer;
using namespace test_cpp_lexer;
using namespace test_cpp_ast;
using namespace test_verify_cpp_ast;
using namespace test_cpp_parser;
using namespace test_cpp_parser_structural;
using namespace test_cpp_types;
using namespace test_cpp_header_import;
using namespace test_cpp_to_source;
using namespace test_cpp_exprs;
using namespace test_lsp_server;
using namespace test_formatter;
using namespace test_fuzz;
using namespace test_golden_harness;
using namespace test_record_field_default_validate;

mlc::Array<test_runner::TestResult> append_suite_results(mlc::Array<test_runner::TestResult>& accumulator, mlc::Array<test_runner::TestResult> suite) noexcept;

mlc::Array<test_runner::TestResult> syntax_suite_results() noexcept;

mlc::Array<test_runner::TestResult> hir_suite_results() noexcept;

mlc::Array<test_runner::TestResult> types_suite_results() noexcept;

mlc::Array<test_runner::TestResult> cpp_suite_results() noexcept;

mlc::Array<test_runner::TestResult> integration_suite_results() noexcept;

mlc::Array<test_runner::TestResult> tooling_suite_results() noexcept;

mlc::Array<test_runner::TestResult> all_test_suites() noexcept;

mlc::Array<test_runner::TestResult> append_suite_results(mlc::Array<test_runner::TestResult>& accumulator, mlc::Array<test_runner::TestResult> suite) noexcept{
int index = 0;
while (index < suite.size()){
{
accumulator.push_back(suite[index]);
index = index + 1;
}
}
return accumulator;
}

mlc::Array<test_runner::TestResult> syntax_suite_results() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, test_lexer::lexer_tests());
results = append_suite_results(results, test_parser::parser_tests());
results = append_suite_results(results, test_verify_ir::verify_ir_tests());
results = append_suite_results(results, test_dump_flags::dump_flags_tests());
results = append_suite_results(results, test_formatter::formatter_tests());
results = append_suite_results(results, test_golden_harness::golden_harness_tests());
return results;
}

mlc::Array<test_runner::TestResult> hir_suite_results() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, test_program_to_semantic::program_to_semantic_tests());
results = append_suite_results(results, test_merge_imports::merge_import_tests());
results = append_suite_results(results, test_partial_application::partial_application_tests());
return results;
}

mlc::Array<test_runner::TestResult> types_suite_results() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, test_checker::checker_tests());
results = append_suite_results(results, test_pipe_and_record_update::pipe_and_record_update_tests());
results = append_suite_results(results, test_pass::pass_type_tests());
results = append_suite_results(results, test_pass_manager::pass_manager_tests());
results = append_suite_results(results, test_record_field_default_validate::record_field_default_validate_tests());
return results;
}

mlc::Array<test_runner::TestResult> cpp_suite_results() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, test_cpp_printer::cpp_printer_tests());
results = append_suite_results(results, test_cpp_lexer::cpp_lexer_tests());
results = append_suite_results(results, test_cpp_ast::cpp_ast_tests());
results = append_suite_results(results, test_verify_cpp_ast::verify_cpp_ast_tests());
results = append_suite_results(results, test_cpp_parser::cpp_parser_tests());
results = append_suite_results(results, test_cpp_parser_structural::cpp_parser_structural_tests());
results = append_suite_results(results, test_cpp_types::cpp_type_parser_tests());
results = append_suite_results(results, test_cpp_header_import::cpp_header_import_tests());
results = append_suite_results(results, test_cpp_to_source::cpp_to_source_tests());
results = append_suite_results(results, test_cpp_exprs::cpp_expr_parser_tests());
return results;
}

mlc::Array<test_runner::TestResult> integration_suite_results() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, test_codegen::codegen_tests());
results = append_suite_results(results, test_decl_gen::decl_gen_tests());
results = append_suite_results(results, test_core_ir::core_ir_tests());
results = append_suite_results(results, test_driver::driver_tests());
results = append_suite_results(results, test_expr_visitor::expr_visitor_tests());
results = append_suite_results(results, test_visitor_pass_parity::visitor_pass_parity_tests());
results = append_suite_results(results, test_compile_commands::compile_commands_tests());
results = append_suite_results(results, test_fuzz::fuzz_tests());
return results;
}

mlc::Array<test_runner::TestResult> tooling_suite_results() noexcept{
return test_lsp_server::lsp_server_tests();
}

mlc::Array<test_runner::TestResult> all_test_suites() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results = append_suite_results(results, syntax_suite_results());
results = append_suite_results(results, hir_suite_results());
results = append_suite_results(results, types_suite_results());
results = append_suite_results(results, cpp_suite_results());
results = append_suite_results(results, integration_suite_results());
results = append_suite_results(results, tooling_suite_results());
return results;
}

} // namespace suite_registry

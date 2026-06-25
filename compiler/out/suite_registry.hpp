#ifndef SUITE_REGISTRY_HPP
#define SUITE_REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

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
#include "test_mir_types.hpp"
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

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct TestIrSyntax {};struct TestIrHir {};struct TestIrTypes {};struct TestIrCpp {};struct TestIrIntegration {};struct TestIrTooling {};using TestIrLevel = std::variant<TestIrSyntax, TestIrHir, TestIrTypes, TestIrCpp, TestIrIntegration, TestIrTooling>;

mlc::Array<test_runner::TestResult> append_suite_results(mlc::Array<test_runner::TestResult>& accumulator, mlc::Array<test_runner::TestResult> suite) noexcept;

mlc::Array<test_runner::TestResult> syntax_suite_results() noexcept;

mlc::Array<test_runner::TestResult> hir_suite_results() noexcept;

mlc::Array<test_runner::TestResult> types_suite_results() noexcept;

mlc::Array<test_runner::TestResult> cpp_suite_results() noexcept;

mlc::Array<test_runner::TestResult> integration_suite_results() noexcept;

mlc::Array<test_runner::TestResult> tooling_suite_results() noexcept;

mlc::Array<test_runner::TestResult> all_test_suites() noexcept;

} // namespace suite_registry

#endif // SUITE_REGISTRY_HPP

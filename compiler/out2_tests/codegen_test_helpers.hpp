#ifndef CODEGEN_TEST_HELPERS_HPP
#define CODEGEN_TEST_HELPERS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "eval.hpp"
#include "type_gen.hpp"
#include "ast_builders.hpp"

namespace codegen_test_helpers {

context::CodegenContext empty_codegen_context() noexcept;
test_runner::TestResult assert_expr_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SExpr> expr, mlc::String expected_code) noexcept;
test_runner::TestResult assert_type_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr, mlc::String expected_code) noexcept;
test_runner::TestResult assert_code_contains(mlc::String test_name, mlc::String actual_code, mlc::String expected_substring) noexcept;
bool chars_match_at(mlc::String haystack, int start_position, mlc::String needle, int needle_length) noexcept;
bool string_contains(mlc::String haystack, mlc::String needle) noexcept;

} // namespace codegen_test_helpers

#endif // CODEGEN_TEST_HELPERS_HPP

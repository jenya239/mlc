#ifndef GOLDEN_HARNESS_HPP
#define GOLDEN_HARNESS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "test_runner.hpp"
#include "path_from_root.hpp"

namespace golden_harness {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool golden_relative_path_is_safe(mlc::String relative_path) noexcept;

ast::Result<mlc::String, mlc::String> read_golden_fixture(mlc::String relative_path) noexcept;

test_runner::TestResult assert_golden_string(mlc::String test_name, mlc::String actual, mlc::String golden_relative_path) noexcept;

} // namespace golden_harness

#endif // GOLDEN_HARNESS_HPP

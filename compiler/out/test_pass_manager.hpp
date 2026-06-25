#ifndef TEST_PASS_MANAGER_HPP
#define TEST_PASS_MANAGER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "pass_manager.hpp"
#include "preserved_analyses.hpp"

namespace test_pass_manager {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> pass_manager_tests() noexcept;

} // namespace test_pass_manager

#endif // TEST_PASS_MANAGER_HPP

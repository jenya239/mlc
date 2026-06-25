#ifndef TEST_GOLDEN_HARNESS_HPP
#define TEST_GOLDEN_HARNESS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "golden_harness.hpp"
#include "path_from_root.hpp"

namespace test_golden_harness {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> golden_harness_tests() noexcept;

} // namespace test_golden_harness

#endif // TEST_GOLDEN_HARNESS_HPP

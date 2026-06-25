#ifndef TEST_DRIVER_HPP
#define TEST_DRIVER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "path_normalize.hpp"
#include "compile_driver.hpp"

namespace test_driver {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> driver_tests() noexcept;

} // namespace test_driver

#endif // TEST_DRIVER_HPP

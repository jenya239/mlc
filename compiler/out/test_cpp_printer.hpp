#ifndef TEST_CPP_PRINTER_HPP
#define TEST_CPP_PRINTER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_ast.hpp"
#include "module_tu_helpers.hpp"
#include "expr.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_printer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_printer_tests() noexcept;

} // namespace test_cpp_printer

#endif // TEST_CPP_PRINTER_HPP

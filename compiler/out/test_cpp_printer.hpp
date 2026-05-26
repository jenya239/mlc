#ifndef TEST_CPP_PRINTER_HPP
#define TEST_CPP_PRINTER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_printer.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_printer {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::Array<test_runner::TestResult> cpp_printer_tests() noexcept;

} // namespace test_cpp_printer

#endif // TEST_CPP_PRINTER_HPP

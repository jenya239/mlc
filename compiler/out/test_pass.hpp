#ifndef TEST_PASS_HPP
#define TEST_PASS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"

namespace test_pass {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::Array<test_runner::TestResult> pass_type_tests() noexcept;

} // namespace test_pass

#endif // TEST_PASS_HPP

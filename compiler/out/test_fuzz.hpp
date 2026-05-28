#ifndef TEST_FUZZ_HPP
#define TEST_FUZZ_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "random_program.hpp"

namespace test_fuzz {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept;

} // namespace test_fuzz

#endif // TEST_FUZZ_HPP

#ifndef TEST_PARTIAL_APPLICATION_HPP
#define TEST_PARTIAL_APPLICATION_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "codegen_test_helpers.hpp"
#include "module.hpp"

namespace test_partial_application {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::Array<test_runner::TestResult> partial_application_tests() noexcept;

} // namespace test_partial_application

#endif // TEST_PARTIAL_APPLICATION_HPP

#ifndef TEST_MERGE_IMPORTS_HPP
#define TEST_MERGE_IMPORTS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "trait_param_expand.hpp"

namespace test_merge_imports {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<test_runner::TestResult> merge_import_tests() noexcept;

} // namespace test_merge_imports

#endif // TEST_MERGE_IMPORTS_HPP

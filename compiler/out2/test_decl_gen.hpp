#ifndef TEST_DECL_GEN_HPP
#define TEST_DECL_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"

namespace test_decl_gen {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

} // namespace test_decl_gen

#endif // TEST_DECL_GEN_HPP

#ifndef TEST_VISITOR_PASS_PARITY_HPP
#define TEST_VISITOR_PASS_PARITY_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "registry.hpp"
#include "expr_visitor_cpp.hpp"
#include "cpp_ast.hpp"
#include "ast_builders.hpp"

namespace test_visitor_pass_parity {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> visitor_pass_parity_tests() noexcept;

} // namespace test_visitor_pass_parity

#endif // TEST_VISITOR_PASS_PARITY_HPP

#ifndef TEST_VERIFY_IR_HPP
#define TEST_VERIFY_IR_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "verify_ast.hpp"
#include "verify_semantic_ir.hpp"
#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"

namespace test_verify_ir {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> verify_ir_tests() noexcept;

} // namespace test_verify_ir

#endif // TEST_VERIFY_IR_HPP

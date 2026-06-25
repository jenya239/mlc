#ifndef TEST_VERIFY_CPP_AST_HPP
#define TEST_VERIFY_CPP_AST_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "verify_cpp_ast.hpp"
#include "cpp_ast.hpp"

namespace test_verify_cpp_ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> verify_cpp_ast_tests() noexcept;

} // namespace test_verify_cpp_ast

#endif // TEST_VERIFY_CPP_AST_HPP

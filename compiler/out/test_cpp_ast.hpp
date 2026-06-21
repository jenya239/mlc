#ifndef TEST_CPP_AST_HPP
#define TEST_CPP_AST_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_ast_tests() noexcept;

} // namespace test_cpp_ast

#endif // TEST_CPP_AST_HPP

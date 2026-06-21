#ifndef TEST_CPP_TYPES_HPP
#define TEST_CPP_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_types.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_type_parser_tests() noexcept;

} // namespace test_cpp_types

#endif // TEST_CPP_TYPES_HPP

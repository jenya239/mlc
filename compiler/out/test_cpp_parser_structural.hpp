#ifndef TEST_CPP_PARSER_STRUCTURAL_HPP
#define TEST_CPP_PARSER_STRUCTURAL_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_parser_structural {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_parser_structural_tests() noexcept;

} // namespace test_cpp_parser_structural

#endif // TEST_CPP_PARSER_STRUCTURAL_HPP

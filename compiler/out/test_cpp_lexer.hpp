#ifndef TEST_CPP_LEXER_HPP
#define TEST_CPP_LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_tokens.hpp"

namespace test_cpp_lexer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept;

} // namespace test_cpp_lexer

#endif // TEST_CPP_LEXER_HPP

#ifndef TEST_CPP_LEXER_HPP
#define TEST_CPP_LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"

namespace test_cpp_lexer {

struct Expr;
struct Stmt;

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept;

} // namespace test_cpp_lexer

#endif // TEST_CPP_LEXER_HPP

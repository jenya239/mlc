#ifndef TEST_LEXER_HPP
#define TEST_LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "ast_tokens.hpp"

namespace test_lexer {

int lex_token_count(mlc::String source) noexcept;
ast_tokens::TKind lex_first_kind(mlc::String source) noexcept;
mlc::String lex_first_string(mlc::String source) noexcept;
mlc::Array<mlc::String> lex_template_parts(mlc::String source) noexcept;
int lex_error_count(mlc::String source) noexcept;
mlc::Array<test_runner::TestResult> lexer_tests() noexcept;

} // namespace test_lexer

#endif // TEST_LEXER_HPP

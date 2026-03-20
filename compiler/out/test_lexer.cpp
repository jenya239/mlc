#include "test_lexer.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"

namespace test_lexer {

using namespace test_runner;
using namespace lexer;

int lex_token_count(mlc::String source) noexcept;

ast_tokens::TKind lex_first_kind(mlc::String source) noexcept;

int lex_error_count(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> lexer_tests() noexcept;

int lex_token_count(mlc::String source) noexcept{return lexer::tokenize(source).tokens.size();}

ast_tokens::TKind lex_first_kind(mlc::String source) noexcept{return lexer::tokenize(source).tokens[0].kind;}

int lex_error_count(mlc::String source) noexcept{return lexer::tokenize(source).errors.size();}

mlc::Array<test_runner::TestResult> lexer_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty source - 1 token (Eof)"), lex_token_count(mlc::String("")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("whitespace only - 1 token (Eof)"), lex_token_count(mlc::String("   ")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("comment only - 1 token (Eof)"), lex_token_count(mlc::String("// hi")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("'fn' - 2 tokens"), lex_token_count(mlc::String("fn")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'type' - 2 tokens"), lex_token_count(mlc::String("type")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'let' - 2 tokens"), lex_token_count(mlc::String("let")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'if' - 2 tokens"), lex_token_count(mlc::String("if")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'match' - 2 tokens"), lex_token_count(mlc::String("match")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'end' - 2 tokens"), lex_token_count(mlc::String("end")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("integer literal - 2 tokens"), lex_token_count(mlc::String("42")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("string literal - 2 tokens"), lex_token_count(mlc::String("\"hi\"")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("identifier - 2 tokens"), lex_token_count(mlc::String("foo")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("no lex errors on valid source"), lex_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn decl - correct token count"), lex_token_count(mlc::String("fn f() -> i32 = 42")), 9));
return results;
}

} // namespace test_lexer

#include "test_lexer.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "ast_tokens.hpp"

namespace test_lexer {

using namespace test_runner;
using namespace lexer;
using namespace ast_tokens;

int lex_token_count(mlc::String source) noexcept;

ast_tokens::TKind lex_first_kind(mlc::String source) noexcept;

mlc::String lex_first_string(mlc::String source) noexcept;

mlc::Array<mlc::String> lex_template_parts(mlc::String source) noexcept;

int lex_error_count(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> lexer_tests() noexcept;

int lex_token_count(mlc::String source) noexcept{return lexer::tokenize(source).tokens.size();}

ast_tokens::TKind lex_first_kind(mlc::String source) noexcept{return lexer::tokenize(source).tokens[0].kind;}

mlc::String lex_first_string(mlc::String source) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LStr>(lexer::tokenize(source).tokens[0].kind)) { auto _v_lstr = std::get<ast_tokens::LStr>(lexer::tokenize(source).tokens[0].kind); auto [text] = _v_lstr; return text; } return mlc::String(""); }();}

mlc::Array<mlc::String> lex_template_parts(mlc::String source) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast_tokens::LTemplate>(lexer::tokenize(source).tokens[0].kind)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(lexer::tokenize(source).tokens[0].kind); auto [parts] = _v_ltemplate; return parts; } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> r = {};
  return r;
 }(); }();}

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
results.push_back(test_runner::assert_eq_str(mlc::String("string literal decodes escaped newline"), lex_first_string(mlc::String("\"a\\nb\"")), mlc::String("a\nb")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal decodes escaped quote"), lex_first_string(mlc::String("\"say \\\"hi\\\"\"")), mlc::String("say \"hi\"")));
results.push_back(test_runner::assert_eq_int(mlc::String("multiline double-quoted string - 2 tokens (LStr + Eof)"), lex_token_count(mlc::String("\"a\nb\"")), 2));
results.push_back(test_runner::assert_eq_str(mlc::String("multiline double-quoted string literal preserves embedded newline"), lex_first_string(mlc::String("\"a\nb\"")), mlc::String("a\nb")));
results.push_back(test_runner::assert_true(mlc::String("unterminated double-quoted string yields lex error"), lex_error_count(mlc::String("\"no closing quote")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("identifier - 2 tokens"), lex_token_count(mlc::String("foo")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("no lex errors on valid source"), lex_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn decl - correct token count"), lex_token_count(mlc::String("fn f() -> i32 = 42")), 9));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template - 2 tokens (LTemplate + Eof)"), lex_token_count(mlc::String("`hello`")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template parts count"), lex_template_parts(mlc::String("`hello`")).size(), 1));
results.push_back(test_runner::assert_eq_str(mlc::String("plain template content"), lex_template_parts(mlc::String("`hello`"))[0], mlc::String("hello")));
results.push_back(test_runner::assert_eq_int(mlc::String("template with interpolation parts count"), lex_template_parts(mlc::String("`a ${x} b`")).size(), 3));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[0]"), lex_template_parts(mlc::String("`a ${x} b`"))[0], mlc::String("a ")));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[1] (expr)"), lex_template_parts(mlc::String("`a ${x} b`"))[1], mlc::String("x")));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[2]"), lex_template_parts(mlc::String("`a ${x} b`"))[2], mlc::String(" b")));
results.push_back(test_runner::assert_true(mlc::String("unterminated template yields lex error"), lex_error_count(mlc::String("`no closing")) > 0));
results.push_back(test_runner::assert_true(mlc::String("unterminated interpolation yields lex error"), lex_error_count(mlc::String("`a ${b")) > 0));
return results;
}

} // namespace test_lexer

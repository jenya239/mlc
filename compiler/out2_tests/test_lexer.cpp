#define main mlc_user_main
#include "test_lexer.hpp"

namespace test_lexer {

int lex_token_count(mlc::String source) noexcept{
return lexer::tokenize(source).tokens.length();
}
ast_tokens::TKind lex_first_kind(mlc::String source) noexcept{
return lexer::tokenize(source).tokens[0].kind;
}
mlc::String lex_first_string(mlc::String source) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LStr& lStr) { auto [text] = lStr; return text; },
[&](const auto& __v) { return mlc::String("", 0); }
}, lexer::tokenize(source).tokens[0].kind);
}
mlc::Array<mlc::String> lex_template_parts(mlc::String source) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LTemplate& lTemplate) { auto [parts] = lTemplate; return parts; },
[&](const auto& __v) { return [&]() {
auto r = mlc::Array<mlc::String>{};
return r;
}(); }
}, lexer::tokenize(source).tokens[0].kind);
}
int lex_error_count(mlc::String source) noexcept{
return lexer::tokenize(source).errors.length();
}
mlc::Array<test_runner::TestResult> lexer_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
results.push_back(test_runner::assert_eq_int(mlc::String("empty source - 1 token (Eof)", 28), lex_token_count(mlc::String("", 0)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("whitespace only - 1 token (Eof)", 31), lex_token_count(mlc::String("   ", 3)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("comment only - 1 token (Eof)", 28), lex_token_count(mlc::String("// hi", 5)), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("'fn' - 2 tokens", 15), lex_token_count(mlc::String("fn", 2)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'type' - 2 tokens", 17), lex_token_count(mlc::String("type", 4)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'let' - 2 tokens", 16), lex_token_count(mlc::String("let", 3)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'if' - 2 tokens", 15), lex_token_count(mlc::String("if", 2)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'match' - 2 tokens", 18), lex_token_count(mlc::String("match", 5)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'end' - 2 tokens", 16), lex_token_count(mlc::String("end", 3)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("integer literal - 2 tokens", 26), lex_token_count(mlc::String("42", 2)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("string literal - 2 tokens", 25), lex_token_count(mlc::String("\"hi\"", 4)), 2));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal decodes escaped newline", 38), lex_first_string(mlc::String("\"a\\nb\"", 6)), mlc::String("a\nb", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal decodes escaped quote", 36), lex_first_string(mlc::String("\"say \\\"hi\\\"\"", 12)), mlc::String("say \"hi\"", 8)));
results.push_back(test_runner::assert_eq_int(mlc::String("multiline double-quoted string - 2 tokens (LStr + Eof)", 54), lex_token_count(mlc::String("\"a\nb\"", 5)), 2));
results.push_back(test_runner::assert_eq_str(mlc::String("multiline double-quoted string literal preserves embedded newline", 65), lex_first_string(mlc::String("\"a\nb\"", 5)), mlc::String("a\nb", 3)));
results.push_back(test_runner::assert_true(mlc::String("unterminated double-quoted string yields lex error", 50), (lex_error_count(mlc::String("\"no closing quote", 17)) > 0)));
results.push_back(test_runner::assert_eq_int(mlc::String("identifier - 2 tokens", 21), lex_token_count(mlc::String("foo", 3)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("no lex errors on valid source", 29), lex_error_count(mlc::String("fn f() -> i32 = 42", 18)), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn decl - correct token count", 29), lex_token_count(mlc::String("fn f() -> i32 = 42", 18)), 9));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template - 2 tokens (LTemplate + Eof)", 43), lex_token_count(mlc::String("`hello`", 7)), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template parts count", 26), lex_template_parts(mlc::String("`hello`", 7)).length(), 1));
results.push_back(test_runner::assert_eq_str(mlc::String("plain template content", 22), lex_template_parts(mlc::String("`hello`", 7))[0], mlc::String("hello", 5)));
results.push_back(test_runner::assert_eq_int(mlc::String("template with interpolation parts count", 39), lex_template_parts(mlc::String("`a \\${x} b`", 11)).length(), 3));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[0]", 23), lex_template_parts(mlc::String("`a \\${x} b`", 11))[0], mlc::String("a ", 2)));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[1] (expr)", 30), lex_template_parts(mlc::String("`a \\${x} b`", 11))[1], mlc::String("x", 1)));
results.push_back(test_runner::assert_eq_str(mlc::String("template interp part[2]", 23), lex_template_parts(mlc::String("`a \\${x} b`", 11))[2], mlc::String(" b", 2)));
results.push_back(test_runner::assert_true(mlc::String("unterminated template yields lex error", 38), (lex_error_count(mlc::String("`no closing", 11)) > 0)));
results.push_back(test_runner::assert_true(mlc::String("unterminated interpolation yields lex error", 43), (lex_error_count(mlc::String("`a \\${b", 7)) > 0)));
return results;
}

} // namespace test_lexer

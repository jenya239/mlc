#ifndef CPP_LEXER_HPP
#define CPP_LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "lexer.hpp"
#include "cpp_tokens.hpp"
#include "cpp_tokens.hpp"

namespace cpp_lexer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CppLexOut {mlc::Array<cpp_tokens::CppToken> tokens;mlc::Array<mlc::String> errors;};

cpp_lexer::CppLexOut cpp_tokenize(mlc::String input_text) noexcept;

bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept;

bool cpp_lex_roundtrip_preserves_source(mlc::String source_text) noexcept;

} // namespace cpp_lexer

#endif // CPP_LEXER_HPP

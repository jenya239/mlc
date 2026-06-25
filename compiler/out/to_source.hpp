#ifndef TO_SOURCE_HPP
#define TO_SOURCE_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"
#include "print.hpp"
#include "cpp_tokens.hpp"
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_decls.hpp"
#include "cpp_predicates.hpp"

namespace to_source {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String cpp_tokens_span_to_source(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;

mlc::String cpp_declaration_to_source(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String cpp_program_to_source(cpp_ast::CppProgram program) noexcept;

cpp_ast::CppProgram cpp_program_from_tokens_preserving_trivia(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_source_roundtrip_preserving_trivia(mlc::String source_text) noexcept;

mlc::String cpp_parse_to_source_canonical(mlc::String source_text) noexcept;

} // namespace to_source

#endif // TO_SOURCE_HPP

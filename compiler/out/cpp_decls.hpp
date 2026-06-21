#ifndef CPP_DECLS_HPP
#define CPP_DECLS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_types.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"

namespace cpp_decls {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CppDeclarationParseResult {std::shared_ptr<cpp_ast::CppDeclaration> declaration;int position;mlc::String error_message;};

cpp_decls::CppDeclarationParseResult parse_cpp_declaration(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

} // namespace cpp_decls

#endif // CPP_DECLS_HPP

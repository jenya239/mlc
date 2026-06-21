#ifndef CPP_TYPES_HPP
#define CPP_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"

namespace cpp_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CppTypedParseResult {std::shared_ptr<cpp_ast::CppType> type_node;int position;};

struct CppTypeStringResult {mlc::String type_string;int position;};

cpp_types::CppTypedParseResult parse_cpp_type(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_types::CppTypeStringResult parse_cpp_type_to_string(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

} // namespace cpp_types

#endif // CPP_TYPES_HPP

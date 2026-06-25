#ifndef CPP_EXPRS_HPP
#define CPP_EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "lexer.hpp"
#include "cpp_predicates.hpp"

namespace cpp_exprs {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CppExpressionParseResult {std::shared_ptr<cpp_ast::CppExpression> expression;int position;mlc::String error_message;};

cpp_exprs::CppExpressionParseResult parse_cpp_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

} // namespace cpp_exprs

#endif // CPP_EXPRS_HPP

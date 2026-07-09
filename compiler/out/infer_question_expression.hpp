#ifndef INFER_QUESTION_EXPRESSION_HPP
#define INFER_QUESTION_EXPRESSION_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"

namespace infer_question_expression {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept;

} // namespace infer_question_expression

#endif // INFER_QUESTION_EXPRESSION_HPP

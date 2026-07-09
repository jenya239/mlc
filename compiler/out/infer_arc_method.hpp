#ifndef INFER_ARC_METHOD_HPP
#define INFER_ARC_METHOD_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "send_safe.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_arc_method {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool is_arc_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;

infer_result::InferResult infer_arc_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_arc_method

#endif // INFER_ARC_METHOD_HPP

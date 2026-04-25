#ifndef INFER_ARRAY_METHOD_HPP
#define INFER_ARRAY_METHOD_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "type_diagnostics.hpp"

namespace infer_array_method {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

} // namespace infer_array_method

#endif // INFER_ARRAY_METHOD_HPP

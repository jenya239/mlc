#ifndef INFER_WEAK_METHOD_HPP
#define INFER_WEAK_METHOD_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "type_diagnostics.hpp"

namespace infer_weak_method {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool is_shared_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;

infer_result::InferResult infer_shared_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool is_shared_weak_method(mlc::String method_name) noexcept;

bool is_weak_upgrade_method(mlc::String method_name) noexcept;

bool is_shared_weak_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

bool is_weak_upgrade_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

infer_result::InferResult infer_shared_weak_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept;

infer_result::InferResult infer_weak_upgrade_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept;

} // namespace infer_weak_method

#endif // INFER_WEAK_METHOD_HPP

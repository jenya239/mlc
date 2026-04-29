#ifndef METHOD_GEN_HPP
#define METHOD_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "expr.hpp"

namespace method_gen {

using EvalExprFn = std::variant<>;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
mlc::String gen_arg_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept;
bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;
mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_result_option_combinator_call(bool is_option, mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;
mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept;

} // namespace method_gen

#endif // METHOD_GEN_HPP

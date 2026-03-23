#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "names.hpp"
#include "registry.hpp"

namespace infer {

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<mlc::String> errors;};
struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<mlc::String> errors;};
struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};
InferResult InferResult_with_type(InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept;
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult stmt_result) noexcept;
InferResult infer_ok(std::shared_ptr<registry::Type> type_val) noexcept;
std::shared_ptr<registry::Type> binary_op_type(mlc::String op, std::shared_ptr<registry::Type> left_type) noexcept;
std::shared_ptr<registry::Type> method_return_type(mlc::String method_name) noexcept;
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> exprs, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
std::shared_ptr<registry::Type> field_lookup_type(std::shared_ptr<registry::Type> obj_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;
InferResult infer_expr(std::shared_ptr<ast::Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> type_val, registry::TypeRegistry registry) noexcept;
bool CheckOut_has_errors(CheckOut self) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program prog) noexcept;

} // namespace infer

#endif // INFER_HPP

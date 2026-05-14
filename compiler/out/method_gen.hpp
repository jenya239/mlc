#ifndef METHOD_GEN_HPP
#define METHOD_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "expr.hpp"

namespace method_gen {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

} // namespace method_gen

#endif // METHOD_GEN_HPP

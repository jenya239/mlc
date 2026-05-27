#ifndef MATCH_GEN_HPP
#define MATCH_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"
#include "registry.hpp"
#include "type_gen.hpp"
#include "literals.hpp"

namespace match_gen {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms) noexcept;

bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

} // namespace match_gen

#endif // MATCH_GEN_HPP

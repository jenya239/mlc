#ifndef MATCH_GEN_HPP
#define MATCH_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"
#include "registry.hpp"
#include "type_gen.hpp"
#include "literals.hpp"

namespace match_gen {

struct RecordFieldBindAccum {mlc::String field_bindings;context::CodegenContext arm_context;};
RecordFieldBindAccum record_field_bind_step(RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pat> field_pattern, mlc::String lower_name) noexcept;
RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept;
context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, context::CodegenContext base_context) noexcept;
mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
std::shared_ptr<semantic_ir::SMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SMatchArm> arm, std::shared_ptr<ast::Pat> pattern) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SMatchArm> arm) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms) noexcept;
bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms) noexcept;
mlc::String guarded_literal_condition(std::shared_ptr<ast::Pat> pattern, mlc::String variant_access_expression) noexcept;
mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

} // namespace match_gen

#endif // MATCH_GEN_HPP

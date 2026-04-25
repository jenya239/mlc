#include "match_gen.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"

namespace match_gen {

using namespace ast;
using namespace semantic_ir;
using namespace context;
using namespace cpp_naming;
using namespace match_analysis;
using namespace expr;
using namespace ast_tokens;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, ctor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(ctor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name);
std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info = ctor_info::lookup_ctor_type_info(context.ctor_type_infos, ctor_name);
bool is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
context::CodegenContext arm_context = std::move(context);
int index = 0;
while (index < sub_patterns.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[index])); auto [pattern_name, _w0] = _v_patident; return [&]() { 
  if (ctor_type_info->shared_pos.contains(index)){
{
arm_context = context::context_add_shared(arm_context, pattern_name);
}
}
  if (ctor_type_info->shared_arr_pos.contains(index)){
arm_context = context::context_add_shared_array(arm_context, pattern_name);
}
 }(); } return; }();
index = index + 1;
}
}
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, eval_expr_fn(arm_body, arm_context, gen_stmts));
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
mlc::String field_bindings = mlc::String("");
int field_index = 0;
while (field_index < field_patterns.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [field_name, _w0] = patident;
{
field_bindings = field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*field_patterns[field_index]));
field_index = field_index + 1;
}
}
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), field_bindings, eval_expr_fn(arm_body, context, gen_stmts));
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatIdent& patident) -> mlc::String { auto [name, _w0] = patident; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
  [&](const PatTuple& pattuple) -> mlc::String { auto [_w0, _w1] = pattuple; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatArray& patarray) -> mlc::String { auto [_w0, _w1, _w2] = patarray; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); }
}, (*arm->pat));}

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < arms.size()){
{
parts.push_back(gen_arm(arms[index], context, gen_stmts, eval_expr_fn));
index = index + 1;
}
}
mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
bool needs_deref = arms.size() > 0 ? match_analysis::first_arm_needs_deref(arms[0], subject, context) : true;
return expr::std_visit_match_expression(parts.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_deref));
}

} // namespace match_gen

#define main mlc_user_main
#include "match_gen.hpp"

namespace match_gen {

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = context::context_resolve(context, ctor_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(ctor_name));
auto binding = ((sub_patterns.length() == 0) ? mlc::String("", 0) : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name));
auto ctor_type_info = ctor_info::lookup_ctor_type_info(context.ctor_type_infos, ctor_name);
auto is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
auto type_argument = (is_generic ? mlc::String("<auto>", 6) : mlc::String("", 0));
auto arm_context = context;
auto index = 0;
while ((index < sub_patterns.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [pattern_name, __1] = patIdent; return [&]() {
if (ctor_type_info->shared_pos.contains(index)) {
arm_context = context::context_add_shared(arm_context, pattern_name);
}
if (ctor_type_info->shared_arr_pos.contains(index)) {
arm_context = context::context_add_shared_array(arm_context, pattern_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*sub_patterns[index]));
index = (index + 1);
}
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, eval_expr_fn(arm_body, arm_context, gen_stmts));
}
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = context::context_resolve(context, record_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
auto is_generic = decl_index::list_contains(context.generic_variants, record_name);
auto type_argument = (is_generic ? mlc::String("<auto>", 6) : mlc::String("", 0));
auto field_bindings = mlc::String("", 0);
auto field_index = 0;
while ((field_index < field_patterns.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [field_name, __1] = patIdent; return [&]() {
field_bindings = (field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*field_patterns[field_index]));
field_index = (field_index + 1);
}
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), field_bindings, eval_expr_fn(arm_body, context, gen_stmts));
}
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms) noexcept{
auto result = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{};
auto i = 0;
while ((i < arms.length())) {
std::visit(overloaded{[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return [&]() {
auto j = 0;
return [&]() {
while ((j < alts.length())) {
result.push_back(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{alts[j], arms[i]->body}));
j = (j + 1);
}
}();
}(); },
[&](const auto& __v) { return result.push_back(arms[i]); }
}, (*arms[i]->pat));
i = (i + 1);
}
return result;
}
mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { auto [__0] = patWild; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatUnit& patUnit) { auto [__0] = patUnit; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatBool& patBool) { auto [__0, __1] = patBool; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatInt& patInt) { auto [__0, __1] = patInt; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatStr& patStr) { auto [__0, __1] = patStr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatIdent& patIdent) { auto [name, __1] = patIdent; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatCtor& patCtor) { auto [name, sub_patterns, __2] = patCtor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatRecord& patRecord) { auto [name, field_patterns, __2] = patRecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatTuple& patTuple) { auto [__0, __1] = patTuple; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatArray& patArray) { auto [__0, __1, __2] = patArray; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatOr& patOr) { auto [__0, __1] = patOr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); }
}, (*arm->pat));
}
mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto expanded = expand_or_arms(arms);
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < expanded.length())) {
parts.push_back(gen_arm(expanded[index], context, gen_stmts, eval_expr_fn));
index = (index + 1);
}
auto subject_code = eval_expr_fn(subject, context, gen_stmts);
auto needs_deref = ((expanded.length() > 0) ? match_analysis::first_arm_needs_deref(expanded[0], subject, context) : true);
return expr::std_visit_match_expression(parts.join(mlc::String(",\n", 2)), expr::visit_subject_for_match(subject_code, needs_deref));
}

} // namespace match_gen

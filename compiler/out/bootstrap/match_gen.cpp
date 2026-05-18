#define main mlc_user_main
#include "match_gen.hpp"

namespace match_gen {

RecordFieldBindAccum record_field_bind_step(RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pat> field_pattern, mlc::String lower_name) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [field_name, __1] = patIdent; return RecordFieldBindAccum{(accumulated.field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name)), CodegenContext_add_value(accumulated.arm_context, field_name)}; },
[&](const auto& __v) { return accumulated; }
}, (*field_pattern));
}
RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept{
return field_patterns.fold(RecordFieldBindAccum{mlc::String("", 0), start_context}, [=](RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pat> field_pattern) mutable { return record_field_bind_step(accumulated, field_pattern, lower_name); });
}
context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, context::CodegenContext base_context) noexcept{
auto ctor_type_information = ctor_info::lookup_ctor_type_info(base_context.ctor_type_infos, constructor_name);
auto arm_context = base_context;
auto pattern_index = 0;
while ((pattern_index < sub_patterns.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [pattern_name, __1] = patIdent; return [&]() {
if (ctor_type_information->shared_pos.contains(pattern_index)) {
arm_context = CodegenContext_add_shared(arm_context, pattern_name);
}
if (ctor_type_information->shared_arr_pos.contains(pattern_index)) {
arm_context = CodegenContext_add_shared_array(arm_context, pattern_name);
}
if (((!ctor_type_information->shared_pos.contains(pattern_index)) && (!ctor_type_information->shared_arr_pos.contains(pattern_index)))) {
arm_context = CodegenContext_add_value(arm_context, pattern_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*sub_patterns[pattern_index]));
pattern_index = (pattern_index + 1);
}
return arm_context;
}
mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = CodegenContext_resolve(context, ctor_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(ctor_name));
auto binding = ((sub_patterns.length() == 0) ? (mlc::String("", 0)) : (expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name)));
auto is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
auto type_argument = (is_generic ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
auto arm_context = codegen_context_with_ctor_field_bindings(ctor_name, sub_patterns, context);
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, eval_expr_fn(arm_body, arm_context, gen_stmts));
}
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = CodegenContext_resolve(context, record_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
auto is_generic = decl_index::list_contains(context.generic_variants, record_name);
auto type_argument = (is_generic ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
auto accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), accumulated.field_bindings, eval_expr_fn(arm_body, accumulated.arm_context, gen_stmts));
}
std::shared_ptr<semantic_ir::SMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SMatchArm> arm, std::shared_ptr<ast::Pat> pattern) noexcept{
return std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});
}
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SMatchArm> arm) noexcept{
return std::visit(overloaded{[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return alts.map([=](std::shared_ptr<ast::Pat> alt) mutable { return match_arm_with_pattern(arm, alt); }); },
[&](const auto& __v) { return mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{arm}; }
}, (*arm->pat));
}
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms) noexcept{
return arms.flat_map([=](std::shared_ptr<semantic_ir::SMatchArm> arm) mutable { return singleton_or_split(arm); });
}
bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms) noexcept{
return expanded_arms.any([=](std::shared_ptr<semantic_ir::SMatchArm> arm) mutable { return arm->has_guard; });
}
mlc::String guarded_literal_condition(std::shared_ptr<ast::Pat> pattern, mlc::String variant_access_expression) noexcept{
return std::visit(overloaded{[&](const ast::PatBool& patBool) { auto [value, __1] = patBool; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_boolean_literal(value))) + mlc::String(")", 1)); },
[&](const ast::PatInt& patInt) { auto [value, __1] = patInt; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_integer_literal(value))) + mlc::String(")", 1)); },
[&](const ast::PatStr& patStr) { auto [value, __1] = patStr; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_string_literal(value))) + mlc::String(")", 1)); },
[&](const auto& __v) { return mlc::String("false", 5); }
}, (*pattern));
}
mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto body_code = eval_expr_fn(arm->body, context, gen_stmts);
if (arm->has_guard) {
auto guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
return (expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n", 1));
} else {
return expr::return_line(body_code);
}
}
mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto condition = guarded_literal_condition(arm->pat, variant_access_expression);
auto body_code = eval_expr_fn(arm->body, context, gen_stmts);
if (arm->has_guard) {
auto guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
auto combined_condition = ((((mlc::String("(", 1) + mlc::to_string(condition)) + mlc::String(" && (", 5)) + mlc::to_string(guard_code)) + mlc::String("))", 2));
return (expr::if_brace_block(combined_condition, expr::return_line(body_code)) + mlc::String("\n", 1));
} else {
return (expr::if_brace_block(condition, expr::return_line(body_code)) + mlc::String("\n", 1));
}
}
mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto binding_cpp_safe = cpp_naming::cpp_safe(binding_name);
auto extended_context = CodegenContext_add_value(context, binding_name);
auto binding_statement = ((((mlc::String("auto ", 5) + mlc::to_string(binding_cpp_safe)) + mlc::String(" = ", 3)) + mlc::to_string(subject_holder_name)) + mlc::String(";\n", 2));
auto body_code = eval_expr_fn(arm->body, extended_context, gen_stmts);
if (arm->has_guard) {
auto guard_code = eval_expr_fn(arm->when_condition, extended_context, gen_stmts);
return (((mlc::String("{\n", 2) + binding_statement) + expr::if_brace_block(guard_code, expr::return_line(body_code))) + mlc::String("}\n", 2));
} else {
return (((mlc::String("{\n", 2) + binding_statement) + expr::return_line(body_code)) + mlc::String("}\n", 2));
}
}
mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = CodegenContext_resolve(context, constructor_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
auto binding_prefix = ((sub_patterns.length() == 0) ? (mlc::String("", 0)) : (expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name)));
auto type_argument = (decl_index::list_contains(context.generic_variants, constructor_name) ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
auto arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
auto holds_check = ((((((mlc::String("std::holds_alternative<", 23) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2)) + mlc::to_string(variant_access_expression)) + mlc::String(")", 1));
auto open_block = ((((((mlc::String("if (", 4) + mlc::to_string(holds_check)) + mlc::String(") {\n", 4)) + ((((((((((mlc::String("const ", 6) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String("& ", 2)) + mlc::to_string(lower_name)) + mlc::String(" = std::get<", 12)) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2))) + variant_access_expression) + mlc::String(");\n", 3)) + binding_prefix);
auto body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
auto inner = (arm->has_guard ? ([&]() {
auto guard_code = eval_expr_fn(arm->when_condition, arm_context, gen_stmts);
return (expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n", 1));
}()) : (expr::return_line(body_code)));
return ((open_block + inner) + mlc::String("}\n", 2));
}
mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto qualified_name = CodegenContext_resolve(context, record_name);
auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
auto type_argument = (decl_index::list_contains(context.generic_variants, record_name) ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
auto accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
auto holds_check = ((((((mlc::String("std::holds_alternative<", 23) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2)) + mlc::to_string(variant_access_expression)) + mlc::String(")", 1));
auto open_block = ((((((mlc::String("if (", 4) + mlc::to_string(holds_check)) + mlc::String(") {\n", 4)) + ((((((((((mlc::String("const ", 6) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String("& ", 2)) + mlc::to_string(lower_name)) + mlc::String(" = std::get<", 12)) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2))) + variant_access_expression) + mlc::String(");\n", 3)) + accumulated.field_bindings);
auto body_code = eval_expr_fn(arm->body, accumulated.arm_context, gen_stmts);
auto inner = (arm->has_guard ? ([&]() {
auto guard_code = eval_expr_fn(arm->when_condition, accumulated.arm_context, gen_stmts);
return (expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n", 1));
}()) : (expr::return_line(body_code)));
return ((open_block + inner) + mlc::String("}\n", 2));
}
mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [name, sub_patterns, __2] = patCtor; return gen_guarded_constructor_arm_fragment(arm, name, sub_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatRecord& patRecord) { auto [name, field_patterns, __2] = patRecord; return gen_guarded_record_arm_fragment(arm, name, field_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatWild& patWild) { auto [__0] = patWild; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatUnit& patUnit) { auto [__0] = patUnit; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatBool& patBool) { auto [__0, __1] = patBool; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatInt& patInt) { auto [__0, __1] = patInt; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatStr& patStr) { auto [__0, __1] = patStr; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return gen_guarded_identifier_binding_arm_fragment(arm, binding_name, subject_holder_name, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatTuple& patTuple) { auto [__0, __1] = patTuple; return mlc::String("", 0); },
[&](const ast::PatArray& patArray) { auto [__0, __1, __2] = patArray; return mlc::String("", 0); },
[&](const ast::PatOr& patOr) { auto [__0, __1] = patOr; return mlc::String("", 0); }
}, (*arm->pat));
}
mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto subject_code = eval_expr_fn(subject, context, gen_stmts);
auto needs_dereference = ((expanded_arms.length() > 0) ? (match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context)) : (true));
auto return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
auto variant_access = expr::visit_subject_for_match(mlc::String("__match_subject", 15), needs_dereference);
auto fragment_lines = expanded_arms.map([=](std::shared_ptr<semantic_ir::SMatchArm> arm) mutable { return gen_guarded_arm_fragment(arm, variant_access, mlc::String("__match_subject", 15), context, gen_stmts, eval_expr_fn); });
auto inner_body = ((((mlc::String("auto __match_subject = ", 23) + mlc::to_string(subject_code)) + mlc::String(";\n", 2)) + fragment_lines.join(mlc::String("", 0))) + mlc::String("std::abort();\n", 14));
return ((((mlc::String("[&]() -> ", 9) + mlc::to_string(return_cpp)) + mlc::String(" {\n", 3)) + mlc::to_string(inner_body)) + mlc::String("}()", 3));
}
mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { auto [__0] = patWild; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatUnit& patUnit) { auto [__0] = patUnit; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatBool& patBool) { auto [__0, __1] = patBool; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatInt& patInt) { auto [__0, __1] = patInt; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatStr& patStr) { auto [__0, __1] = patStr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatIdent& patIdent) { auto [name, __1] = patIdent; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, CodegenContext_add_value(context, name), gen_stmts)); },
[&](const ast::PatCtor& patCtor) { auto [name, sub_patterns, __2] = patCtor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatRecord& patRecord) { auto [name, field_patterns, __2] = patRecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatTuple& patTuple) { auto [__0, __1] = patTuple; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatArray& patArray) { auto [__0, __1, __2] = patArray; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
[&](const ast::PatOr& patOr) { auto [__0, __1] = patOr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); }
}, (*arm->pat));
}
mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto expanded = expand_or_arms(arms);
if (expanded_any_guard(expanded)) {
return gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn);
} else {
auto arm_lines = expanded.map([=](std::shared_ptr<semantic_ir::SMatchArm> arm) mutable { return gen_arm(arm, context, gen_stmts, eval_expr_fn); });
auto subject_code = eval_expr_fn(subject, context, gen_stmts);
auto needs_deref = ((expanded.length() > 0) ? (match_analysis::first_arm_needs_deref(expanded[0], subject, context)) : (true));
return expr::std_visit_match_expression(arm_lines.join(mlc::String(",\n", 2)), expr::visit_subject_for_match(subject_code, needs_deref));
}
}

} // namespace match_gen

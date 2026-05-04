#include "match_gen.hpp"

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

using namespace ast;
using namespace semantic_ir;
using namespace context;
using namespace cpp_naming;
using namespace match_analysis;
using namespace expr;
using namespace registry;
using namespace type_gen;
using namespace literals;
using namespace ast_tokens;

context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, context::CodegenContext base_context) noexcept;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

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

context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, context::CodegenContext base_context) noexcept{
std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_information = ctor_info::lookup_ctor_type_info(base_context.ctor_type_infos, constructor_name);
context::CodegenContext arm_context = std::move(base_context);
int pattern_index = 0;
while (pattern_index < sub_patterns.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[pattern_index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[pattern_index])); auto [pattern_name, _w0] = _v_patident; return [&]() { 
  if (ctor_type_information->shared_pos.contains(pattern_index)){
{
arm_context = context::context_add_shared(arm_context, pattern_name);
}
}
  if (ctor_type_information->shared_arr_pos.contains(pattern_index)){
{
arm_context = context::context_add_shared_array(arm_context, pattern_name);
}
}
  if (!ctor_type_information->shared_pos.contains(pattern_index) && !ctor_type_information->shared_arr_pos.contains(pattern_index)){
arm_context = context::context_add_value(arm_context, pattern_name);
}
 }(); } return; }();
pattern_index = pattern_index + 1;
}
}
return arm_context;
}

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, ctor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(ctor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name);
bool is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(ctor_name, sub_patterns, context);
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, eval_expr_fn(arm_body, arm_context, gen_stmts));
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
mlc::String field_bindings = mlc::String("");
context::CodegenContext arm_context = std::move(context);
int field_index = 0;
while (field_index < field_patterns.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [field_name, _w0] = patident;
{
field_bindings = field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name);
arm_context = context::context_add_value(arm_context, field_name);
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
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), field_bindings, eval_expr_fn(arm_body, arm_context, gen_stmts));
}

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> result = {};
int i = 0;
while (i < arms.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatOr>((*arms[i]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[i]->pat)); auto [alts, _w0] = _v_pator; return [&]() { 
  int j = 0;
  return [&]() { 
  while (j < alts.size()){
{
result.push_back(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{alts[j], arms[i]->has_guard, arms[i]->when_condition, arms[i]->body}));
j = j + 1;
}
}
 }();
 }(); } return result.push_back(arms[i]); }();
i = i + 1;
}
}
return result;
}

bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms) noexcept{
int arm_index = 0;
while (arm_index < expanded_arms.size()){
{
if (expanded_arms[arm_index]->has_guard){
{
return true;
}
}
arm_index = arm_index + 1;
}
}
return false;
}

mlc::String guarded_literal_condition(std::shared_ptr<ast::Pat> pattern, mlc::String variant_access_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::PatBool>((*pattern))) { auto _v_patbool = std::get<ast::PatBool>((*pattern)); auto [value, _w0] = _v_patbool; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_boolean_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatInt>((*pattern))) { auto _v_patint = std::get<ast::PatInt>((*pattern)); auto [value, _w0] = _v_patint; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_integer_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatStr>((*pattern))) { auto _v_patstr = std::get<ast::PatStr>((*pattern)); auto [value, _w0] = _v_patstr; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_string_literal(value) + mlc::String(")"); } return mlc::String("false"); }();}

mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
}

mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String condition = guarded_literal_condition(arm->pat, variant_access_expression);
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  mlc::String combined_condition = mlc::String("(") + condition + mlc::String(" && (") + guard_code + mlc::String("))");
  return expr::if_brace_block(combined_condition, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::if_brace_block(condition, expr::return_line(body_code)) + mlc::String("\n");
}

mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String binding_cpp_safe = cpp_naming::cpp_safe(binding_name);
context::CodegenContext extended_context = context::context_add_value(context, binding_name);
mlc::String binding_statement = mlc::String("auto ") + binding_cpp_safe + mlc::String(" = ") + subject_holder_name + mlc::String(";\n");
mlc::String body_code = eval_expr_fn(arm->body, extended_context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, extended_context, gen_stmts);
  return mlc::String("{\n") + binding_statement + expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("}\n");
 }() : mlc::String("{\n") + binding_statement + expr::return_line(body_code) + mlc::String("}\n");
}

mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding_prefix = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), lower_name);
mlc::String type_argument = decl_index::list_contains(context.generic_variants, constructor_name) ? mlc::String("<auto>") : mlc::String("");
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + binding_prefix;
mlc::String body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
mlc::String inner = arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, arm_context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::context_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
mlc::String type_argument = decl_index::list_contains(context.generic_variants, record_name) ? mlc::String("<auto>") : mlc::String("");
mlc::String field_bindings = mlc::String("");
context::CodegenContext arm_context = std::move(context);
int field_index = 0;
while (field_index < field_patterns.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [field_name, _w0] = patident;
{
field_bindings = field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name);
arm_context = context::context_add_value(arm_context, field_name);
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
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + field_bindings;
mlc::String body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
mlc::String inner = arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, arm_context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return std::visit(overloaded{
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_guarded_constructor_arm_fragment(arm, name, sub_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_guarded_record_arm_fragment(arm, name, field_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatIdent& patident) -> mlc::String { auto [binding_name, _w0] = patident; return gen_guarded_identifier_binding_arm_fragment(arm, binding_name, subject_holder_name, context, gen_stmts, eval_expr_fn); },
  [&](const PatTuple& pattuple) -> mlc::String { auto [_w0, _w1] = pattuple; return mlc::String(""); },
  [&](const PatArray& patarray) -> mlc::String { auto [_w0, _w1, _w2] = patarray; return mlc::String(""); },
  [&](const PatOr& pator) -> mlc::String { auto [_w0, _w1] = pator; return mlc::String(""); }
}, (*arm->pat));}

mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
mlc::String inner_body = mlc::String("auto __match_subject = ") + subject_code + mlc::String(";\n");
mlc::String variant_access = expr::visit_subject_for_match(mlc::String("__match_subject"), needs_dereference);
int arm_index = 0;
while (arm_index < expanded_arms.size()){
{
inner_body = inner_body + gen_guarded_arm_fragment(expanded_arms[arm_index], variant_access, mlc::String("__match_subject"), context, gen_stmts, eval_expr_fn);
arm_index = arm_index + 1;
}
}
inner_body = inner_body + mlc::String("std::abort();\n");
return mlc::String("[&]() -> ") + return_cpp + mlc::String(" {\n") + inner_body + mlc::String("}()");
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatIdent& patident) -> mlc::String { auto [name, _w0] = patident; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, context::context_add_value(context, name), gen_stmts)); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn); },
  [&](const PatTuple& pattuple) -> mlc::String { auto [_w0, _w1] = pattuple; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatArray& patarray) -> mlc::String { auto [_w0, _w1, _w2] = patarray; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatOr& pator) -> mlc::String { auto [_w0, _w1] = pator; return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); }
}, (*arm->pat));}

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> expanded = expand_or_arms(arms);
return expanded_any_guard(expanded) ? gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < expanded.size()){
{
parts.push_back(gen_arm(expanded[index], context, gen_stmts, eval_expr_fn));
index = index + 1;
}
}
  mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
  bool needs_deref = expanded.size() > 0 ? match_analysis::first_arm_needs_deref(expanded[0], subject, context) : true;
  return expr::std_visit_match_expression(parts.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_deref));
 }();
}

} // namespace match_gen

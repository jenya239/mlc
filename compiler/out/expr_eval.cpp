#include "expr_eval.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "match_analysis.hpp"
#include "expression_support.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_eval {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace literals;
using namespace identifiers;
using namespace match_analysis;
using namespace expression_support;
using namespace statement_context;
using namespace expr;
using namespace ast_tokens;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < expressions.size()){
{
parts.push_back(eval_expr(expressions[index], context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
std::shared_ptr<semantic_ir::SExpr> value = expression_support::find_field_value(field_values, field_order[index]);
parts.push_back(eval_expr(value, context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_values.size()){
{
parts.push_back(eval_expr(field_values[index]->val, context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
mlc::String field_name = field_order[index];
mlc::String value_string = mlc::String("");
bool override_found = false;
int override_index = 0;
while (override_index < overrides.size()){
{
if (overrides[override_index]->name == field_name){
{
value_string = eval_expr(overrides[override_index]->val, context, gen_stmts);
override_found = true;
}
}
override_index = override_index + 1;
}
}
if (!override_found){
{
value_string = eval_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(base_expr, field_name, std::make_shared<registry::Type>((registry::TUnknown{})), ast::span_unknown())), context, gen_stmts);
}
}
parts.push_back(value_string);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = mlc::Array<mlc::String>{expr::record_update_base_prefix(eval_expr(base_expr, context, gen_stmts))};
int index = 0;
while (index < overrides.size()){
{
parts.push_back(expr::mutable_member_assignment(mlc::String("__upd"), cpp_naming::cpp_safe(overrides[index]->name), eval_expr(overrides[index]->val, context, gen_stmts)));
index = index + 1;
}
}
return expr::record_update_lazy_closure(parts.join(mlc::String("")));
}

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
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
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, eval_expr(arm_body, arm_context, gen_stmts));
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
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
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), field_bindings, eval_expr(arm_body, context, gen_stmts));
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return expr::match_arm_wild_or_unit_return(eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return expr::match_arm_wild_or_unit_return(eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return expr::match_arm_wild_or_unit_return(eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return expr::match_arm_wild_or_unit_return(eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return expr::match_arm_wild_or_unit_return(eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatIdent& patident) -> mlc::String { auto [name, _w0] = patident; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr(arm->body, context, gen_stmts)); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts); }
}, (*arm->pat));}

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < arms.size()){
{
parts.push_back(gen_arm(arms[index], context, gen_stmts));
index = index + 1;
}
}
mlc::String subject_code = eval_expr(subject, context, gen_stmts);
bool needs_deref = arms.size() > 0 ? match_analysis::first_arm_needs_deref(arms[0], subject, context) : true;
return expr::std_visit_match_expression(parts.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_deref));
}

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::parenthesized_binary(eval_expr(left_expr, context, gen_stmts), operation, eval_expr(right_expr, context, gen_stmts));}

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::parenthesized_unary(operation, eval_expr(inner_expr, context, gen_stmts));}

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expr)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expr)._); auto [name, _w0, _w1] = _v_sexprident; return context::context_resolve(context, cpp_naming::map_builtin(name)); } return eval_expr(function_expr, context, gen_stmts); }();}

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String function_code = gen_call_function_code(function_expr, context, gen_stmts);
mlc::String argument_list = eval_argument_list(arguments, context, gen_stmts);
return expression_support::is_constructor_call(function_expr) ? expr::constructor_call_braces(function_code, argument_list) : expr::function_call_parentheses(function_code, argument_list);
}

mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*object)._); auto [name, _w0, _w1] = _v_sexprident; return context.self_type.length() > 0 ? expression_support::resolve_object_code_in_self_context(name, context) : context::context_resolve(context, cpp_naming::map_builtin(name)); } return eval_expr(object, context, gen_stmts); }();}

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), eval_argument_list(arguments, context, gen_stmts));}

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String argument_code = eval_expr(argument, context, gen_stmts);
mlc::String type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String mangled_name = context.method_owners.get(method_name);
mlc::String function_name = context::context_resolve(context, mangled_name);
return arguments.size() == 0 ? expr::function_call_parentheses(function_name, object_code) : expr::function_call_parentheses(function_name, expr::comma_separated_pair(object_code, eval_argument_list(arguments, context, gen_stmts)));
}

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String call_base = expr::dot_member_access(object_code, cpp_naming::map_method(method_name));
return arguments.size() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, eval_argument_list(arguments, context, gen_stmts));
}

mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String call_base = static_prefix + cpp_naming::map_method(method_name);
return arguments.size() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, eval_argument_list(arguments, context, gen_stmts));
}

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TI32>((*type_value))) {  return mlc::String("i32"); } if (std::holds_alternative<registry::TString>((*type_value))) {  return mlc::String("string"); } if (std::holds_alternative<registry::TBool>((*type_value))) {  return mlc::String("bool"); } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner))) { auto _v_tnamed = std::get<registry::TNamed>((*inner)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }(); } return mlc::String(""); }();}

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{return context.method_owners.has(method_name) && type_name != mlc::String("") && context.method_owners.get(method_name) == type_name + mlc::String("_") + method_name;}

mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return object_code == mlc::String("File") ? gen_method_file(method_name, arguments, context, gen_stmts) : object_code == mlc::String("Map") && method_name == mlc::String("new") ? expr::empty_map_initializer() : object_code == mlc::String("Shared") && method_name == mlc::String("new") && arguments.size() == 1 ? gen_method_shared_new(arguments[0], context, gen_stmts) : is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call(object_code, method_name, arguments, context, gen_stmts) : method_name == mlc::String("to_string") && arguments.size() == 0 ? expr::runtime_to_string_call(object_code) : gen_method_builtin(object_code, method_name, arguments, context, gen_stmts);}

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String object_type_name = object_type_name_for_dispatch(semantic_ir::sexpr_type(object));
return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*object)._); auto [object_name, _w0, _w1] = _v_sexprident; return context.namespace_alias_prefixes.has(object_name) ? gen_method_namespace_alias(context.namespace_alias_prefixes.get(object_name), method_name, arguments, context, gen_stmts) : gen_method_expr_after_object(gen_object_code(object, context, gen_stmts), object_type_name, method_name, arguments, context, gen_stmts); } return gen_method_expr_after_object(gen_object_code(object, context, gen_stmts), object_type_name, method_name, arguments, context, gen_stmts); }();
}

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String object_code = eval_expr(object, context, gen_stmts);
mlc::String operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::index_subscript(eval_expr(object, context, gen_stmts), eval_expr(index_expr, context, gen_stmts));}

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::ternary_conditional(eval_expr(condition, context, gen_stmts), eval_expr(then_expr, context, gen_stmts), eval_expr(else_expr, context, gen_stmts));}

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return statements.size() == 0 ? eval_expr(result_expr, context, gen_stmts) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(eval_expr(result_expr, statement_context::stmts_final_ctx(statements, context), gen_stmts));
  return expr::block_as_immediate_invoked_function_expression(body);
 }();}

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::while_loop_immediate_invoked_function_expression(eval_expr(condition, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), eval_expr(iterator, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
mlc::String values_code = field_order.size() > 0 ? gen_record_ordered(field_values, field_order, context, gen_stmts) : gen_record_unordered(field_values, context, gen_stmts);
return expr::record_initializer(context::context_resolve(context, type_name), values_code);
}

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
return field_order.size() > 0 ? expr::record_initializer(context::context_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts)) : gen_record_update_lazy(base_expr, overrides, context, gen_stmts);
}

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::array_literal(eval_argument_list(elements, context, gen_stmts));}

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String inner_code = eval_expr(inner_expr, context, gen_stmts);
return expr::question_try_result(inner_code);
}

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::lambda_with_return(expression_support::cpp_lambda_header_prefix(parameters), eval_expr(body_expr, context, gen_stmts));}

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> mlc::String { auto [integer_value, _w0, _w1] = sexprint; return literals::gen_integer_literal(integer_value); },
  [&](const SExprStr& sexprstr) -> mlc::String { auto [string_value, _w0, _w1] = sexprstr; return literals::gen_string_literal(string_value); },
  [&](const SExprBool& sexprbool) -> mlc::String { auto [boolean_value, _w0, _w1] = sexprbool; return literals::gen_boolean_literal(boolean_value); },
  [&](const SExprUnit& sexprunit) -> mlc::String { auto [_w0, _w1] = sexprunit; return literals::gen_unit_literal(); },
  [&](const SExprExtern& sexprextern) -> mlc::String { auto [_w0, _w1] = sexprextern; return literals::gen_extern_placeholder(); },
  [&](const SExprIdent& sexprident) -> mlc::String { auto [name, _w0, _w1] = sexprident; return identifiers::gen_identifier(name, context); },
  [&](const SExprBin& sexprbin) -> mlc::String { auto [operation, left_expr, right_expr, _w0, _w1] = sexprbin; return gen_binary_expr(operation, left_expr, right_expr, context, gen_stmts); },
  [&](const SExprUn& sexprun) -> mlc::String { auto [operation, inner_expr, _w0, _w1] = sexprun; return gen_unary_expr(operation, inner_expr, context, gen_stmts); },
  [&](const SExprCall& sexprcall) -> mlc::String { auto [function_expr, arguments, _w0, _w1] = sexprcall; return gen_call_expr(function_expr, arguments, context, gen_stmts); },
  [&](const SExprMethod& sexprmethod) -> mlc::String { auto [object, method_name, arguments, _w0, _w1] = sexprmethod; return gen_method_expr(object, method_name, arguments, context, gen_stmts); },
  [&](const SExprField& sexprfield) -> mlc::String { auto [object, field_name, _w0, _w1] = sexprfield; return gen_field_expr(object, field_name, context, gen_stmts); },
  [&](const SExprIndex& sexprindex) -> mlc::String { auto [object, index_expr, _w0, _w1] = sexprindex; return gen_index_expr(object, index_expr, context, gen_stmts); },
  [&](const SExprIf& sexprif) -> mlc::String { auto [condition, then_expr, else_expr, _w0, _w1] = sexprif; return gen_if_expr(condition, then_expr, else_expr, context, gen_stmts); },
  [&](const SExprBlock& sexprblock) -> mlc::String { auto [statements, result_expr, _w0, _w1] = sexprblock; return gen_block_expr(statements, result_expr, context, gen_stmts); },
  [&](const SExprWhile& sexprwhile) -> mlc::String { auto [condition, statements, _w0, _w1] = sexprwhile; return gen_while_expr(condition, statements, context, gen_stmts); },
  [&](const SExprFor& sexprfor) -> mlc::String { auto [variable, iterator, statements, _w0, _w1] = sexprfor; return gen_for_expr(variable, iterator, statements, context, gen_stmts); },
  [&](const SExprMatch& sexprmatch) -> mlc::String { auto [subject, arms, _w0, _w1] = sexprmatch; return gen_match(subject, arms, context, gen_stmts); },
  [&](const SExprRecord& sexprrecord) -> mlc::String { auto [type_name, field_values, _w0, _w1] = sexprrecord; return gen_record_expr(type_name, field_values, context, gen_stmts); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> mlc::String { auto [type_name, base_expr, overrides, _w0, _w1] = sexprrecordupdate; return gen_record_update_expr(type_name, base_expr, overrides, context, gen_stmts); },
  [&](const SExprArray& sexprarray) -> mlc::String { auto [elements, _w0, _w1] = sexprarray; return gen_array_expr(elements, context, gen_stmts); },
  [&](const SExprQuestion& sexprquestion) -> mlc::String { auto [inner_expr, _w0, _w1] = sexprquestion; return gen_question_expr(inner_expr, context, gen_stmts); },
  [&](const SExprLambda& sexprlambda) -> mlc::String { auto [parameters, body_expr, _w0, _w1] = sexprlambda; return gen_lambda_expr(parameters, body_expr, context, gen_stmts); }
}, (*expr)._);}

} // namespace expr_eval

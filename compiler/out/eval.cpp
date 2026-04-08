#include "eval.hpp"

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

namespace eval {

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

context::CodegenContext codegen_context_with_shared_binding(context::CodegenContext arm_context, mlc::String binding_name) noexcept;

context::CodegenContext codegen_context_with_shared_array_binding(context::CodegenContext arm_context, mlc::String binding_name) noexcept;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept;

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context) noexcept;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context) noexcept;

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context) noexcept;

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context) noexcept;

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context) noexcept;

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context) noexcept;

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context) noexcept;

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context) noexcept;

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context) noexcept;

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context) noexcept;

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context) noexcept;

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context) noexcept;

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context) noexcept;

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context) noexcept;

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context) noexcept;

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

mlc::String gen_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

context::CodegenContext codegen_context_with_shared_binding(context::CodegenContext arm_context, mlc::String binding_name) noexcept{return context::context_add_shared(arm_context, binding_name);}

context::CodegenContext codegen_context_with_shared_array_binding(context::CodegenContext arm_context, mlc::String binding_name) noexcept{return context::context_add_shared_array(arm_context, binding_name);}

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < expressions.size()){
{
parts.push_back(gen_expr(expressions[index], context));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
std::shared_ptr<semantic_ir::SExpr> value = expression_support::find_field_value(field_values, field_order[index]);
parts.push_back(gen_expr(value, context));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_values.size()){
{
parts.push_back(gen_expr(field_values[index]->val, context));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept{
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
value_string = gen_expr(overrides[override_index]->val, context);
override_found = true;
}
}
override_index = override_index + 1;
}
}
if (!override_found){
{
value_string = gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(base_expr, field_name, std::make_shared<registry::Type>((registry::TUnknown{})), ast::span_unknown())), context);
}
}
parts.push_back(value_string);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = mlc::Array<mlc::String>{expr::record_update_base_prefix(gen_expr(base_expr, context))};
int index = 0;
while (index < overrides.size()){
{
parts.push_back(expr::mutable_member_assignment(mlc::String("__upd"), cpp_naming::cpp_safe(overrides[index]->name), gen_expr(overrides[index]->val, context)));
index = index + 1;
}
}
return expr::record_update_lazy_closure(parts.join(mlc::String("")));
}

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context) noexcept{
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
arm_context = codegen_context_with_shared_binding(arm_context, pattern_name);
}
}
  if (ctor_type_info->shared_arr_pos.contains(index)){
arm_context = codegen_context_with_shared_array_binding(arm_context, pattern_name);
}
 }(); } return; }();
index = index + 1;
}
}
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), binding, gen_expr(arm_body, arm_context));
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<semantic_ir::SExpr> arm_body, context::CodegenContext context) noexcept{
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
return expr::match_arm_constructed_value(expr::match_lambda_const_reference_parameter(qualified_name, type_argument, lower_name), field_bindings, gen_expr(arm_body, context));
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SMatchArm> arm, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return expr::match_arm_wild_or_unit_return(gen_expr(arm->body, context)); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return expr::match_arm_wild_or_unit_return(gen_expr(arm->body, context)); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return expr::match_arm_wild_or_unit_return(gen_expr(arm->body, context)); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return expr::match_arm_wild_or_unit_return(gen_expr(arm->body, context)); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return expr::match_arm_wild_or_unit_return(gen_expr(arm->body, context)); },
  [&](const PatIdent& patident) -> mlc::String { auto [name, _w0] = patident; return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), gen_expr(arm->body, context)); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_arm_ctor(name, sub_patterns, arm->body, context); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context); }
}, (*arm->pat));}

mlc::String gen_match(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < arms.size()){
{
parts.push_back(gen_arm(arms[index], context));
index = index + 1;
}
}
mlc::String subject_code = gen_expr(subject, context);
bool needs_deref = arms.size() > 0 ? match_analysis::first_arm_needs_deref(arms[0], subject, context) : true;
return expr::std_visit_match_expression(parts.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_deref));
}

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context) noexcept{return expr::parenthesized_binary(gen_expr(left_expr, context), operation, gen_expr(right_expr, context));}

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context) noexcept{return expr::parenthesized_unary(operation, gen_expr(inner_expr, context));}

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expr)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expr)._); auto [name, _w0, _w1] = _v_sexprident; return context::context_resolve(context, cpp_naming::map_builtin(name)); } return gen_expr(function_expr, context); }();}

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{
mlc::String function_code = gen_call_function_code(function_expr, context);
mlc::String argument_list = gen_argument_list(arguments, context);
return expression_support::is_constructor_call(function_expr) ? expr::constructor_call_braces(function_code, argument_list) : expr::function_call_parentheses(function_code, argument_list);
}

mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*object)._); auto [name, _w0, _w1] = _v_sexprident; return context.self_type.length() > 0 ? expression_support::resolve_object_code_in_self_context(name, context) : context::context_resolve(context, cpp_naming::map_builtin(name)); } return gen_expr(object, context); }();}

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), gen_argument_list(arguments, context));}

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context) noexcept{
mlc::String argument_code = gen_expr(argument, context);
mlc::String type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{
mlc::String mangled_name = context.method_owners.get(method_name);
mlc::String function_name = context::context_resolve(context, mangled_name);
return arguments.size() == 0 ? expr::function_call_parentheses(function_name, object_code) : expr::function_call_parentheses(function_name, expr::comma_separated_pair(object_code, gen_argument_list(arguments, context)));
}

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{
mlc::String call_base = expr::dot_member_access(object_code, cpp_naming::map_method(method_name));
return arguments.size() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, gen_argument_list(arguments, context));
}

mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{
mlc::String call_base = static_prefix + cpp_naming::map_method(method_name);
return arguments.size() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, gen_argument_list(arguments, context));
}

mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{return object_code == mlc::String("File") ? gen_method_file(method_name, arguments, context) : object_code == mlc::String("Map") && method_name == mlc::String("new") ? expr::empty_map_initializer() : object_code == mlc::String("Shared") && method_name == mlc::String("new") && arguments.size() == 1 ? gen_method_shared_new(arguments[0], context) : method_name == mlc::String("to_string") && arguments.size() == 0 ? expr::runtime_to_string_call(object_code) : context.method_owners.has(method_name) ? gen_method_owner_call(object_code, method_name, arguments, context) : gen_method_builtin(object_code, method_name, arguments, context);}

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*object)._); auto [object_name, _w0, _w1] = _v_sexprident; return context.namespace_alias_prefixes.has(object_name) ? gen_method_namespace_alias(context.namespace_alias_prefixes.get(object_name), method_name, arguments, context) : gen_method_expr_after_object(gen_object_code(object, context), method_name, arguments, context); } return gen_method_expr_after_object(gen_object_code(object, context), method_name, arguments, context); }();}

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context) noexcept{
mlc::String object_code = gen_expr(object, context);
mlc::String operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context) noexcept{return expr::index_subscript(gen_expr(object, context), gen_expr(index_expr, context));}

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context) noexcept{return expr::ternary_conditional(gen_expr(condition, context), gen_expr(then_expr, context), gen_expr(else_expr, context));}

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context) noexcept{return statements.size() == 0 ? gen_expr(result_expr, context) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(statements, context) + expr::block_immediate_function_return_fragment(gen_expr(result_expr, statement_context::stmts_final_ctx(statements, context)));
  return expr::block_as_immediate_invoked_function_expression(body);
 }();}

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return expr::while_loop_immediate_invoked_function_expression(gen_expr(condition, context), gen_stmts_str(statements, context));}

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), gen_expr(iterator, context), gen_stmts_str(statements, context));}

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
mlc::String values_code = field_order.size() > 0 ? gen_record_ordered(field_values, field_order, context) : gen_record_unordered(field_values, context);
return expr::record_initializer(context::context_resolve(context, type_name), values_code);
}

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
return field_order.size() > 0 ? expr::record_initializer(context::context_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context)) : gen_record_update_lazy(base_expr, overrides, context);
}

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context) noexcept{return expr::array_literal(gen_argument_list(elements, context));}

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context) noexcept{
mlc::String inner_code = gen_expr(inner_expr, context);
return expr::question_try_result(inner_code);
}

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept{return expr::lambda_with_return(expression_support::cpp_lambda_header_prefix(parameters), gen_expr(body_expr, context));}

mlc::String gen_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> mlc::String { auto [integer_value, _w0, _w1] = sexprint; return literals::gen_integer_literal(integer_value); },
  [&](const SExprStr& sexprstr) -> mlc::String { auto [string_value, _w0, _w1] = sexprstr; return literals::gen_string_literal(string_value); },
  [&](const SExprBool& sexprbool) -> mlc::String { auto [boolean_value, _w0, _w1] = sexprbool; return literals::gen_boolean_literal(boolean_value); },
  [&](const SExprUnit& sexprunit) -> mlc::String { auto [_w0, _w1] = sexprunit; return literals::gen_unit_literal(); },
  [&](const SExprExtern& sexprextern) -> mlc::String { auto [_w0, _w1] = sexprextern; return literals::gen_extern_placeholder(); },
  [&](const SExprIdent& sexprident) -> mlc::String { auto [name, _w0, _w1] = sexprident; return identifiers::gen_identifier(name, context); },
  [&](const SExprBin& sexprbin) -> mlc::String { auto [operation, left_expr, right_expr, _w0, _w1] = sexprbin; return gen_binary_expr(operation, left_expr, right_expr, context); },
  [&](const SExprUn& sexprun) -> mlc::String { auto [operation, inner_expr, _w0, _w1] = sexprun; return gen_unary_expr(operation, inner_expr, context); },
  [&](const SExprCall& sexprcall) -> mlc::String { auto [function_expr, arguments, _w0, _w1] = sexprcall; return gen_call_expr(function_expr, arguments, context); },
  [&](const SExprMethod& sexprmethod) -> mlc::String { auto [object, method_name, arguments, _w0, _w1] = sexprmethod; return gen_method_expr(object, method_name, arguments, context); },
  [&](const SExprField& sexprfield) -> mlc::String { auto [object, field_name, _w0, _w1] = sexprfield; return gen_field_expr(object, field_name, context); },
  [&](const SExprIndex& sexprindex) -> mlc::String { auto [object, index_expr, _w0, _w1] = sexprindex; return gen_index_expr(object, index_expr, context); },
  [&](const SExprIf& sexprif) -> mlc::String { auto [condition, then_expr, else_expr, _w0, _w1] = sexprif; return gen_if_expr(condition, then_expr, else_expr, context); },
  [&](const SExprBlock& sexprblock) -> mlc::String { auto [statements, result_expr, _w0, _w1] = sexprblock; return gen_block_expr(statements, result_expr, context); },
  [&](const SExprWhile& sexprwhile) -> mlc::String { auto [condition, statements, _w0, _w1] = sexprwhile; return gen_while_expr(condition, statements, context); },
  [&](const SExprFor& sexprfor) -> mlc::String { auto [variable, iterator, statements, _w0, _w1] = sexprfor; return gen_for_expr(variable, iterator, statements, context); },
  [&](const SExprMatch& sexprmatch) -> mlc::String { auto [subject, arms, _w0, _w1] = sexprmatch; return gen_match(subject, arms, context); },
  [&](const SExprRecord& sexprrecord) -> mlc::String { auto [type_name, field_values, _w0, _w1] = sexprrecord; return gen_record_expr(type_name, field_values, context); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> mlc::String { auto [type_name, base_expr, overrides, _w0, _w1] = sexprrecordupdate; return gen_record_update_expr(type_name, base_expr, overrides, context); },
  [&](const SExprArray& sexprarray) -> mlc::String { auto [elements, _w0, _w1] = sexprarray; return gen_array_expr(elements, context); },
  [&](const SExprQuestion& sexprquestion) -> mlc::String { auto [inner_expr, _w0, _w1] = sexprquestion; return gen_question_expr(inner_expr, context); },
  [&](const SExprLambda& sexprlambda) -> mlc::String { auto [parameters, body_expr, _w0, _w1] = sexprlambda; return gen_lambda_expr(parameters, body_expr, context); }
}, (*expr)._);}

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept{
mlc::String inner_code = gen_expr(inner_expr, context);
return expr::try_unwrap_result_block(try_identifier, inner_code, success_line);
}

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SStmtLet& sstmtlet) -> context::GenStmtResult { auto [name, _w0, value, value_type, _w1] = sstmtlet; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  int next_counter = try_counter + 1;
  context::GenStmtResult result = [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*value)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*value)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), try_identifier)), next_counter}; } if (std::holds_alternative<semantic_ir::SExprArray>((*value)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*value)._); auto [elements, array_type, _w0] = _v_sexprarray; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = elements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*array_type))) { auto _v_tarray = std::get<registry::TArray>((*array_type)); auto [inner_type] = _v_tarray; return expr::typed_array_empty(type_gen::sem_type_to_cpp(context, inner_type)); } return expr::array_string_fallback_empty(); }() : expr::constructor_call_braces(mlc::String("mlc::Array"), gen_argument_list(elements, context));
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
 }(); } if (std::holds_alternative<semantic_ir::SExprMethod>((*value)._)) { auto _v_sexprmethod = std::get<semantic_ir::SExprMethod>((*value)._); auto [map_object, method_name, _w0, _w1, _w2] = _v_sexprmethod; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = method_name == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*map_object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*map_object)._); auto [object_name, _w0, _w1] = _v_sexprident; return object_name == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [type_name, type_args] = _v_tgeneric; return type_name == mlc::String("Map") && type_args.size() == 2 ? expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_args[0]), type_gen::sem_type_to_cpp(context, type_args[1])) : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context);
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
 }(); } return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr(value, context)), try_counter}; }();
  return result;
 }(); },
  [&](const SStmtLetConst& sstmtletconst) -> context::GenStmtResult { auto [name, value, _w0, _w1] = sstmtletconst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr(value, context)), try_counter}; },
  [&](const SStmtExpr& sstmtexpr) -> context::GenStmtResult { auto [expression, _w0] = sstmtexpr; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, expr::discard_try_ok_field0_statement(try_identifier)), try_counter + 1};
 }(); } return context::GenStmtResult{gen_stmt_expr(expression, context), try_counter}; }(); },
  [&](const SStmtBreak& sstmtbreak) -> context::GenStmtResult { auto [_w0] = sstmtbreak; return context::GenStmtResult{expr::break_line(), try_counter}; },
  [&](const SStmtContinue& sstmtcontinue) -> context::GenStmtResult { auto [_w0] = sstmtcontinue; return context::GenStmtResult{expr::continue_line(), try_counter}; },
  [&](const SStmtReturn& sstmtreturn) -> context::GenStmtResult { auto [expression, _w0] = sstmtreturn; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_ret_") + mlc::to_string(try_counter);
  return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier)), try_counter + 1};
 }(); } return context::GenStmtResult{expr::return_line(gen_expr(expression, context)), try_counter}; }(); }
}, (*stmt)._);}

mlc::String gen_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBin>((*expression)._)) { auto _v_sexprbin = std::get<semantic_ir::SExprBin>((*expression)._); auto [operation, left_expr, right_expr, _w0, _w1] = _v_sexprbin; return operation == mlc::String("=") ? expr::assignment_statement(gen_expr(left_expr, context), gen_expr(right_expr, context)) : expr::expression_operation_statement(operation, gen_expr(left_expr, context), gen_expr(right_expr, context)); } if (std::holds_alternative<semantic_ir::SExprIf>((*expression)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expression)._); auto [condition, then_expr, else_expr, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(gen_expr(condition, context), gen_block_body(then_expr, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprBlock>((*else_expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*else_expr)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return statements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } return expr::else_brace_block(gen_block_body(else_expr, context)); }() : expr::else_brace_block(gen_block_body(else_expr, context)); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(gen_stmt_expr(else_expr, context)); } return expr::else_brace_block(gen_block_body(else_expr, context)); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return expr::fragment_with_newline(output);
 }(); } if (std::holds_alternative<semantic_ir::SExprWhile>((*expression)._)) { auto _v_sexprwhile = std::get<semantic_ir::SExprWhile>((*expression)._); auto [condition, statements, _w0, _w1] = _v_sexprwhile; return expr::while_loop_statement(gen_expr(condition, context), gen_stmts_str(statements, context)); } if (std::holds_alternative<semantic_ir::SExprFor>((*expression)._)) { auto _v_sexprfor = std::get<semantic_ir::SExprFor>((*expression)._); auto [variable, iterator, statements, _w0, _w1] = _v_sexprfor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr(iterator, context), gen_stmts_str(statements, context)); } if (std::holds_alternative<semantic_ir::SExprBlock>((*expression)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expression)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = gen_stmts_str(statements, context);
  mlc::String result_code = gen_expr(result_expr, statement_context::stmts_final_ctx(statements, context));
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); } return expr::suffix_semicolon_newline(gen_expr(expression, context)); }();}

mlc::String gen_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBlock>((*expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expr)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = gen_stmts_str(statements, context);
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIf>((*result_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return statements_code + gen_stmt_expr(result_expr, final_context); } if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expr)._); auto [_w0, _w1] = _v_sexprunit; return statements_code; } return [&]() -> mlc::String { 
  mlc::String result_code = gen_expr(result_expr, final_context);
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); }();
 }(); } if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return gen_stmt_expr(expr, context); } return expr::suffix_semicolon_newline(gen_expr(expr, context)); }();}

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
context::CodegenContext current_context = std::move(context);
int counter = try_counter;
while (index < statements.size()){
{
context::GenStmtResult result = gen_stmt_with_try(statements[index], current_context, counter);
parts.push_back(result.output);
counter = result.next_try;
current_context = context::update_context_from_statement(statements[index], current_context);
index = index + 1;
}
}
return context::GenStmtsResult{parts.join(mlc::String("")), counter};
}

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{
context::GenStmtsResult result = gen_stmts_str_with_try(statements, context, 0);
return result.code;
}

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(gen_expr(condition, context), gen_return_body(then_expr, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); } return expr::else_brace_block(gen_return_body(else_expr, context)); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return expr::fragment_with_newline(output);
 }(); } return expr::if_always_true_block(gen_return_body(expr, context)); }();}

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBlock>((*expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expr)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  mlc::String statements_code = gen_stmts_str(statements, context);
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprQuestion>((*result_expr)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*result_expr)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return statements_code + gen_try_unwrap(inner_expr, final_context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expr)._); auto [_w0, _w1] = _v_sexprunit; return statements_code; } if (std::holds_alternative<semantic_ir::SExprIf>((*result_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<semantic_ir::SExprMatch>((*result_expr)._)) { auto _v_sexprmatch = std::get<semantic_ir::SExprMatch>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_sexprmatch; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<semantic_ir::SExprBlock>((*result_expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_sexprblock; return statements_code + gen_return_body(result_expr, final_context); } return statements_code + expr::return_line(gen_expr(result_expr, final_context)); }();
 }(); } if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(gen_expr(condition, context), gen_return_body(then_expr, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); } return expr::else_brace_block(gen_return_body(else_expr, context)); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return expr::fragment_with_newline(output);
 }(); } if (std::holds_alternative<semantic_ir::SExprQuestion>((*expr)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expr)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return gen_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } return expr::return_line(gen_expr(expr, context)); }();}

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept{return gen_return_body(body_expr, context);}

} // namespace eval

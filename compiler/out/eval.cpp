#include "eval.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace eval {

using namespace ast;
using namespace context;
using namespace type_gen;
using namespace ast_tokens;

context::CodegenContext CodegenContext_with_shared_param(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_with_shared_array_param(context::CodegenContext self, mlc::String name) noexcept;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, context::CodegenContext context) noexcept;

std::shared_ptr<ast::Expr> find_field_value(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String field_name) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, context::CodegenContext context) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept;

mlc::String gen_record_update_lazy(std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, context::CodegenContext context) noexcept;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<ast::Expr> arm_body, context::CodegenContext context) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<ast::Expr> arm_body, context::CodegenContext context) noexcept;

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, context::CodegenContext context) noexcept;

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, context::CodegenContext context) noexcept;

mlc::String gen_integer_literal(int integer_value) noexcept;

mlc::String gen_string_literal(mlc::String string_value) noexcept;

mlc::String gen_boolean_literal(bool boolean_value) noexcept;

mlc::String gen_unit_literal() noexcept;

mlc::String gen_extern_placeholder() noexcept;

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept;

mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept;

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr, context::CodegenContext context) noexcept;

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<ast::Expr> function_expr, context::CodegenContext context) noexcept;

bool is_constructor_call(std::shared_ptr<ast::Expr> function_expr) noexcept;

mlc::String gen_call_expr(std::shared_ptr<ast::Expr> function_expr, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept;

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;

mlc::String gen_object_code(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept;

mlc::String infer_shared_new_type_name(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept;

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept;

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept;

mlc::String gen_method_expr(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept;

mlc::String field_access_operator(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept;

mlc::String gen_field_expr(std::shared_ptr<ast::Expr> object, mlc::String field_name, context::CodegenContext context) noexcept;

mlc::String gen_index_expr(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expr, context::CodegenContext context) noexcept;

mlc::String gen_if_expr(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expr, std::shared_ptr<ast::Expr> else_expr, context::CodegenContext context) noexcept;

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expr, context::CodegenContext context) noexcept;

mlc::String gen_while_expr(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, context::CodegenContext context) noexcept;

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, context::CodegenContext context) noexcept;

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<ast::Expr>> elements, context::CodegenContext context) noexcept;

mlc::String gen_question_expr(std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context) noexcept;

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<ast::Expr> body_expr, context::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body_expr, context::CodegenContext context) noexcept;

context::CodegenContext CodegenContext_with_shared_param(context::CodegenContext self, mlc::String name) noexcept{return context::context_add_shared(self, name);}

context::CodegenContext CodegenContext_with_shared_array_param(context::CodegenContext self, mlc::String name) noexcept{return context::context_add_shared_array(self, name);}

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, context::CodegenContext context) noexcept{
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

std::shared_ptr<ast::Expr> find_field_value(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String field_name) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));
int index = 0;
while (index < field_values.size()){
{
if (field_values[index]->name == field_name){
{
result = field_values[index]->val;
}
}
index = index + 1;
}
}
return result;
}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
std::shared_ptr<ast::Expr> value = find_field_value(field_values, field_order[index]);
parts.push_back(gen_expr(value, context));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, context::CodegenContext context) noexcept{
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

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context) noexcept{
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
value_string = gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base_expr, field_name, ast::span_unknown())), context);
}
}
parts.push_back(value_string);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_lazy(std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = mlc::Array<mlc::String>{mlc::String("auto __upd = ") + gen_expr(base_expr, context) + mlc::String("; ")};
int index = 0;
while (index < overrides.size()){
{
parts.push_back(mlc::String("__upd.") + context::cpp_safe(overrides[index]->name) + mlc::String(" = ") + gen_expr(overrides[index]->val, context) + mlc::String("; "));
index = index + 1;
}
}
return mlc::String("[&]() { ") + parts.join(mlc::String("")) + mlc::String("return __upd; }()");
}

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept{
return context::expr_returns_shared_sum_visit_ptr(subject) ? true : [&]() { if (std::holds_alternative<ast::ExprIdent>((*subject)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*subject)._); auto [name, _w0] = _v_exprident; return context.match_deref_params.contains(name) ? true : context.shared_params.contains(name) ? true : context.value_params.contains(name) ? false : true; } if (std::holds_alternative<ast::ExprIndex>((*subject)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*subject)._); auto [array_expr, _w0, _w1] = _v_exprindex; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*array_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*array_expr)._); auto [array_name, _w0] = _v_exprident; return context.shared_array_params.contains(array_name); } return true; }(); } if (std::holds_alternative<ast::ExprMethod>((*subject)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*subject)._); auto [_w0, _w1, _w2, _w3] = _v_exprmethod; return false; } return true; }();
}

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::PatCtor>((*first_arm->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*first_arm->pat)); auto [name, _w0, _w1] = _v_patctor; return [&]() -> bool { 
  mlc::String resolved = context::context_resolve(context, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : context::list_contains(context.generic_variants, name) ? false : subject_needs_deref(subject, context);
 }(); } return subject_needs_deref(subject, context); }();}

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < sub_patterns.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[index])); auto [name, _w0] = _v_patident; return context::cpp_safe(name); } return mlc::String("__") + mlc::to_string(index); }();
parts.push_back(bind_name);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_arm_ctor(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, std::shared_ptr<ast::Expr> arm_body, context::CodegenContext context) noexcept{
mlc::String qualified_name = context::context_resolve(context, ctor_name);
mlc::String lower_name = context::cpp_safe(context::lower_first(ctor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : mlc::String("auto [") + pat_bind_names(sub_patterns) + mlc::String("] = ") + lower_name + mlc::String("; ");
std::shared_ptr<context::CtorTypeInfo> ctor_type_info = context::lookup_ctor_type_info(context.ctor_type_infos, ctor_name);
bool is_generic = context::list_contains(context.generic_variants, ctor_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
context::CodegenContext arm_context = std::move(context);
int index = 0;
while (index < sub_patterns.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[index])); auto [pattern_name, _w0] = _v_patident; return [&]() { 
  if (ctor_type_info->shared_pos.contains(index)){
{
arm_context = CodegenContext_with_shared_param(arm_context, pattern_name);
}
}
  if (ctor_type_info->shared_arr_pos.contains(index)){
arm_context = CodegenContext_with_shared_array_param(arm_context, pattern_name);
}
 }(); } return; }();
index = index + 1;
}
}
return mlc::String("[&](const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(") { ") + binding + mlc::String("return ") + gen_expr(arm_body, arm_context) + mlc::String("; }");
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pat>> field_patterns, std::shared_ptr<ast::Expr> arm_body, context::CodegenContext context) noexcept{
mlc::String qualified_name = context::context_resolve(context, record_name);
mlc::String lower_name = context::cpp_safe(context::lower_first(record_name));
bool is_generic = context::list_contains(context.generic_variants, record_name);
mlc::String type_argument = is_generic ? mlc::String("<auto>") : mlc::String("");
mlc::String field_bindings = mlc::String("");
int field_index = 0;
while (field_index < field_patterns.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [field_name, _w0] = patident;
{
field_bindings = field_bindings + mlc::String("const auto& ") + context::cpp_safe(field_name) + mlc::String(" = ") + lower_name + mlc::String(".") + context::cpp_safe(field_name) + mlc::String("; ");
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
return mlc::String("[&](const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(") { ") + field_bindings + mlc::String("return ") + gen_expr(arm_body, context) + mlc::String("; }");
}

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { auto [_w0] = patwild; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatUnit& patunit) -> mlc::String { auto [_w0] = patunit; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0, _w1] = patbool; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0, _w1] = patint; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0, _w1] = patstr; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatIdent& patident) -> mlc::String { auto [name, _w0] = patident; return mlc::String("[&](const auto& ") + context::cpp_safe(name) + mlc::String(") { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_patterns, _w0] = patctor; return gen_arm_ctor(name, sub_patterns, arm->body, context); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_patterns, _w0] = patrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context); }
}, (*arm->pat));}

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < arms.size()){
{
parts.push_back(gen_arm(arms[index], context));
index = index + 1;
}
}
mlc::String subject_code = gen_expr(subject, context);
bool needs_deref = arms.size() > 0 ? first_arm_needs_deref(arms[0], subject, context) : true;
mlc::String visit_subject = needs_deref ? mlc::String("(*") + subject_code + mlc::String(")") : subject_code;
return mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n")) + mlc::String("\n}, ") + visit_subject + mlc::String(")");
}

mlc::String gen_integer_literal(int integer_value) noexcept{return mlc::to_string(integer_value);}

mlc::String gen_string_literal(mlc::String string_value) noexcept{return mlc::String("mlc::String(\"") + context::escape_str(string_value) + mlc::String("\", ") + mlc::to_string(string_value.length()) + mlc::String(")");}

mlc::String gen_boolean_literal(bool boolean_value) noexcept{return boolean_value ? mlc::String("true") : mlc::String("false");}

mlc::String gen_unit_literal() noexcept{return mlc::String("/* unit */");}

mlc::String gen_extern_placeholder() noexcept{return mlc::String("");}

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = context::lookup_fields(context.field_orders, context.self_type);
bool is_known_field = name == mlc::String("errors") || name == mlc::String("kind") || name == mlc::String("tokens") || name == mlc::String("line") || name == mlc::String("col") || name == mlc::String("inferred_type") || name == mlc::String("type_env");
return context::list_contains(self_fields, name) || is_known_field ? mlc::String("self.") + context::cpp_safe(name) : [&]() -> mlc::String { 
  mlc::String mapped = context::map_builtin(name);
  mlc::String resolved = context::context_resolve(context, mapped);
  bool needs_constructor_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_constructor_brace ? resolved + mlc::String("{}") : resolved;
 }();
}

mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept{return context.self_type.length() > 0 ? resolve_identifier_in_self_context(name, context) : [&]() -> mlc::String { 
  mlc::String mapped = context::map_builtin(name);
  mlc::String resolved = context::context_resolve(context, mapped);
  bool needs_constructor_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_constructor_brace ? resolved + mlc::String("{}") : resolved;
 }();}

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr, context::CodegenContext context) noexcept{return mlc::String("(") + gen_expr(left_expr, context) + mlc::String(" ") + operation + mlc::String(" ") + gen_expr(right_expr, context) + mlc::String(")");}

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context) noexcept{return mlc::String("(") + operation + gen_expr(inner_expr, context) + mlc::String(")");}

mlc::String gen_call_function_code(std::shared_ptr<ast::Expr> function_expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function_expr)._); auto [name, _w0] = _v_exprident; return context::context_resolve(context, context::map_builtin(name)); } return gen_expr(function_expr, context); }();}

bool is_constructor_call(std::shared_ptr<ast::Expr> function_expr) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*function_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function_expr)._); auto [name, _w0] = _v_exprident; return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z"); } return false; }();}

mlc::String gen_call_expr(std::shared_ptr<ast::Expr> function_expr, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept{
mlc::String function_code = gen_call_function_code(function_expr, context);
mlc::String argument_list = gen_argument_list(arguments, context);
return is_constructor_call(function_expr) ? function_code + mlc::String("{") + argument_list + mlc::String("}") : function_code + mlc::String("(") + argument_list + mlc::String(")");
}

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = context::lookup_fields(context.field_orders, context.self_type);
bool is_known_field = object_name == mlc::String("errors") || object_name == mlc::String("kind") || object_name == mlc::String("tokens") || object_name == mlc::String("line") || object_name == mlc::String("col") || object_name == mlc::String("inferred_type") || object_name == mlc::String("type_env");
return context::list_contains(self_fields, object_name) || is_known_field ? mlc::String("self.") + context::cpp_safe(object_name) : context::context_resolve(context, context::map_builtin(object_name));
}

mlc::String gen_object_code(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return context.self_type.length() > 0 ? resolve_object_code_in_self_context(name, context) : context::context_resolve(context, context::map_builtin(name)); } return gen_expr(object, context); }();}

mlc::String infer_shared_new_type_name(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept{
mlc::String raw_type_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprRecord>((*argument)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*argument)._); auto [name, _w0, _w1] = _v_exprrecord; return name; } if (std::holds_alternative<ast::ExprCall>((*argument)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*argument)._); auto [function_expr, _w0, _w1] = _v_exprcall; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function_expr)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }(); } if (std::holds_alternative<ast::ExprIdent>((*argument)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*argument)._); auto [name, _w0] = _v_exprident; return name; } if (std::holds_alternative<ast::ExprIndex>((*argument)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*argument)._); auto [array_expr, _w0, _w1] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*array_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*array_expr)._); auto [array_name, _w0] = _v_exprident; return context.array_elem_types.has(array_name) ? context.array_elem_types.get(array_name) : mlc::String(""); } return mlc::String(""); }(); } return mlc::String(""); }();
mlc::String parent_type = raw_type_name.length() > 0 && context.variant_types.has(raw_type_name) ? context.variant_types.get(raw_type_name) : raw_type_name;
return parent_type.length() > 0 ? context::context_resolve(context, parent_type) : mlc::String("auto");
}

mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept{
mlc::String cpp_function = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : method_name == mlc::String("write") ? mlc::String("mlc::file::write_string") : mlc::String("mlc::file::") + method_name;
return cpp_function + mlc::String("(") + gen_argument_list(arguments, context) + mlc::String(")");
}

mlc::String gen_method_shared_new(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept{
mlc::String argument_code = gen_expr(argument, context);
mlc::String type_name = infer_shared_new_type_name(argument, context);
return mlc::String("std::make_shared<") + type_name + mlc::String(">(") + argument_code + mlc::String(")");
}

mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept{
mlc::String mangled_name = context.method_owners.get(method_name);
mlc::String function_name = context::context_resolve(context, mangled_name);
return arguments.size() == 0 ? function_name + mlc::String("(") + object_code + mlc::String(")") : function_name + mlc::String("(") + object_code + mlc::String(", ") + gen_argument_list(arguments, context) + mlc::String(")");
}

mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept{
mlc::String call_base = object_code + mlc::String(".") + context::map_method(method_name) + mlc::String("(");
return arguments.size() == 0 ? call_base + mlc::String(")") : call_base + gen_argument_list(arguments, context) + mlc::String(")");
}

mlc::String gen_method_expr(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments, context::CodegenContext context) noexcept{
mlc::String object_code = gen_object_code(object, context);
return object_code == mlc::String("File") ? gen_method_file(method_name, arguments, context) : object_code == mlc::String("Map") && method_name == mlc::String("new") ? mlc::String("{}") : object_code == mlc::String("Shared") && method_name == mlc::String("new") && arguments.size() == 1 ? gen_method_shared_new(arguments[0], context) : method_name == mlc::String("to_string") && arguments.size() == 0 ? mlc::String("mlc::to_string(") + object_code + mlc::String(")") : context.method_owners.has(method_name) ? gen_method_owner_call(object_code, method_name, arguments, context) : gen_method_builtin(object_code, method_name, arguments, context);
}

mlc::String field_access_operator(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIndex>((*object)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*object)._); auto [inner_expr, _w0, _w1] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*inner_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*inner_expr)._); auto [array_name, _w0] = _v_exprident; return context.shared_array_params.contains(array_name) ? mlc::String("->") : mlc::String("."); } return mlc::String("."); }(); } if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return context.shared_params.contains(name) ? mlc::String("->") : mlc::String("."); } if (std::holds_alternative<ast::ExprMethod>((*object)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*object)._); auto [map_object, method_name, _w0, _w1] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*map_object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_object)._); auto [map_name, _w0] = _v_exprident; return context.shared_map_params.contains(map_name) ? mlc::String("->") : mlc::String("."); } return mlc::String("."); }() : mlc::String("."); } return mlc::String("."); }();}

mlc::String gen_field_expr(std::shared_ptr<ast::Expr> object, mlc::String field_name, context::CodegenContext context) noexcept{
mlc::String object_code = gen_expr(object, context);
mlc::String operator_ = field_access_operator(object, context);
return object_code + operator_ + context::cpp_safe(field_name);
}

mlc::String gen_index_expr(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expr, context::CodegenContext context) noexcept{return gen_expr(object, context) + mlc::String("[") + gen_expr(index_expr, context) + mlc::String("]");}

mlc::String gen_if_expr(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expr, std::shared_ptr<ast::Expr> else_expr, context::CodegenContext context) noexcept{return mlc::String("(") + gen_expr(condition, context) + mlc::String(" ? ") + gen_expr(then_expr, context) + mlc::String(" : ") + gen_expr(else_expr, context) + mlc::String(")");}

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expr, context::CodegenContext context) noexcept{return statements.size() == 0 ? gen_expr(result_expr, context) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(statements, context) + mlc::String("return ") + gen_expr(result_expr, stmts_final_ctx(statements, context)) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }();}

mlc::String gen_while_expr(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept{return mlc::String("[&]() {\nwhile (") + gen_expr(condition, context) + mlc::String(") {\n") + gen_stmts_str(statements, context) + mlc::String("}\n}()");}

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept{return mlc::String("[&]() {\nfor (auto ") + context::cpp_safe(variable) + mlc::String(" : ") + gen_expr(iterator, context) + mlc::String(") {\n") + gen_stmts_str(statements, context) + mlc::String("}\n}()");}

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> field_order = context::lookup_fields(context.field_orders, type_name);
mlc::String values_code = field_order.size() > 0 ? gen_record_ordered(field_values, field_order, context) : gen_record_unordered(field_values, context);
return context::context_resolve(context, type_name) + mlc::String("{") + values_code + mlc::String("}");
}

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<ast::Expr> base_expr, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> field_order = context::lookup_fields(context.field_orders, type_name);
return field_order.size() > 0 ? context::context_resolve(context, type_name) + mlc::String("{") + gen_record_update_ordered(base_expr, overrides, field_order, context) + mlc::String("}") : gen_record_update_lazy(base_expr, overrides, context);
}

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<ast::Expr>> elements, context::CodegenContext context) noexcept{return elements.size() == 0 ? mlc::String("{}") : mlc::String("mlc::Array{") + gen_argument_list(elements, context) + mlc::String("}");}

mlc::String gen_question_expr(std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context) noexcept{
mlc::String inner_code = gen_expr(inner_expr, context);
return mlc::String("({ auto __q = ") + inner_code + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })");
}

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<ast::Expr> body_expr, context::CodegenContext context) noexcept{
mlc::String capture = parameters.size() == 0 ? mlc::String("[]") : mlc::String("[=]");
mlc::String parameter_list = parameters.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < parameters.size()){
{
parts.push_back(mlc::String("auto ") + context::cpp_safe(parameters[index]));
index = index + 1;
}
}
  return parts.join(mlc::String(", "));
 }();
return capture + mlc::String("(") + parameter_list + mlc::String(") { return ") + gen_expr(body_expr, context) + mlc::String("; }");
}

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> mlc::String { auto [integer_value, _w0] = exprint; return gen_integer_literal(integer_value); },
  [&](const ExprStr& exprstr) -> mlc::String { auto [string_value, _w0] = exprstr; return gen_string_literal(string_value); },
  [&](const ExprBool& exprbool) -> mlc::String { auto [boolean_value, _w0] = exprbool; return gen_boolean_literal(boolean_value); },
  [&](const ExprUnit& exprunit) -> mlc::String { auto [_w0] = exprunit; return gen_unit_literal(); },
  [&](const ExprExtern& exprextern) -> mlc::String { auto [_w0] = exprextern; return gen_extern_placeholder(); },
  [&](const ExprIdent& exprident) -> mlc::String { auto [name, _w0] = exprident; return gen_identifier(name, context); },
  [&](const ExprBin& exprbin) -> mlc::String { auto [operation, left_expr, right_expr, _w0] = exprbin; return gen_binary_expr(operation, left_expr, right_expr, context); },
  [&](const ExprUn& exprun) -> mlc::String { auto [operation, inner_expr, _w0] = exprun; return gen_unary_expr(operation, inner_expr, context); },
  [&](const ExprCall& exprcall) -> mlc::String { auto [function_expr, arguments, _w0] = exprcall; return gen_call_expr(function_expr, arguments, context); },
  [&](const ExprMethod& exprmethod) -> mlc::String { auto [object, method_name, arguments, _w0] = exprmethod; return gen_method_expr(object, method_name, arguments, context); },
  [&](const ExprField& exprfield) -> mlc::String { auto [object, field_name, _w0] = exprfield; return gen_field_expr(object, field_name, context); },
  [&](const ExprIndex& exprindex) -> mlc::String { auto [object, index_expr, _w0] = exprindex; return gen_index_expr(object, index_expr, context); },
  [&](const ExprIf& exprif) -> mlc::String { auto [condition, then_expr, else_expr, _w0] = exprif; return gen_if_expr(condition, then_expr, else_expr, context); },
  [&](const ExprBlock& exprblock) -> mlc::String { auto [statements, result_expr, _w0] = exprblock; return gen_block_expr(statements, result_expr, context); },
  [&](const ExprWhile& exprwhile) -> mlc::String { auto [condition, statements, _w0] = exprwhile; return gen_while_expr(condition, statements, context); },
  [&](const ExprFor& exprfor) -> mlc::String { auto [variable, iterator, statements, _w0] = exprfor; return gen_for_expr(variable, iterator, statements, context); },
  [&](const ExprMatch& exprmatch) -> mlc::String { auto [subject, arms, _w0] = exprmatch; return gen_match(subject, arms, context); },
  [&](const ExprRecord& exprrecord) -> mlc::String { auto [type_name, field_values, _w0] = exprrecord; return gen_record_expr(type_name, field_values, context); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> mlc::String { auto [type_name, base_expr, overrides, _w0] = exprrecordupdate; return gen_record_update_expr(type_name, base_expr, overrides, context); },
  [&](const ExprArray& exprarray) -> mlc::String { auto [elements, _w0] = exprarray; return gen_array_expr(elements, context); },
  [&](const ExprQuestion& exprquestion) -> mlc::String { auto [inner_expr, _w0] = exprquestion; return gen_question_expr(inner_expr, context); },
  [&](const ExprLambda& exprlambda) -> mlc::String { auto [parameters, body_expr, _w0] = exprlambda; return gen_lambda_expr(parameters, body_expr, context); }
}, (*expr)._);}

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept{
context::CodegenContext current_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
current_context = context::update_context_from_statement(statements[index], current_context);
index = index + 1;
}
}
return current_context;
}

mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept{
mlc::String inner_code = gen_expr(inner_expr, context);
return mlc::String("auto ") + try_identifier + mlc::String(" = ") + inner_code + mlc::String(";\n") + mlc::String("if (std::get_if<1>(&") + try_identifier + mlc::String(")) return *std::get_if<1>(&") + try_identifier + mlc::String(");\n") + success_line;
}

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> context::GenStmtResult { auto [name, _w0, typ, value, _w1] = stmtlet; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  int next_counter = try_counter + 1;
  context::GenStmtResult result = [&]() -> context::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*value)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*value)._); auto [inner_expr, _w0] = _v_exprquestion; return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, mlc::String("auto ") + context::cpp_safe(name) + mlc::String(" = std::get<0>(") + try_identifier + mlc::String(").field0;\n")), next_counter}; } if (std::holds_alternative<ast::ExprArray>((*value)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*value)._); auto [elements, _w0] = _v_exprarray; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = elements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*typ))) { auto _v_tyarray = std::get<ast::TyArray>((*typ)); auto [inner_type] = _v_tyarray; return mlc::String("mlc::Array<") + type_gen::type_to_cpp(context, inner_type) + mlc::String(">{}"); } return mlc::String("mlc::Array<mlc::String>{}"); }() : mlc::String("mlc::Array{") + gen_argument_list(elements, context) + mlc::String("}");
  return context::GenStmtResult{mlc::String("auto ") + context::cpp_safe(name) + mlc::String(" = ") + value_code + mlc::String(";\n"), try_counter};
 }(); } if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_object, method_name, _w0, _w1] = _v_exprmethod; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = method_name == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*map_object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_object)._); auto [object_name, _w0] = _v_exprident; return object_name == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyGeneric>((*typ))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*typ)); auto [type_name, type_arguments] = _v_tygeneric; return type_name == mlc::String("Map") && type_arguments.size() == 2 ? mlc::String("mlc::HashMap<") + type_gen::type_to_cpp(context, type_arguments[0]) + mlc::String(", ") + type_gen::type_to_cpp(context, type_arguments[1]) + mlc::String(">()") : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context);
  return context::GenStmtResult{mlc::String("auto ") + context::cpp_safe(name) + mlc::String(" = ") + value_code + mlc::String(";\n"), try_counter};
 }(); } return context::GenStmtResult{mlc::String("auto ") + context::cpp_safe(name) + mlc::String(" = ") + gen_expr(value, context) + mlc::String(";\n"), try_counter}; }();
  return result;
 }(); },
  [&](const StmtExpr& stmtexpr) -> context::GenStmtResult { auto [expression, _w0] = stmtexpr; return [&]() -> context::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expr, _w0] = _v_exprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, mlc::String("/* discard */ (void)std::get<0>(") + try_identifier + mlc::String(").field0;\n")), try_counter + 1};
 }(); } return context::GenStmtResult{gen_stmt_expr(expression, context), try_counter}; }(); },
  [&](const StmtBreak& stmtbreak) -> context::GenStmtResult { auto [_w0] = stmtbreak; return context::GenStmtResult{mlc::String("break;\n"), try_counter}; },
  [&](const StmtContinue& stmtcontinue) -> context::GenStmtResult { auto [_w0] = stmtcontinue; return context::GenStmtResult{mlc::String("continue;\n"), try_counter}; },
  [&](const StmtReturn& stmtreturn) -> context::GenStmtResult { auto [expression, _w0] = stmtreturn; return [&]() -> context::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expr, _w0] = _v_exprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_ret_") + mlc::to_string(try_counter);
  return context::GenStmtResult{gen_try_unwrap(inner_expr, context, try_identifier, mlc::String("return std::get<0>(") + try_identifier + mlc::String(").field0;\n")), try_counter + 1};
 }(); } return context::GenStmtResult{mlc::String("return ") + gen_expr(expression, context) + mlc::String(";\n"), try_counter}; }(); }
}, (*stmt)._);}

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left_expr, right_expr, _w0] = _v_exprbin; return operation == mlc::String("=") ? gen_expr(left_expr, context) + mlc::String(" = ") + gen_expr(right_expr, context) + mlc::String(";\n") : mlc::String("(") + gen_expr(left_expr, context) + mlc::String(" ") + operation + mlc::String(" ") + gen_expr(right_expr, context) + mlc::String(");\n"); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expr, else_expr, _w0] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(condition, context) + mlc::String(") {\n") + gen_block_body(then_expr, context) + mlc::String("}");
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*else_expr)._); auto [_w0] = _v_exprunit; return mlc::String(""); } if (std::holds_alternative<ast::ExprBlock>((*else_expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*else_expr)._); auto [statements, result_expr, _w0] = _v_exprblock; return statements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*result_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*result_expr)._); auto [_w0] = _v_exprunit; return mlc::String(""); } return mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); }() : mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return mlc::String(" else ") + gen_stmt_expr(else_expr, context); } return mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return mlc::String("while (") + gen_expr(condition, context) + mlc::String(") {\n") + gen_stmts_str(statements, context) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable, iterator, statements, _w0] = _v_exprfor; return mlc::String("for (auto ") + context::cpp_safe(variable) + mlc::String(" : ") + gen_expr(iterator, context) + mlc::String(") {\n") + gen_stmts_str(statements, context) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result_expr, _w0] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = gen_stmts_str(statements, context);
  mlc::String result_code = gen_expr(result_expr, stmts_final_ctx(statements, context));
  return result_code == mlc::String("/* unit */") ? statements_code : statements_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expression, context) + mlc::String(";\n"); }();}

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [statements, result_expr, _w0] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = gen_stmts_str(statements, context);
  context::CodegenContext final_context = stmts_final_ctx(statements, context);
  return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*result_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return statements_code + gen_stmt_expr(result_expr, final_context); } if (std::holds_alternative<ast::ExprUnit>((*result_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*result_expr)._); auto [_w0] = _v_exprunit; return statements_code; } return [&]() -> mlc::String { 
  mlc::String result_code = gen_expr(result_expr, final_context);
  return result_code == mlc::String("/* unit */") ? statements_code : statements_code + result_code + mlc::String(";\n");
 }(); }();
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return gen_stmt_expr(expr, context); } return gen_expr(expr, context) + mlc::String(";\n"); }();}

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context, int try_counter) noexcept{
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

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept{
context::GenStmtsResult result = gen_stmts_str_with_try(statements, context, 0);
return result.code;
}

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(condition, context) + mlc::String(") {\n") + gen_return_body(then_expr, context) + mlc::String("}");
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*else_expr)._); auto [_w0] = _v_exprunit; return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, context); } return mlc::String(" else {\n") + gen_return_body(else_expr, context) + mlc::String("}"); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("if (true) {\n") + gen_return_body(expr, context) + mlc::String("}\n"); }();}

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [statements, result_expr, _w0] = _v_exprblock; return [&]() -> mlc::String { 
  context::CodegenContext final_context = stmts_final_ctx(statements, context);
  mlc::String statements_code = gen_stmts_str(statements, context);
  return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprQuestion>((*result_expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*result_expr)._); auto [inner_expr, _w0] = _v_exprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return statements_code + gen_try_unwrap(inner_expr, final_context, try_identifier, mlc::String("return std::get<0>(") + try_identifier + mlc::String(").field0;\n"));
 }(); } if (std::holds_alternative<ast::ExprUnit>((*result_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*result_expr)._); auto [_w0] = _v_exprunit; return statements_code; } if (std::holds_alternative<ast::ExprIf>((*result_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<ast::ExprMatch>((*result_expr)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*result_expr)._); auto [_w0, _w1, _w2] = _v_exprmatch; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<ast::ExprBlock>((*result_expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*result_expr)._); auto [_w0, _w1, _w2] = _v_exprblock; return statements_code + gen_return_body(result_expr, final_context); } return statements_code + mlc::String("return ") + gen_expr(result_expr, final_context) + mlc::String(";\n"); }();
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(condition, context) + mlc::String(") {\n") + gen_return_body(then_expr, context) + mlc::String("}");
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*else_expr)._); auto [_w0] = _v_exprunit; return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, context); } return mlc::String(" else {\n") + gen_return_body(else_expr, context) + mlc::String("}"); }();
  if (else_string.length() > 0){
{
output = output + else_string;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expr)._); auto [inner_expr, _w0] = _v_exprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return gen_try_unwrap(inner_expr, context, try_identifier, mlc::String("return std::get<0>(") + try_identifier + mlc::String(").field0;\n"));
 }(); } return mlc::String("return ") + gen_expr(expr, context) + mlc::String(";\n"); }();}

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body_expr, context::CodegenContext context) noexcept{return gen_return_body(body_expr, context);}

} // namespace eval

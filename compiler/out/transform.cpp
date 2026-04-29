#include "transform.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "semantic_ir.hpp"
#include "semantic_ir.hpp"

namespace transform {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace result_option_method_types;
using namespace pattern_env;
using namespace semantic_type_structure;
using namespace semantic_ir;
using namespace semantic_ir;
using namespace ast_tokens;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> tunk_args(int count) noexcept;

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;

bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;

int transform_find_param_slot(mlc::Array<mlc::String> param_names, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{return transform::TransformContext{type_env, registry};}

transform::TransformContext empty_transform_context() noexcept{return transform::TransformContext{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), registry::empty_registry()};}

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{return transform::TransformContext{type_env, base.registry};}

mlc::Array<std::shared_ptr<registry::Type>> tunk_args(int count) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> args = {};
int i = 0;
while (i < count){
{
args.push_back(std::make_shared<registry::Type>((registry::TUnknown{})));
i = i + 1;
}
}
return args;
}

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<semantic_ir::SExprArray>((*expression)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*expression)._); auto [elements, _w0, source_span] = _v_sexprarray; return elements.size() == 0 ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, target_type, source_span)) : expression; } if (std::holds_alternative<semantic_ir::SExprRecord>((*expression)._)) { auto _v_sexprrecord = std::get<semantic_ir::SExprRecord>((*expression)._); auto [type_name, fields, expr_type, source_span] = _v_sexprrecord; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TGeneric>((*target_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*target_type)); auto [tgt_name, _w0] = _v_tgeneric; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TGeneric>((*expr_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expr_type)); auto [exp_name, _w0] = _v_tgeneric; return tgt_name == exp_name ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(type_name, fields, target_type, source_span)) : expression; } return expression; }(); } return expression; }(); } return expression; }();}

bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{
int i = 0;
bool found = false;
while (i < args.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::ExprNamedArg>((*args[i])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*args[i])._); auto [_w0, _w1, _w2] = _v_exprnamedarg; return [&]() -> std::tuple<> { 
  found = true;
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
i = i + 1;
}
}
return found;
}

int transform_find_param_slot(mlc::Array<mlc::String> param_names, mlc::String name) noexcept{
int i = 0;
int slot = -1;
while (i < param_names.size()){
{
if (param_names[i] == name){
{
slot = i;
}
}
i = i + 1;
}
}
return slot;
}

mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> stripped = {};
int i = 0;
while (i < args.size()){
{
[&]() -> void { if (std::holds_alternative<ast::ExprNamedArg>((*args[i])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*args[i])._); auto [_w0, inner, _w1] = _v_exprnamedarg; return stripped.push_back(inner); } return stripped.push_back(args[i]); }();
i = i + 1;
}
}
return stripped;
}

mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept{
int n = param_names.size();
mlc::Array<std::shared_ptr<ast::Expr>> result = {};
mlc::Array<int> filled = {};
int k = 0;
while (k < n){
{
result.push_back(ast::expr_placeholder());
filled.push_back(0);
k = k + 1;
}
}
int pos_slot = 0;
int i = 0;
while (i < args.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::ExprNamedArg>((*args[i])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*args[i])._); auto [name, inner, _w0] = _v_exprnamedarg; return [&]() -> std::tuple<> { 
  int slot = transform_find_param_slot(param_names, name);
  if (slot >= 0 && filled[slot] == 0){
{
result.set(slot, inner);
filled.set(slot, 1);
}
}
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  while (pos_slot < n && filled[pos_slot] == 1){
{
pos_slot = pos_slot + 1;
}
}
  if (pos_slot < n){
{
result.set(pos_slot, args[i]);
filled.set(pos_slot, 1);
pos_slot = pos_slot + 1;
}
}
  return std::make_tuple();
 }(); }();
i = i + 1;
}
}
mlc::Array<std::shared_ptr<ast::Expr>> compact = {};
int j = 0;
while (j < n){
{
if (filled[j] == 1){
{
compact.push_back(result[j]);
}
}
j = j + 1;
}
}
return compact;
}

mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept{return !transform_has_named_args(args) ? args : [&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
  return param_names.size() == 0 ? transform_strip_labels(args) : transform_reorder_to_positional(args, param_names);
 }();}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> result = {};
int index = 0;
while (index < expressions.size()){
{
result.push_back(transform_expr(expressions[index], transform_context, stmts_fn));
index = index + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> result = {};
int index = 0;
while (index < field_values.size()){
{
std::shared_ptr<semantic_ir::SExpr> typed_val = transform_expr(field_values[index]->val, transform_context, stmts_fn);
result.push_back(std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{field_values[index]->name, typed_val}));
index = index + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*subject_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*subject_type)); auto [type_name, type_args] = _v_tgeneric; return [&]() { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
  int i = 0;
  return [&]() { 
  while (i < param_names.size() && i < type_args.size()){
{
substitution.set(param_names[i], type_args[i]);
i = i + 1;
}
}
 }();
 }(); } return; }();
return substitution;
}

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> result = {};
int index = 0;
while (index < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_env = pattern_env::env_for_pattern_substituted(transform_context.type_env, arms[index]->pat, transform_context.registry, substitution);
transform::TransformContext arm_context = transform_context_with_env(transform_context, arm_env);
std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(arms[index]->body, arm_context, stmts_fn);
result.push_back(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{arms[index]->pat, typed_body}));
index = index + 1;
}
}
return result;
}

std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_env = transform_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> parameter_type_vec = {};
int parameter_index = 0;
while (parameter_index < parameter_names.size()){
{
std::shared_ptr<registry::Type> pt = parameter_index < parameter_types.size() ? parameter_types[parameter_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
parameter_type_vec.push_back(pt);
lambda_env.set(parameter_names[parameter_index], pt);
parameter_index = parameter_index + 1;
}
}
transform::TransformContext lambda_context = transform_context_with_env(transform_context, lambda_env);
std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(body, lambda_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn(parameter_type_vec, semantic_ir::sexpr_type(typed_body))), source_span));
}

std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, source_span] = _v_exprlambda; return transform_expr_lambda_with_param_types(parameter_names, expected_param_types, body, source_span, transform_context, stmts_fn); } return transform_expr(expression, transform_context, stmts_fn); }();}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> out = {};
if (method_name == mlc::String("fold")){
{
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
if (method_arguments.size() > 1){
std::shared_ptr<registry::Type> acc_t = semantic_ir::sexpr_type(out[0]);
out.push_back(transform_expr_maybe_lambda_context(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{acc_t, element_type}, transform_context, stmts_fn));
}
}
}
} else {
{
if (method_name == mlc::String("zip") || method_name == mlc::String("join")){
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else {
if (method_name == mlc::String("take") || method_name == mlc::String("drop")){
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else {
if (method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat")){
int _skip = 0;
} else {
if (method_arguments.size() > 0){
out.push_back(transform_expr_maybe_lambda_context(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, transform_context, stmts_fn));
}
}
}
}
}
}
return out;
}

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [value, source_span] = _v_exprint; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt(value, std::make_shared<registry::Type>((registry::TI32{})), source_span)); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [value, source_span] = _v_exprstr; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr(value, std::make_shared<registry::Type>((registry::TString{})), source_span)); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [value, source_span] = _v_exprbool; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(value, std::make_shared<registry::Type>((registry::TBool{})), source_span)); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [source_span] = _v_exprunit; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [value, source_span] = _v_exprfloat; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFloat(value, std::make_shared<registry::Type>((registry::TF64{})), source_span)); } if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); auto [value, source_span] = _v_expri64; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprI64(value, std::make_shared<registry::Type>((registry::TI64{})), source_span)); } if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); auto [value, source_span] = _v_expru8; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprU8(value, std::make_shared<registry::Type>((registry::TU8{})), source_span)); } if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); auto [value, source_span] = _v_exprusize; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUsize(value, std::make_shared<registry::Type>((registry::TUsize{})), source_span)); } if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); auto [value, source_span] = _v_exprchar; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprChar(value, std::make_shared<registry::Type>((registry::TChar{})), source_span)); } if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [source_span] = _v_exprextern; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprExtern(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, source_span] = _v_exprident; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<registry::Type> resolved_type = transform_context.type_env.has(name) ? transform_context.type_env.get(name) : registry::TypeRegistry_has_fn(transform_context.registry, name) ? registry::TypeRegistry_fn_type(transform_context.registry, name) : registry::TypeRegistry_has_ctor(transform_context.registry, name) ? registry::TypeRegistry_ctor_type(transform_context.registry, name) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(name, resolved_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, source_span] = _v_exprbin; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_left = transform_expr(left, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_right = transform_expr(right, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = semantic_type_structure::binary_operation_result_type(operation, semantic_ir::sexpr_type(typed_left));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(operation, typed_left, typed_right, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner, source_span] = _v_exprun; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context, stmts_fn);
  auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : semantic_ir::sexpr_type(typed_inner);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn(operation, typed_inner, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, source_span] = _v_exprcall; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
  mlc::Array<std::shared_ptr<ast::Expr>> resolved_call_args = reorder_named_args_for_transform(call_arguments, callee_name, transform_context.registry);
  std::shared_ptr<semantic_ir::SExpr> typed_fn = transform_expr(function, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(resolved_call_args, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(transform_context.registry, callee_name) ? registry::TypeRegistry_ctor_type(transform_context.registry, callee_name) : [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*semantic_ir::sexpr_type(typed_fn)))) { auto _v_tfn = std::get<registry::TFn>((*semantic_ir::sexpr_type(typed_fn))); auto [_w0, return_type] = _v_tfn; return return_type; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(typed_fn, typed_args, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, method_name, method_arguments, source_span] = _v_exprmethod; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  return result_option_method_types::should_infer_result_option_combinator(semantic_ir::sexpr_type(typed_object), method_name) ? [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> arg_types = {};
  int arg_index = 0;
  while (arg_index < typed_args.size()){
{
arg_types.push_back(semantic_ir::sexpr_type(typed_args[arg_index]));
arg_index = arg_index + 1;
}
}
  std::shared_ptr<registry::Type> result_type = result_option_method_types::result_option_hof_call_result_type(semantic_ir::sexpr_type(typed_object), method_name, arg_types);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, result_type, source_span));
 }() : semantic_type_structure::type_is_array(semantic_ir::sexpr_type(typed_object)) && array_method_types::is_array_hof_method(method_name) ? [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> arg_types = {};
  int arg_index = 0;
  while (arg_index < typed_args.size()){
{
arg_types.push_back(semantic_ir::sexpr_type(typed_args[arg_index]));
arg_index = arg_index + 1;
}
}
  std::shared_ptr<registry::Type> elem = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
  std::shared_ptr<registry::Type> result_type = array_method_types::array_hof_call_result_type(elem, method_name, arg_types);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, result_type, source_span));
 }() : [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
  std::shared_ptr<registry::Type> result_type = semantic_type_structure::type_is_unknown(builtin_type) ? registry::method_return_type_from_object(semantic_ir::sexpr_type(typed_object), method_name, transform_context.registry) : builtin_type;
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, result_type, source_span));
 }();
 }(); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, field_name, source_span] = _v_exprfield; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, transform_context.registry);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(typed_object, field_name, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, source_span] = _v_exprindex; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_index = transform_expr(index_expression, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_object)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_object))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex(typed_object, typed_index, element_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, source_span] = _v_exprif; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_then = transform_expr(then_expression, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_else = transform_expr(else_expression, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(typed_then);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(typed_condition, typed_then, typed_else, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, source_span] = _v_exprblock; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, transform_context);
  transform::TransformContext body_context = transform_context_with_env(transform_context, stmts_result.type_env);
  std::shared_ptr<semantic_ir::SExpr> typed_result = transform_expr(result, body_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(typed_result);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(stmts_result.statements, typed_result, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, source_span] = _v_exprwhile; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context, stmts_fn);
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, transform_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile(typed_condition, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable_name, iterator, statements, source_span] = _v_exprfor; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_iterator = transform_expr(iterator, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = transform_context.type_env;
  inner_env.set(variable_name, element_type);
  transform::TransformContext loop_context = transform_context_with_env(transform_context, inner_env);
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, loop_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFor(variable_name, typed_iterator, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, source_span] = _v_exprmatch; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_subject = transform_expr(subject, transform_context, stmts_fn);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = build_transform_substitution(semantic_ir::sexpr_type(typed_subject), transform_context.registry);
  mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> typed_arms = transform_match_arms(arms, transform_context, substitution, stmts_fn);
  std::shared_ptr<registry::Type> result_type = typed_arms.size() > 0 ? semantic_ir::sexpr_type(typed_arms[0]->body) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(typed_subject, typed_arms, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [type_name, field_values, source_span] = _v_exprrecord; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
  mlc::Array<mlc::String> phantom = registry::TypeRegistry_phantom_type_params_for(transform_context.registry, type_name);
  mlc::Array<mlc::String> all_params = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(transform_context.registry, type_name);
  std::shared_ptr<registry::Type> expr_type = all_params.size() > 0 && phantom.size() == all_params.size() ? std::make_shared<registry::Type>(registry::TGeneric(type_name, tunk_args(all_params.size()))) : std::make_shared<registry::Type>(registry::TNamed(type_name));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(type_name, typed_fields, expr_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [type_name, base, field_values, source_span] = _v_exprrecordupdate; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_base = transform_expr(base, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate(type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed(type_name)), source_span));
 }(); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, source_span] = _v_exprarray; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_elements = transform_exprs(elements, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = typed_elements.size() > 0 ? semantic_ir::sexpr_type(typed_elements[0]) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(typed_elements, std::make_shared<registry::Type>(registry::TArray(element_type)), source_span));
 }(); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, source_span] = _v_exprtuple; return elements.size() < 2 ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown())) : [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed = transform_exprs(elements, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> tuple_types = {};
  int ti = 0;
  while (ti < typed.size()){
{
tuple_types.push_back(semantic_ir::sexpr_type(typed[ti]));
ti = ti + 1;
}
}
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprTuple(typed, std::make_shared<registry::Type>(registry::TTuple(tuple_types)), source_span));
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, source_span] = _v_exprquestion; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> inner_type = semantic_ir::sexpr_type(typed_inner);
  std::shared_ptr<registry::Type> unwrapped_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [_w0, type_args] = _v_tgeneric; return type_args.size() > 0 ? type_args[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion(typed_inner, unwrapped_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, source_span] = _v_exprlambda; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_env = transform_context.type_env;
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = {};
  int parameter_index = 0;
  while (parameter_index < parameter_names.size()){
{
parameter_types.push_back(std::make_shared<registry::Type>((registry::TUnknown{})));
lambda_env.set(parameter_names[parameter_index], std::make_shared<registry::Type>((registry::TUnknown{})));
parameter_index = parameter_index + 1;
}
}
  transform::TransformContext lambda_context = transform_context_with_env(transform_context, lambda_env);
  std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(body, lambda_context, stmts_fn);
  std::shared_ptr<registry::Type> function_type = std::make_shared<registry::Type>(registry::TFn(parameter_types, semantic_ir::sexpr_type(typed_body)));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(parameter_names, typed_body, function_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner, _w1] = _v_exprnamedarg; return transform_expr(inner, transform_context, stmts_fn); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, binder, stmts, source_span] = _v_exprwith; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_resource = transform_expr(resource, transform_context, stmts_fn);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = transform_context.type_env;
  inner_env.set(binder, semantic_ir::sexpr_type(typed_resource));
  transform::TransformContext inner_context = transform_context_with_env(transform_context, inner_env);
  transform::TransformStmtsResult stmts_result = stmts_fn(stmts, inner_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWith(typed_resource, binder, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown())); }();}

} // namespace transform

#include "transform.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "semantic_ir.hpp"

namespace transform {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace pattern_env;
using namespace semantic_type_structure;
using namespace semantic_ir;
using namespace ast_tokens;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context) noexcept;

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{return transform::TransformContext{type_env, registry};}

transform::TransformContext empty_transform_context() noexcept{return transform::TransformContext{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), registry::empty_registry()};}

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{return transform::TransformContext{type_env, base.registry};}

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<semantic_ir::SExprArray>((*expression)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*expression)._); auto [elements, _w0, source_span] = _v_sexprarray; return elements.size() == 0 ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, target_type, source_span)) : expression; } return expression; }();}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> result = {};
int index = 0;
while (index < expressions.size()){
{
result.push_back(transform_expr(expressions[index], transform_context));
index = index + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> result = {};
int index = 0;
while (index < field_values.size()){
{
std::shared_ptr<semantic_ir::SExpr> typed_val = transform_expr(field_values[index]->val, transform_context);
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

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> result = {};
int index = 0;
while (index < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_env = pattern_env::env_for_pattern_substituted(transform_context.type_env, arms[index]->pat, transform_context.registry, substitution);
transform::TransformContext arm_context = transform_context_with_env(transform_context, arm_env);
std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(arms[index]->body, arm_context);
result.push_back(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{arms[index]->pat, typed_body}));
index = index + 1;
}
}
return result;
}

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [value, source_span] = _v_exprint; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt(value, std::make_shared<registry::Type>((registry::TI32{})), source_span)); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [value, source_span] = _v_exprstr; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr(value, std::make_shared<registry::Type>((registry::TString{})), source_span)); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [value, source_span] = _v_exprbool; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(value, std::make_shared<registry::Type>((registry::TBool{})), source_span)); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [source_span] = _v_exprunit; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [source_span] = _v_exprextern; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprExtern(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, source_span] = _v_exprident; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<registry::Type> resolved_type = transform_context.type_env.has(name) ? transform_context.type_env.get(name) : registry::TypeRegistry_has_fn(transform_context.registry, name) ? registry::TypeRegistry_fn_type(transform_context.registry, name) : registry::TypeRegistry_has_ctor(transform_context.registry, name) ? registry::TypeRegistry_ctor_type(transform_context.registry, name) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(name, resolved_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, source_span] = _v_exprbin; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_left = transform_expr(left, transform_context);
  std::shared_ptr<semantic_ir::SExpr> typed_right = transform_expr(right, transform_context);
  std::shared_ptr<registry::Type> result_type = semantic_type_structure::binary_operation_result_type(operation, semantic_ir::sexpr_type(typed_left));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(operation, typed_left, typed_right, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner, source_span] = _v_exprun; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context);
  auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : semantic_ir::sexpr_type(typed_inner);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn(operation, typed_inner, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, source_span] = _v_exprcall; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_fn = transform_expr(function, transform_context);
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(call_arguments, transform_context);
  mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
  std::shared_ptr<registry::Type> result_type = callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(transform_context.registry, callee_name) ? registry::TypeRegistry_ctor_type(transform_context.registry, callee_name) : [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*semantic_ir::sexpr_type(typed_fn)))) { auto _v_tfn = std::get<registry::TFn>((*semantic_ir::sexpr_type(typed_fn))); auto [_w0, return_type] = _v_tfn; return return_type; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(typed_fn, typed_args, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, method_name, method_arguments, source_span] = _v_exprmethod; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context);
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(method_arguments, transform_context);
  std::shared_ptr<registry::Type> builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
  std::shared_ptr<registry::Type> result_type = semantic_type_structure::type_is_unknown(builtin_type) ? registry::method_return_type_from_object(semantic_ir::sexpr_type(typed_object), method_name, transform_context.registry) : builtin_type;
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, field_name, source_span] = _v_exprfield; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context);
  std::shared_ptr<registry::Type> result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, transform_context.registry);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(typed_object, field_name, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, source_span] = _v_exprindex; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context);
  std::shared_ptr<semantic_ir::SExpr> typed_index = transform_expr(index_expression, transform_context);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_object)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_object))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex(typed_object, typed_index, element_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, source_span] = _v_exprif; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context);
  std::shared_ptr<semantic_ir::SExpr> typed_then = transform_expr(then_expression, transform_context);
  std::shared_ptr<semantic_ir::SExpr> typed_else = transform_expr(else_expression, transform_context);
  std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(typed_then);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(typed_condition, typed_then, typed_else, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, source_span] = _v_exprblock; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  transform::TransformStmtsResult stmts_result = transform_stmts(statements, transform_context);
  transform::TransformContext body_context = transform_context_with_env(transform_context, stmts_result.type_env);
  std::shared_ptr<semantic_ir::SExpr> typed_result = transform_expr(result, body_context);
  std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(typed_result);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(stmts_result.statements, typed_result, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, source_span] = _v_exprwhile; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context);
  transform::TransformStmtsResult stmts_result = transform_stmts(statements, transform_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile(typed_condition, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable_name, iterator, statements, source_span] = _v_exprfor; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_iterator = transform_expr(iterator, transform_context);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = transform_context.type_env;
  inner_env.set(variable_name, element_type);
  transform::TransformContext loop_context = transform_context_with_env(transform_context, inner_env);
  transform::TransformStmtsResult stmts_result = transform_stmts(statements, loop_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFor(variable_name, typed_iterator, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, source_span] = _v_exprmatch; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_subject = transform_expr(subject, transform_context);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = build_transform_substitution(semantic_ir::sexpr_type(typed_subject), transform_context.registry);
  mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> typed_arms = transform_match_arms(arms, transform_context, substitution);
  std::shared_ptr<registry::Type> result_type = typed_arms.size() > 0 ? semantic_ir::sexpr_type(typed_arms[0]->body) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(typed_subject, typed_arms, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [type_name, field_values, source_span] = _v_exprrecord; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(field_values, transform_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(type_name, typed_fields, std::make_shared<registry::Type>(registry::TNamed(type_name)), source_span));
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [type_name, base, field_values, source_span] = _v_exprrecordupdate; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_base = transform_expr(base, transform_context);
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(field_values, transform_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate(type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed(type_name)), source_span));
 }(); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, source_span] = _v_exprarray; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_elements = transform_exprs(elements, transform_context);
  std::shared_ptr<registry::Type> element_type = typed_elements.size() > 0 ? semantic_ir::sexpr_type(typed_elements[0]) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(typed_elements, std::make_shared<registry::Type>(registry::TArray(element_type)), source_span));
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, source_span] = _v_exprquestion; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context);
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
  std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(body, lambda_context);
  std::shared_ptr<registry::Type> function_type = std::make_shared<registry::Type>(registry::TFn(parameter_types, semantic_ir::sexpr_type(typed_body)));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(parameter_names, typed_body, function_type, source_span));
 }(); } return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown())); }();}

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> typed_statements = {};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_env = transform_context.type_env;
int statement_index = 0;
while (statement_index < statements.size()){
{
transform::TransformContext current_context = transform_context_with_env(transform_context, current_env);
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [binding_name, is_mut, annotation, value_expression, source_span] = stmtlet; return [&]() -> std::tuple<> { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation(annotation);
  std::shared_ptr<semantic_ir::SExpr> typed_value = transform_expr(value_expression, current_context);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  current_env.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SExpr> coerced_value = coerce_expr_to_type(typed_value, value_type);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet(binding_name, is_mut, coerced_value, value_type, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [binding_name, annotation, value_expression, source_span] = stmtletconst; return [&]() -> std::tuple<> { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation(annotation);
  std::shared_ptr<semantic_ir::SExpr> typed_value = transform_expr(value_expression, current_context);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  current_env.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SExpr> coerced_value = coerce_expr_to_type(typed_value, value_type);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst(binding_name, coerced_value, value_type, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, source_span] = stmtexpr; return [&]() -> std::tuple<> { 
  std::shared_ptr<semantic_ir::SExpr> typed_expr = transform_expr(expression, current_context);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr(typed_expr, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [return_expression, source_span] = stmtreturn; return [&]() -> std::tuple<> { 
  std::shared_ptr<semantic_ir::SExpr> typed_return = transform_expr(return_expression, current_context);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtReturn(typed_return, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [source_span] = stmtbreak; return [&]() -> std::tuple<> { 
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtBreak(source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [source_span] = stmtcontinue; return [&]() -> std::tuple<> { 
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtContinue(source_span)));
  return std::make_tuple();
 }(); }
}, (*statements[statement_index])._);
statement_index = statement_index + 1;
}
}
return transform::TransformStmtsResult{typed_statements, current_env};
}

} // namespace transform

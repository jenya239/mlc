#include "names.hpp"

#include "ast.hpp"

namespace names {

using namespace ast;
using namespace ast_tokens;

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_empty() noexcept;

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
bool found = false;
int index = 0;
while (index < scope.size() && !found){
{
if (scope[index] == name){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept{
mlc::Array<mlc::String> accumulator = {};
return collect_pattern_bindings(pattern, accumulator);
}

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [name, _w0] = _v_patident; return [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(name);
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [_w0, sub_patterns, _w1] = _v_patctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatRecord>((*pattern))) { auto _v_patrecord = std::get<ast::PatRecord>((*pattern)); auto [_w0, field_patterns, _w1] = _v_patrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < field_patterns.size()){
{
accumulator = collect_pattern_bindings(field_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatTuple>((*pattern))) { auto _v_pattuple = std::get<ast::PatTuple>((*pattern)); auto [sub_patterns, _w0] = _v_pattuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatArray>((*pattern))) { auto _v_patarray = std::get<ast::PatArray>((*pattern)); auto [subs, rest, _w0] = _v_patarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < subs.size()){
{
accumulator = collect_pattern_bindings(subs[index], accumulator);
index = index + 1;
}
}
  return rest != mlc::String("") && rest != mlc::String("_") ? [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(rest);
  return accumulator;
 }() : accumulator;
 }(); } if (std::holds_alternative<ast::PatOr>((*pattern))) { auto _v_pator = std::get<ast::PatOr>((*pattern)); auto [alts, _w0] = _v_pator; return alts.size() > 0 ? collect_pattern_bindings(alts[0], accumulator) : accumulator; } return accumulator; }();
}

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
if (!scope_contains(locals, name) && !globals.has(name)){
{
collected.push_back(ast::diagnostic_error(mlc::String("undefined: ") + name, source_span));
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(inner, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = check_names_expr(function, locals, globals);
int index = 0;
while (index < call_arguments.size()){
{
collected = ast::diagnostics_append(collected, check_names_expr(call_arguments[index], locals, globals));
index = index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = check_names_expr(object, locals, globals);
int index = 0;
while (index < method_arguments.size()){
{
collected = ast::diagnostics_append(collected, check_names_expr(method_arguments[index], locals, globals));
index = index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(object, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(check_names_expr(object, locals, globals), check_names_expr(index_expression, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(ast::diagnostics_append(check_names_expr(condition, locals, globals), check_names_expr(then_expression, locals, globals)), check_names_expr(else_expression, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
names::NameCheckResult statement_result = check_names_statements(statements, locals, globals);
return NameCheckResult_append_expression_diagnostics(statement_result, check_names_expr(result, statement_result.scope, globals));
}

mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
names::NameCheckResult statement_check = check_names_statements(statements, locals, globals);
return ast::diagnostics_append(check_names_expr(condition, locals, globals), statement_check.diagnostics);
}

mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> inner_scope = locals;
inner_scope.push_back(variable_name);
names::NameCheckResult statement_check = check_names_statements(statements, inner_scope, globals);
return ast::diagnostics_append(check_names_expr(iterator, locals, globals), statement_check.diagnostics);
}

mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = check_names_expr(subject, locals, globals);
int arm_index = 0;
while (arm_index < arms.size()){
{
mlc::Array<mlc::String> bindings = pattern_bindings(arms[arm_index]->pat);
mlc::Array<mlc::String> arm_scope = locals;
int binding_index = 0;
while (binding_index < bindings.size()){
{
arm_scope.push_back(bindings[binding_index]);
binding_index = binding_index + 1;
}
}
collected = ast::diagnostics_append(collected, check_names_expr(arms[arm_index]->body, arm_scope, globals));
arm_index = arm_index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
int index = 0;
while (index < field_values.size()){
{
collected = ast::diagnostics_append(collected, check_names_expr(field_values[index]->val, locals, globals));
index = index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = check_names_expr(base, locals, globals);
int index = 0;
while (index < field_values.size()){
{
collected = ast::diagnostics_append(collected, check_names_expr(field_values[index]->val, locals, globals));
index = index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
int index = 0;
while (index < elements.size()){
{
collected = ast::diagnostics_append(collected, check_names_expr(elements[index], locals, globals));
index = index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(inner, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> lambda_locals = {};
int local_index = 0;
while (local_index < locals.size()){
{
lambda_locals.push_back(locals[local_index]);
local_index = local_index + 1;
}
}
int parameter_index = 0;
while (parameter_index < parameter_names.size()){
{
lambda_locals.push_back(parameter_names[parameter_index]);
parameter_index = parameter_index + 1;
}
}
return check_names_expr(body, lambda_locals, globals);
}

mlc::Array<ast::Diagnostic> check_names_empty() noexcept{
mlc::Array<ast::Diagnostic> empty_list = {};
return empty_list;
}

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, source_span] = _v_exprident; return check_names_identifier(name, locals, globals, source_span); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [_w0, left, right, _w1] = _v_exprbin; return check_names_binary_expression(left, right, locals, globals); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner, _w1] = _v_exprun; return check_names_unary_expression(inner, locals, globals); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, _w0] = _v_exprcall; return check_names_call_expression(function, call_arguments, locals, globals); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, _w0, method_arguments, _w1] = _v_exprmethod; return check_names_method_expression(object, method_arguments, locals, globals); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return check_names_field_expression(object, locals, globals); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, _w0] = _v_exprindex; return check_names_index_expression(object, index_expression, locals, globals); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, _w0] = _v_exprif; return check_names_conditional_expression(condition, then_expression, else_expression, locals, globals); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return check_names_block_expression(statements, result, locals, globals); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return check_names_while_expression(condition, statements, locals, globals); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable, iterator, statements, _w0] = _v_exprfor; return check_names_for_expression(variable, iterator, statements, locals, globals); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, _w0] = _v_exprmatch; return check_names_match_expression(subject, arms, locals, globals); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, field_values, _w1] = _v_exprrecord; return check_names_record_expression(field_values, locals, globals); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, field_values, _w1] = _v_exprrecordupdate; return check_names_record_update_expression(base, field_values, locals, globals); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return check_names_array_expression(elements, locals, globals); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return check_names_array_expression(elements, locals, globals); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, _w0] = _v_exprquestion; return check_names_question_expression(inner, locals, globals); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return check_names_lambda_expression(parameter_names, body, locals, globals); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner, _w1] = _v_exprnamedarg; return check_names_expr(inner, locals, globals); } return check_names_empty(); }();}

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{return ast::diagnostics_append(self.diagnostics, expression_diagnostics);}

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> collected_diagnostics = {};
mlc::Array<mlc::String> scope = locals;
int index = 0;
while (index < statements.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [name, _w0, _w1, value, _w2] = stmtlet; return [&]() -> std::tuple<> { 
  collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
  scope.push_back(name);
  return std::make_tuple();
 }(); },
  [&](const StmtLetPat& stmtletpat) -> std::tuple<> { auto [pattern, _w0, _w1, value, has_else, else_body, _w2] = stmtletpat; return [&]() -> std::tuple<> { 
  collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
  mlc::Array<mlc::String> pnames = pattern_bindings(pattern);
  int j = 0;
  while (j < pnames.size()){
{
scope.push_back(pnames[j]);
j = j + 1;
}
}
  if (has_else){
{
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(else_body, scope, globals));
}
}
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [name, _w0, value, _w1] = stmtletconst; return [&]() -> std::tuple<> { 
  collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
  scope.push_back(name);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, _w0] = stmtexpr; return [&]() -> std::tuple<> { 
  collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(expression, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [expression, _w0] = stmtreturn; return [&]() -> std::tuple<> { 
  collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(expression, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [_w0] = stmtbreak; return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [_w0] = stmtcontinue; return std::make_tuple(); }
}, (*statements[index])._);
index = index + 1;
}
}
return names::NameCheckResult{collected_diagnostics, scope};
}

} // namespace names

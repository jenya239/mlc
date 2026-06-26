#include "verify_ast.hpp"

#include "ast.hpp"

namespace verify_ast {

using namespace ast;
using namespace ast_tokens;

mlc::Array<mlc::String> verify_ast_empty_name_errors(mlc::String name, mlc::String message) noexcept;

mlc::Array<mlc::String> verify_ast_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept;

mlc::Array<mlc::String> verify_ast_span_errors(mlc::String label, ast::Span span) noexcept;

mlc::Array<mlc::String> verify_ast_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;

mlc::Array<mlc::String> verify_ast_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> verify_ast_record_lit_part(mlc::Array<mlc::String> errors, ast::RecordLitPart part) noexcept;

mlc::Array<mlc::String> verify_ast_expression(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<mlc::String> verify_ast_statement(std::shared_ptr<ast::Stmt> statement) noexcept;

mlc::Array<mlc::String> verify_ast_parameter(std::shared_ptr<ast::Param> parameter) noexcept;

mlc::Array<mlc::String> verify_ast_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<ast::Decl>> members) noexcept;

mlc::Array<mlc::String> verify_ast_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> verify_ast_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> verify_ast_program(ast::Program program) noexcept;

mlc::Array<mlc::String> verify_ast_empty_name_errors(mlc::String name, mlc::String message) noexcept{return name == mlc::String("") ? mlc::Array<mlc::String>{message} : mlc::Array<mlc::String>{};}

mlc::Array<mlc::String> verify_ast_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept{return condition ? errors : mlc::Array<mlc::String>{};}

mlc::Array<mlc::String> verify_ast_span_errors(mlc::String label, ast::Span span) noexcept{
return span.line < 0 ? mlc::Array<mlc::String>{label + mlc::String(": negative source line")} : mlc::Array<mlc::String>{};
}

mlc::Array<mlc::String> verify_ast_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
mlc::Array<mlc::String> combined = accumulator;
int index = 0;
while (index < more.size()){
{
combined.push_back(more[index]);
index = index + 1;
}
}
return combined;
}

mlc::Array<mlc::String> verify_ast_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept{
mlc::Array<mlc::String> errors = verify_ast_span_errors(mlc::String("pattern"), ast::pattern_span(pattern));
errors = verify_ast_append_errors(errors, {});
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, arguments, _w1] = _v_patternctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_pattern(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [_w0, fields, _w1] = _v_patternrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < fields.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_pattern(fields[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [arguments, _w0] = _v_patterntuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_pattern(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [elements, _w0, _w1] = _v_patternarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < elements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_pattern(elements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < alternatives.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_pattern(alternatives[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [name, _w0] = _v_patternident; return name == mlc::String("") ? verify_ast_append_errors(errors, mlc::Array<mlc::String>{mlc::String("pattern: empty identifier")}) : errors; } return errors; }();
}

mlc::Array<mlc::String> verify_ast_record_lit_part(mlc::Array<mlc::String> errors, ast::RecordLitPart part) noexcept{
return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<mlc::String> { auto [fields] = recordlitfields; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> field_errors = errors;
  int field_index = 0;
  while (field_index < fields.size()){
{
field_errors = verify_ast_append_errors(field_errors, verify_ast_expression(fields[field_index]->value));
field_index = field_index + 1;
}
}
  return field_errors;
 }(); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<mlc::String> { auto [spread_expression] = recordlitspread; return verify_ast_append_errors(errors, verify_ast_expression(spread_expression)); }
}, part);
}

mlc::Array<mlc::String> verify_ast_expression(std::shared_ptr<ast::Expr> expression) noexcept{
mlc::Array<mlc::String> errors = verify_ast_span_errors(mlc::String("expression"), ast::expr_span(expression));
errors = verify_ast_append_errors(errors, {});
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return name == mlc::String("") ? verify_ast_append_errors(errors, mlc::Array<mlc::String>{mlc::String("expression: empty identifier")}) : errors; } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [_w0, left, right, _w1] = _v_exprbin; return verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(left)), verify_ast_expression(right)); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, operand, _w1] = _v_exprun; return verify_ast_append_errors(errors, verify_ast_expression(operand)); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [callee, arguments, _w0] = _v_exprcall; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(callee));
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [receiver, _w0, arguments, _w1] = _v_exprmethod; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(receiver));
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [receiver, _w0, _w1] = _v_exprfield; return verify_ast_append_errors(errors, verify_ast_expression(receiver)); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [receiver, index_expression, _w0] = _v_exprindex; return verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(receiver)), verify_ast_expression(index_expression)); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_branch, else_branch, _w0] = _v_exprif; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(condition)), verify_ast_expression(then_branch)), verify_ast_expression(else_branch)); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(result));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(condition));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprSpawn>((*expression)._)) { auto _v_exprspawn = std::get<ast::ExprSpawn>((*expression)._); auto [statements, _w0] = _v_exprspawn; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < statements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [_w0, iterable, statements, _w1] = _v_exprfor; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(iterable));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, _w0] = _v_exprmatch; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(subject));
  int index = 0;
  while (index < arms.size()){
{
std::shared_ptr<ast::MatchArm> arm = arms[index];
errors = verify_ast_append_errors(errors, verify_ast_pattern(arm->pattern));
if (arm->has_guard){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(arm->when_condition));
}
}
errors = verify_ast_append_errors(errors, verify_ast_expression(arm->body));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, parts, _w1] = _v_exprrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < parts.size()){
{
errors = verify_ast_record_lit_part(errors, parts[index]);
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, fields, _w1] = _v_exprrecordupdate; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(base));
  int index = 0;
  while (index < fields.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(fields[index]->value));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < elements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(elements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < elements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_expression(elements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, _w0] = _v_exprquestion; return verify_ast_append_errors(errors, verify_ast_expression(inner)); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [_w0, body, _w1] = _v_exprlambda; return verify_ast_append_errors(errors, verify_ast_expression(body)); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, value, _w1] = _v_exprnamedarg; return verify_ast_append_errors(errors, verify_ast_expression(value)); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [subject, _w0, statements, _w1] = _v_exprwith; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_ast_append_errors(errors, verify_ast_expression(subject));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } return errors; }();
}

mlc::Array<mlc::String> verify_ast_statement(std::shared_ptr<ast::Stmt> statement) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::StmtLet>((*statement)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*statement)._); auto [name, _w0, _w1, initializer, _w2] = _v_stmtlet; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)), verify_ast_empty_name_errors(name, mlc::String("statement: empty let name"))), verify_ast_expression(initializer)); } if (std::holds_alternative<ast::StmtLetPattern>((*statement)._)) { auto _v_stmtletpattern = std::get<ast::StmtLetPattern>((*statement)._); auto [pattern, _w0, _w1, initializer, has_else, else_branch, _w2] = _v_stmtletpattern; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)), verify_ast_pattern(pattern)), verify_ast_expression(initializer)), verify_ast_errors_if(has_else, verify_ast_expression(else_branch))); } if (std::holds_alternative<ast::StmtLetConst>((*statement)._)) { auto _v_stmtletconst = std::get<ast::StmtLetConst>((*statement)._); auto [name, _w0, initializer, _w1] = _v_stmtletconst; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)), verify_ast_empty_name_errors(name, mlc::String("statement: empty const name"))), verify_ast_expression(initializer)); } if (std::holds_alternative<ast::StmtExpr>((*statement)._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*statement)._); auto [expression, _w0] = _v_stmtexpr; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)), verify_ast_expression(expression)); } if (std::holds_alternative<ast::StmtReturn>((*statement)._)) { auto _v_stmtreturn = std::get<ast::StmtReturn>((*statement)._); auto [expression, _w0] = _v_stmtreturn; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)), verify_ast_expression(expression)); } return verify_ast_span_errors(mlc::String("statement"), ast::stmt_span(statement)); }();}

mlc::Array<mlc::String> verify_ast_parameter(std::shared_ptr<ast::Param> parameter) noexcept{
return parameter->name == mlc::String("") ? mlc::Array<mlc::String>{mlc::String("parameter: empty name")} : mlc::Array<mlc::String>{};
}

mlc::Array<mlc::String> verify_ast_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<ast::Decl>> members) noexcept{
mlc::Array<mlc::String> accumulated = errors;
int index = 0;
while (index < members.size()){
{
accumulated = verify_ast_append_errors(accumulated, verify_ast_declaration(members[index]));
index = index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> verify_ast_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [name, _w0, _w1, parameters, _w2, body, _w3] = _v_declfn; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_empty_name_errors(name, mlc::String("declaration: empty function name")), verify_ast_parameters(parameters)), verify_ast_expression(body)); } if (std::holds_alternative<ast::DeclType>((*declaration))) { auto _v_decltype = std::get<ast::DeclType>((*declaration)); auto [name, _w0, _w1, _w2, span] = _v_decltype; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty type name"))); } if (std::holds_alternative<ast::DeclTypeAlias>((*declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*declaration)); auto [name, _w0, _w1, span] = _v_decltypealias; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty type alias name"))); } if (std::holds_alternative<ast::DeclTrait>((*declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*declaration)); auto [name, _w0, members, span] = _v_decltrait; return verify_ast_declaration_members(verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty trait name"))), members); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [type_name, _w0, members, span] = _v_declextend; return verify_ast_declaration_members(verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(type_name, mlc::String("declaration: empty extend type name"))), members); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner] = _v_declexported; return verify_ast_declaration(inner); } if (std::holds_alternative<ast::DeclAssocType>((*declaration))) { auto _v_declassoctype = std::get<ast::DeclAssocType>((*declaration)); auto [name, span] = _v_declassoctype; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty assoc type name"))); } if (std::holds_alternative<ast::DeclAssocBind>((*declaration))) { auto _v_declassocbind = std::get<ast::DeclAssocBind>((*declaration)); auto [name, _w0, span] = _v_declassocbind; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration"), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty assoc bind name"))); } return {}; }();}

mlc::Array<mlc::String> verify_ast_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < parameters.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_parameter(parameters[index]));
index = index + 1;
}
}
return errors;
}

mlc::Array<mlc::String> verify_ast_program(ast::Program program) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < program.decls.size()){
{
errors = verify_ast_append_errors(errors, verify_ast_declaration(program.decls[index]));
index = index + 1;
}
}
return errors;
}

} // namespace verify_ast

#define main mlc_user_main
#include "verify_ast.hpp"

#include "ast.hpp"

namespace verify_ast {

using namespace ast;
using namespace ast_tokens;

mlc::Array<mlc::String> verify_ast_empty_name_errors(mlc::String name, mlc::String message) noexcept{
  if ((name == mlc::String("", 0)))   {
    return mlc::Array<mlc::String>{message};
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_ast_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept{
  if (condition)   {
    return errors;
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_ast_span_errors(mlc::String label, ast::Span span) noexcept{
  if ((span.line < 0))   {
    return mlc::Array<mlc::String>{(label + mlc::String(": negative source line", 22))};
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_ast_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
  auto combined = accumulator;
  auto index = 0;
  while ((index < more.length()))   {
    combined.push_back(more[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return combined;
}
mlc::Array<mlc::String> verify_ast_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept{
  auto errors = verify_ast_span_errors(mlc::String("pattern", 7), ast::pattern_span(pattern));
  (errors = verify_ast_append_errors(errors, mlc::Array<mlc::String>{}));
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, arguments, __2] = patternCtor; return [&]() {
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(arguments[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::PatternRecord>((*__match_subject))) {
const ast::PatternRecord& patternRecord = std::get<ast::PatternRecord>((*__match_subject));
auto [__0, fields, __2] = patternRecord; return [&]() {
auto index = 0;
while ((index < fields.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(fields[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::PatternTuple>((*__match_subject))) {
const ast::PatternTuple& patternTuple = std::get<ast::PatternTuple>((*__match_subject));
auto [arguments, __1] = patternTuple; return [&]() {
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(arguments[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::PatternArray>((*__match_subject))) {
const ast::PatternArray& patternArray = std::get<ast::PatternArray>((*__match_subject));
auto [elements, __1, __2] = patternArray; return [&]() {
auto index = 0;
while ((index < elements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(elements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return [&]() {
auto index = 0;
while ((index < alternatives.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(alternatives[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [name, __1] = patternIdent; return ((name == mlc::String("", 0)) ? (verify_ast_append_errors(errors, mlc::Array<mlc::String>{mlc::String("pattern: empty identifier", 25)})) : (errors));
}
return errors;
std::abort();
}();
}
mlc::Array<mlc::String> verify_ast_record_lit_part(mlc::Array<mlc::String> errors, ast::RecordLitPart part) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [fields] = recordLitFields; return [&]() {
auto field_errors = errors;
auto field_index = 0;
while ((field_index < fields.length())) {
  (field_errors = verify_ast_append_errors(field_errors, verify_ast_expression(fields[field_index]->value)));
  (field_index = mlc::arith::checked_add(field_index, 1));
}
return field_errors;
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return verify_ast_append_errors(errors, verify_ast_expression(spread_expression)); }
}, part);
}
mlc::Array<mlc::String> verify_ast_expression(std::shared_ptr<ast::Expr> expression) noexcept{
  auto errors = verify_ast_span_errors(mlc::String("expression", 10), ast::expr_span(expression));
  (errors = verify_ast_append_errors(errors, mlc::Array<mlc::String>{}));
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return ((name == mlc::String("", 0)) ? (verify_ast_append_errors(errors, mlc::Array<mlc::String>{mlc::String("expression: empty identifier", 28)})) : (errors));
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [__0, left, right, __3] = exprBin; return verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(left)), verify_ast_expression(right));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, operand, __2] = exprUn; return verify_ast_append_errors(errors, verify_ast_expression(operand));
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, arguments, __2] = exprCall; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(callee)));
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_expression(arguments[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, __1, arguments, __3] = exprMethod; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(receiver)));
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_expression(arguments[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [receiver, __1, __2] = exprField; return verify_ast_append_errors(errors, verify_ast_expression(receiver));
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [receiver, index_expression, __2] = exprIndex; return verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(receiver)), verify_ast_expression(index_expression));
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_branch, else_branch, __3] = exprIf; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_append_errors(errors, verify_ast_expression(condition)), verify_ast_expression(then_branch)), verify_ast_expression(else_branch));
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result, __2] = exprBlock; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(result)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprWhile>((*__match_subject))) {
const ast::ExprWhile& exprWhile = std::get<ast::ExprWhile>((*__match_subject));
auto [condition, statements, __2] = exprWhile; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(condition)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [statements, __1] = exprSpawn; return [&]() {
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprFor>((*__match_subject))) {
const ast::ExprFor& exprFor = std::get<ast::ExprFor>((*__match_subject));
auto [__0, iterable, statements, __3] = exprFor; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(iterable)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [subject, arms, __2] = exprMatch; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(subject)));
auto index = 0;
while ((index < arms.length())) {
  auto arm = arms[index];
  (errors = verify_ast_append_errors(errors, verify_ast_pattern(arm->pattern)));
  if (arm->has_guard)   {
    (errors = verify_ast_append_errors(errors, verify_ast_expression(arm->when_condition)));
  }
  (errors = verify_ast_append_errors(errors, verify_ast_expression(arm->body)));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [__0, parts, __2] = exprRecord; return [&]() {
auto index = 0;
while ((index < parts.length())) {
  (errors = verify_ast_record_lit_part(errors, parts[index]));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprRecordUpdate>((*__match_subject))) {
const ast::ExprRecordUpdate& exprRecordUpdate = std::get<ast::ExprRecordUpdate>((*__match_subject));
auto [__0, base, fields, __3] = exprRecordUpdate; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(base)));
auto index = 0;
while ((index < fields.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_expression(fields[index]->value)));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return [&]() {
auto index = 0;
while ((index < elements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_expression(elements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return [&]() {
auto index = 0;
while ((index < elements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_expression(elements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner, __1] = exprQuestion; return verify_ast_append_errors(errors, verify_ast_expression(inner));
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [__0, body, __2] = exprLambda; return verify_ast_append_errors(errors, verify_ast_expression(body));
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, value, __2] = exprNamedArg; return verify_ast_append_errors(errors, verify_ast_expression(value));
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [subject, __1, statements, __3] = exprWith; return [&]() {
(errors = verify_ast_append_errors(errors, verify_ast_expression(subject)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_ast_append_errors(errors, verify_ast_statement(statements[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return errors;
}();
}
return errors;
std::abort();
}();
}
mlc::Array<mlc::String> verify_ast_statement(std::shared_ptr<ast::Stmt> statement) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, __1, __2, initializer, __4] = stmtLet; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement)), verify_ast_empty_name_errors(name, mlc::String("statement: empty let name", 25))), verify_ast_expression(initializer));
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [pattern, __1, __2, initializer, has_else, else_branch, __6] = stmtLetPattern; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement)), verify_ast_pattern(pattern)), verify_ast_expression(initializer)), verify_ast_errors_if(has_else, verify_ast_expression(else_branch)));
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [name, __1, initializer, __3] = stmtLetConst; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement)), verify_ast_empty_name_errors(name, mlc::String("statement: empty const name", 27))), verify_ast_expression(initializer));
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement)), verify_ast_expression(expression));
}
if (std::holds_alternative<ast::StmtReturn>((*__match_subject))) {
const ast::StmtReturn& stmtReturn = std::get<ast::StmtReturn>((*__match_subject));
auto [expression, __1] = stmtReturn; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement)), verify_ast_expression(expression));
}
return verify_ast_span_errors(mlc::String("statement", 9), ast::stmt_span(statement));
std::abort();
}();
}
mlc::Array<mlc::String> verify_ast_parameter(std::shared_ptr<ast::Param> parameter) noexcept{
  if ((parameter->name == mlc::String("", 0)))   {
    return mlc::Array<mlc::String>{mlc::String("parameter: empty name", 21)};
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_ast_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<ast::Decl>> members) noexcept{
  auto accumulated = errors;
  auto index = 0;
  while ((index < members.length()))   {
    (accumulated = verify_ast_append_errors(accumulated, verify_ast_declaration(members[index])));
    (index = mlc::arith::checked_add(index, 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> verify_ast_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, __1, __2, parameters, __4, body, __6] = declFn; return verify_ast_append_errors(verify_ast_append_errors(verify_ast_empty_name_errors(name, mlc::String("declaration: empty function name", 32)), verify_ast_parameters(parameters)), verify_ast_expression(body));
}
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [name, __1, __2, __3, span] = declType; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty type name", 28)));
}
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [name, __1, __2, span] = declTypeAlias; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty type alias name", 34)));
}
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [name, __1, members, span] = declTrait; return verify_ast_declaration_members(verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty trait name", 29))), members);
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [type_name, __1, members, span] = declExtend; return verify_ast_declaration_members(verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(type_name, mlc::String("declaration: empty extend type name", 35))), members);
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return verify_ast_declaration(inner);
}
if (std::holds_alternative<ast::DeclAssocType>((*__match_subject))) {
const ast::DeclAssocType& declAssocType = std::get<ast::DeclAssocType>((*__match_subject));
auto [name, span] = declAssocType; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty assoc type name", 34)));
}
if (std::holds_alternative<ast::DeclAssocBind>((*__match_subject))) {
const ast::DeclAssocBind& declAssocBind = std::get<ast::DeclAssocBind>((*__match_subject));
auto [name, __1, span] = declAssocBind; return verify_ast_append_errors(verify_ast_span_errors(mlc::String("declaration", 11), span), verify_ast_empty_name_errors(name, mlc::String("declaration: empty assoc bind name", 34)));
}
return mlc::Array<mlc::String>{};
std::abort();
}();
}
mlc::Array<mlc::String> verify_ast_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  auto errors = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < parameters.length()))   {
    (errors = verify_ast_append_errors(errors, verify_ast_parameter(parameters[index])));
    (index = mlc::arith::checked_add(index, 1));
  }
  return errors;
}
mlc::Array<mlc::String> verify_ast_program(ast::Program program) noexcept{
  auto errors = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < program.decls.length()))   {
    (errors = verify_ast_append_errors(errors, verify_ast_declaration(program.decls[index])));
    (index = mlc::arith::checked_add(index, 1));
  }
  return errors;
}

} // namespace verify_ast

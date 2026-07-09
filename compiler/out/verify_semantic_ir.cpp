#include "verify_semantic_ir.hpp"

#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"

namespace verify_semantic_ir {

using namespace registry;
using namespace ast;
using namespace semantic_ir;
using namespace ast_tokens;

mlc::Array<mlc::String> verify_semantic_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;

bool semantic_type_is_resolved(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<mlc::String> verify_semantic_empty_name_errors(mlc::String name, mlc::String message) noexcept;

mlc::Array<mlc::String> verify_semantic_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept;

mlc::Array<mlc::String> verify_semantic_span_errors(mlc::String label, ast::Span span) noexcept;

mlc::Array<mlc::String> verify_semantic_type_errors(mlc::String label, std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<mlc::String> verify_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::Array<mlc::String> verify_semantic_statement(std::shared_ptr<semantic_ir::SemanticStatement> semantic_statement) noexcept;

mlc::Array<mlc::String> verify_semantic_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> members) noexcept;

mlc::Array<mlc::String> verify_semantic_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::Array<mlc::String> verify_semantic_ir_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::Array<mlc::String> verify_semantic_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
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

bool semantic_type_is_resolved(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*type_value))) {  return false; } return true; }();}

mlc::Array<mlc::String> verify_semantic_empty_name_errors(mlc::String name, mlc::String message) noexcept{return name == mlc::String("") ? mlc::Array<mlc::String>{message} : mlc::Array<mlc::String>{};}

mlc::Array<mlc::String> verify_semantic_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept{return condition ? errors : mlc::Array<mlc::String>{};}

mlc::Array<mlc::String> verify_semantic_span_errors(mlc::String label, ast::Span span) noexcept{
return span.line < 0 ? mlc::Array<mlc::String>{label + mlc::String(": negative source line")} : mlc::Array<mlc::String>{};
}

mlc::Array<mlc::String> verify_semantic_type_errors(mlc::String label, std::shared_ptr<registry::Type> type_value) noexcept{
return semantic_type_is_resolved(type_value) ? mlc::Array<mlc::String>{} : mlc::Array<mlc::String>{label + mlc::String(": unresolved type")};
}

mlc::Array<mlc::String> verify_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
mlc::Array<mlc::String> errors = verify_semantic_span_errors(mlc::String("semantic expression"), semantic_ir::sexpr_span(expression));
errors = verify_semantic_append_errors(errors, verify_semantic_type_errors(mlc::String("semantic expression"), semantic_ir::sexpr_type(expression)));
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [_w0, left, right, _w1, _w2] = _v_semanticexpressionbin; return verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(left)), verify_semantic_expression(right)); } if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*expression)._)) { auto _v_semanticexpressionun = std::get<semantic_ir::SemanticExpressionUn>((*expression)._); auto [_w0, operand, _w1, _w2] = _v_semanticexpressionun; return verify_semantic_append_errors(errors, verify_semantic_expression(operand)); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [callee, arguments, _w0, _w1, _w2] = _v_semanticexpressioncall; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(callee));
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [receiver, _w0, arguments, _w1, _w2, _w3] = _v_semanticexpressionmethod; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(receiver));
  int index = 0;
  while (index < arguments.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(arguments[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [receiver, _w0, _w1, _w2] = _v_semanticexpressionfield; return verify_semantic_append_errors(errors, verify_semantic_expression(receiver)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [receiver, index_expression, _w0, _w1] = _v_semanticexpressionindex; return verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(receiver)), verify_semantic_expression(index_expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_branch, else_branch, _w0, _w1] = _v_semanticexpressionif; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(condition)), verify_semantic_expression(then_branch)), verify_semantic_expression(else_branch)); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(result));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, _w0, _w1] = _v_semanticexpressionwhile; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(condition));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [_w0, iterable, statements, _w1, _w2] = _v_semanticexpressionfor; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(iterable));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, _w0, _w1] = _v_semanticexpressionmatch; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(subject));
  int index = 0;
  while (index < arms.size()){
{
std::shared_ptr<semantic_ir::SemanticMatchArm> arm = arms[index];
if (arm->has_guard){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(arm->when_condition));
}
}
errors = verify_semantic_append_errors(errors, verify_semantic_expression(arm->body));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*expression)._)) { auto _v_semanticexpressionrecord = std::get<semantic_ir::SemanticExpressionRecord>((*expression)._); auto [_w0, fields, _w1, _w2] = _v_semanticexpressionrecord; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < fields.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(fields[index]->value));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._)) { auto _v_semanticexpressionrecordupdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._); auto [_w0, base, fields, _w1, _w2] = _v_semanticexpressionrecordupdate; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(base));
  int index = 0;
  while (index < fields.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(fields[index]->value));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, _w0, _w1] = _v_semanticexpressionarray; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < elements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(elements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*expression)._)) { auto _v_semanticexpressiontuple = std::get<semantic_ir::SemanticExpressionTuple>((*expression)._); auto [elements, _w0, _w1] = _v_semanticexpressiontuple; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < elements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_expression(elements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner, _w0, _w1] = _v_semanticexpressionquestion; return verify_semantic_append_errors(errors, verify_semantic_expression(inner)); } if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*expression)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*expression)._); auto [_w0, body, _w1, _w2] = _v_semanticexpressionlambda; return verify_semantic_append_errors(errors, verify_semantic_expression(body)); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [subject, _w0, statements, _w1, _w2] = _v_semanticexpressionwith; return [&]() -> mlc::Array<mlc::String> { 
  errors = verify_semantic_append_errors(errors, verify_semantic_expression(subject));
  int index = 0;
  while (index < statements.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return name == mlc::String("") ? verify_semantic_append_errors(errors, mlc::Array<mlc::String>{mlc::String("semantic expression: empty identifier")}) : errors; } return errors; }();
}

mlc::Array<mlc::String> verify_semantic_statement(std::shared_ptr<semantic_ir::SemanticStatement> semantic_statement) noexcept{return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> mlc::Array<mlc::String> { auto [name, _w0, initializer, type_value, span] = semanticstatementlet; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement"), span), verify_semantic_type_errors(mlc::String("semantic statement let"), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic statement: empty let name"))), verify_semantic_expression(initializer)); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> mlc::Array<mlc::String> { auto [_w0, _w1, initializer, type_value, has_else, else_branch, span] = semanticstatementletpattern; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement"), span), verify_semantic_type_errors(mlc::String("semantic statement let pattern"), type_value)), verify_semantic_expression(initializer)), verify_semantic_errors_if(has_else, verify_semantic_expression(else_branch))); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> mlc::Array<mlc::String> { auto [name, initializer, type_value, span] = semanticstatementletconst; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement"), span), verify_semantic_type_errors(mlc::String("semantic statement const"), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic statement: empty const name"))), verify_semantic_expression(initializer)); },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> mlc::Array<mlc::String> { auto [expression, span] = semanticstatementexpr; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement"), span), verify_semantic_expression(expression)); },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> mlc::Array<mlc::String> { auto [expression, span] = semanticstatementreturn; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement"), span), verify_semantic_expression(expression)); },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> mlc::Array<mlc::String> { auto [span] = semanticstatementbreak; return verify_semantic_span_errors(mlc::String("semantic statement"), span); },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> mlc::Array<mlc::String> { auto [span] = semanticstatementcontinue; return verify_semantic_span_errors(mlc::String("semantic statement"), span); }
}, (*semantic_statement)._);}

mlc::Array<mlc::String> verify_semantic_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> members) noexcept{
mlc::Array<mlc::String> accumulated = errors;
int index = 0;
while (index < members.size()){
{
accumulated = verify_semantic_append_errors(accumulated, verify_semantic_declaration(members[index]));
index = index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> verify_semantic_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*declaration))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*declaration)); auto [name, _w0, _w1, _w2, return_type, body, _w3, _w4] = _v_semanticdeclarationfn; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), semantic_ir::sdecl_span(declaration)), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty function name"))), verify_semantic_type_errors(mlc::String("semantic function return"), return_type)), verify_semantic_expression(body)); } if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*declaration))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*declaration)); auto [name, _w0, _w1, _w2, span] = _v_semanticdeclarationtype; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty type name"))); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTypeAlias>((*declaration))) { auto _v_semanticdeclarationtypealias = std::get<semantic_ir::SemanticDeclarationTypeAlias>((*declaration)); auto [name, _w0, _w1, span] = _v_semanticdeclarationtypealias; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty type alias name"))); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*declaration))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*declaration)); auto [name, _w0, members, span] = _v_semanticdeclarationtrait; return verify_semantic_declaration_members(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty trait name"))), members); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [type_name, _w0, members, span] = _v_semanticdeclarationextend; return verify_semantic_declaration_members(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), span), verify_semantic_empty_name_errors(type_name, mlc::String("semantic declaration: empty extend type name"))), members); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner] = _v_semanticdeclarationexported; return verify_semantic_declaration(inner); } if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*declaration))) { auto _v_semanticdeclarationassocbind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*declaration)); auto [name, type_value, span] = _v_semanticdeclarationassocbind; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration"), span), verify_semantic_type_errors(mlc::String("semantic assoc bind"), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty assoc bind name"))); } return verify_semantic_span_errors(mlc::String("semantic declaration"), semantic_ir::sdecl_span(declaration)); }();}

mlc::Array<mlc::String> verify_semantic_ir_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
mlc::Array<mlc::String> errors = {};
int item_index = 0;
while (item_index < items.size()){
{
semantic_ir::SemanticLoadItem load_item = items[item_index];
int decl_index = 0;
while (decl_index < load_item.decls.size()){
{
errors = verify_semantic_append_errors(errors, verify_semantic_declaration(load_item.decls[decl_index]));
decl_index = decl_index + 1;
}
}
item_index = item_index + 1;
}
}
return errors;
}

} // namespace verify_semantic_ir

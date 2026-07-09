#define main mlc_user_main
#include "verify_semantic_ir.hpp"

#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"

namespace verify_semantic_ir {

using namespace registry;
using namespace ast;
using namespace semantic_ir;
using namespace ast_tokens;

mlc::Array<mlc::String> verify_semantic_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
  auto combined = accumulator;
  auto index = 0;
  while ((index < more.length()))   {
    combined.push_back(more[index]);
    (index = (index + 1));
  }
  return combined;
}
bool semantic_type_is_resolved(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return false;
}
return true;
std::abort();
}();
}
mlc::Array<mlc::String> verify_semantic_empty_name_errors(mlc::String name, mlc::String message) noexcept{
  if ((name == mlc::String("", 0)))   {
    return mlc::Array<mlc::String>{message};
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_semantic_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept{
  if (condition)   {
    return errors;
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_semantic_span_errors(mlc::String label, ast::Span span) noexcept{
  if ((span.line < 0))   {
    return mlc::Array<mlc::String>{(label + mlc::String(": negative source line", 22))};
  } else   {
    return mlc::Array<mlc::String>{};
  }
}
mlc::Array<mlc::String> verify_semantic_type_errors(mlc::String label, std::shared_ptr<registry::Type> type_value) noexcept{
  if (semantic_type_is_resolved(type_value))   {
    return mlc::Array<mlc::String>{};
  } else   {
    return mlc::Array<mlc::String>{(label + mlc::String(": unresolved type", 17))};
  }
}
mlc::Array<mlc::String> verify_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  auto errors = verify_semantic_span_errors(mlc::String("semantic expression", 19), semantic_ir::sexpr_span(expression));
  (errors = verify_semantic_append_errors(errors, verify_semantic_type_errors(mlc::String("semantic expression", 19), semantic_ir::sexpr_type(expression))));
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [__0, left, right, __3, __4] = semanticExpressionBin; return verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(left)), verify_semantic_expression(right));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [__0, operand, __2, __3] = semanticExpressionUn; return verify_semantic_append_errors(errors, verify_semantic_expression(operand));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [callee, arguments, __2, __3, __4] = semanticExpressionCall; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(callee)));
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(arguments[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [receiver, __1, arguments, __3, __4, __5] = semanticExpressionMethod; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(receiver)));
auto index = 0;
while ((index < arguments.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(arguments[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [receiver, __1, __2, __3] = semanticExpressionField; return verify_semantic_append_errors(errors, verify_semantic_expression(receiver));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [receiver, index_expression, __2, __3] = semanticExpressionIndex; return verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(receiver)), verify_semantic_expression(index_expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_branch, else_branch, __3, __4] = semanticExpressionIf; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(errors, verify_semantic_expression(condition)), verify_semantic_expression(then_branch)), verify_semantic_expression(else_branch));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result, __2, __3] = semanticExpressionBlock; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(result)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*__match_subject))) {
const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile = std::get<semantic_ir::SemanticExpressionWhile>((*__match_subject));
auto [condition, statements, __2, __3] = semanticExpressionWhile; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(condition)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*__match_subject))) {
const semantic_ir::SemanticExpressionFor& semanticExpressionFor = std::get<semantic_ir::SemanticExpressionFor>((*__match_subject));
auto [__0, iterable, statements, __3, __4] = semanticExpressionFor; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(iterable)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*__match_subject))) {
const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch = std::get<semantic_ir::SemanticExpressionMatch>((*__match_subject));
auto [subject, arms, __2, __3] = semanticExpressionMatch; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(subject)));
auto index = 0;
while ((index < arms.length())) {
  auto arm = arms[index];
  if (arm->has_guard)   {
    (errors = verify_semantic_append_errors(errors, verify_semantic_expression(arm->when_condition)));
  }
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(arm->body)));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [__0, fields, __2, __3] = semanticExpressionRecord; return [&]() {
auto index = 0;
while ((index < fields.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(fields[index]->value)));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject));
auto [__0, base, fields, __3, __4] = semanticExpressionRecordUpdate; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(base)));
auto index = 0;
while ((index < fields.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(fields[index]->value)));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionArray; return [&]() {
auto index = 0;
while ((index < elements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(elements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionTuple; return [&]() {
auto index = 0;
while ((index < elements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_expression(elements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner, __1, __2] = semanticExpressionQuestion; return verify_semantic_append_errors(errors, verify_semantic_expression(inner));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [__0, body, __2, __3] = semanticExpressionLambda; return verify_semantic_append_errors(errors, verify_semantic_expression(body));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*__match_subject))) {
const semantic_ir::SemanticExpressionWith& semanticExpressionWith = std::get<semantic_ir::SemanticExpressionWith>((*__match_subject));
auto [subject, __1, statements, __3, __4] = semanticExpressionWith; return [&]() {
(errors = verify_semantic_append_errors(errors, verify_semantic_expression(subject)));
auto index = 0;
while ((index < statements.length())) {
  (errors = verify_semantic_append_errors(errors, verify_semantic_statement(statements[index])));
  (index = (index + 1));
}
return errors;
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return ((name == mlc::String("", 0)) ? (verify_semantic_append_errors(errors, mlc::Array<mlc::String>{mlc::String("semantic expression: empty identifier", 37)})) : (errors));
}
return errors;
std::abort();
}();
}
mlc::Array<mlc::String> verify_semantic_statement(std::shared_ptr<semantic_ir::SemanticStatement> semantic_statement) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) { auto [name, __1, initializer, type_value, span] = semanticStatementLet; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement", 18), span), verify_semantic_type_errors(mlc::String("semantic statement let", 22), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic statement: empty let name", 34))), verify_semantic_expression(initializer)); },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) { auto [__0, __1, initializer, type_value, has_else, else_branch, span] = semanticStatementLetPattern; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement", 18), span), verify_semantic_type_errors(mlc::String("semantic statement let pattern", 30), type_value)), verify_semantic_expression(initializer)), verify_semantic_errors_if(has_else, verify_semantic_expression(else_branch))); },
[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) { auto [name, initializer, type_value, span] = semanticStatementLetConst; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement", 18), span), verify_semantic_type_errors(mlc::String("semantic statement const", 24), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic statement: empty const name", 36))), verify_semantic_expression(initializer)); },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) { auto [expression, span] = semanticStatementExpr; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement", 18), span), verify_semantic_expression(expression)); },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) { auto [expression, span] = semanticStatementReturn; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic statement", 18), span), verify_semantic_expression(expression)); },
[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) { auto [span] = semanticStatementBreak; return verify_semantic_span_errors(mlc::String("semantic statement", 18), span); },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) { auto [span] = semanticStatementContinue; return verify_semantic_span_errors(mlc::String("semantic statement", 18), span); }
}, (*semantic_statement));
}
mlc::Array<mlc::String> verify_semantic_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> members) noexcept{
  auto accumulated = errors;
  auto index = 0;
  while ((index < members.length()))   {
    (accumulated = verify_semantic_append_errors(accumulated, verify_semantic_declaration(members[index])));
    (index = (index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> verify_semantic_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [name, __1, __2, __3, return_type, body, __6, __7] = semanticDeclarationFn; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), semantic_ir::sdecl_span(declaration)), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty function name", 41))), verify_semantic_type_errors(mlc::String("semantic function return", 24), return_type)), verify_semantic_expression(body));
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*__match_subject))) {
const semantic_ir::SemanticDeclarationType& semanticDeclarationType = std::get<semantic_ir::SemanticDeclarationType>((*__match_subject));
auto [name, __1, __2, __3, span] = semanticDeclarationType; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty type name", 37)));
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationTypeAlias>((*__match_subject))) {
const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias = std::get<semantic_ir::SemanticDeclarationTypeAlias>((*__match_subject));
auto [name, __1, __2, span] = semanticDeclarationTypeAlias; return verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty type alias name", 43)));
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*__match_subject))) {
const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait = std::get<semantic_ir::SemanticDeclarationTrait>((*__match_subject));
auto [name, __1, members, span] = semanticDeclarationTrait; return verify_semantic_declaration_members(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), span), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty trait name", 38))), members);
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, __1, members, span] = semanticDeclarationExtend; return verify_semantic_declaration_members(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), span), verify_semantic_empty_name_errors(type_name, mlc::String("semantic declaration: empty extend type name", 44))), members);
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner] = semanticDeclarationExported; return verify_semantic_declaration(inner);
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject))) {
const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject));
auto [name, type_value, span] = semanticDeclarationAssocBind; return verify_semantic_append_errors(verify_semantic_append_errors(verify_semantic_span_errors(mlc::String("semantic declaration", 20), span), verify_semantic_type_errors(mlc::String("semantic assoc bind", 19), type_value)), verify_semantic_empty_name_errors(name, mlc::String("semantic declaration: empty assoc bind name", 43)));
}
return verify_semantic_span_errors(mlc::String("semantic declaration", 20), semantic_ir::sdecl_span(declaration));
std::abort();
}();
}
mlc::Array<mlc::String> verify_semantic_ir_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  auto errors = mlc::Array<mlc::String>{};
  auto item_index = 0;
  while ((item_index < items.length()))   {
    auto load_item = items[item_index];
    auto decl_index = 0;
    while ((decl_index < load_item.decls.length()))     {
      (errors = verify_semantic_append_errors(errors, verify_semantic_declaration(load_item.decls[decl_index])));
      (decl_index = (decl_index + 1));
    }
    (item_index = (item_index + 1));
  }
  return errors;
}

} // namespace verify_semantic_ir

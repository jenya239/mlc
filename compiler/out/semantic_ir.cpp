#define main mlc_user_main
#include "semantic_ir.hpp"

#include "registry.hpp"
#include "ast.hpp"

namespace semantic_ir {

using namespace registry;
using namespace ast;
using namespace ast_tokens;

FnEscapeInfo empty_fn_escape_info() noexcept{
  auto empty_names = mlc::Array<mlc::String>{};
  return FnEscapeInfo{empty_names, {}};
}
std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const SemanticExpressionInt& semanticExpressionInt) { auto [__0, type_value, __2] = semanticExpressionInt; return type_value; },
[&](const SemanticExpressionStr& semanticExpressionStr) { auto [__0, type_value, __2] = semanticExpressionStr; return type_value; },
[&](const SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, type_value, __2] = semanticExpressionFloat; return type_value; },
[&](const SemanticExpressionI64& semanticExpressionI64) { auto [__0, type_value, __2] = semanticExpressionI64; return type_value; },
[&](const SemanticExpressionU8& semanticExpressionU8) { auto [__0, type_value, __2] = semanticExpressionU8; return type_value; },
[&](const SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, type_value, __2] = semanticExpressionUsize; return type_value; },
[&](const SemanticExpressionChar& semanticExpressionChar) { auto [__0, type_value, __2] = semanticExpressionChar; return type_value; },
[&](const SemanticExpressionBool& semanticExpressionBool) { auto [__0, type_value, __2] = semanticExpressionBool; return type_value; },
[&](const SemanticExpressionUnit& semanticExpressionUnit) { auto [type_value, __1] = semanticExpressionUnit; return type_value; },
[&](const SemanticExpressionExtern& semanticExpressionExtern) { auto [type_value, __1] = semanticExpressionExtern; return type_value; },
[&](const SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, type_value, __2] = semanticExpressionIdent; return type_value; },
[&](const SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, type_value, __4] = semanticExpressionBin; return type_value; },
[&](const SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, type_value, __3] = semanticExpressionUn; return type_value; },
[&](const SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, type_value, __4] = semanticExpressionCall; return type_value; },
[&](const SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, type_value, __5] = semanticExpressionMethod; return type_value; },
[&](const SemanticExpressionField& semanticExpressionField) { auto [__0, __1, type_value, __3] = semanticExpressionField; return type_value; },
[&](const SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, type_value, __3] = semanticExpressionIndex; return type_value; },
[&](const SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, type_value, __4] = semanticExpressionIf; return type_value; },
[&](const SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, type_value, __3] = semanticExpressionBlock; return type_value; },
[&](const SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, type_value, __3] = semanticExpressionWhile; return type_value; },
[&](const SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, type_value, __4] = semanticExpressionFor; return type_value; },
[&](const SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, type_value, __3] = semanticExpressionMatch; return type_value; },
[&](const SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, type_value, __3] = semanticExpressionRecord; return type_value; },
[&](const SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, type_value, __4] = semanticExpressionRecordUpdate; return type_value; },
[&](const SemanticExpressionArray& semanticExpressionArray) { auto [__0, type_value, __2] = semanticExpressionArray; return type_value; },
[&](const SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, type_value, __2] = semanticExpressionTuple; return type_value; },
[&](const SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, type_value, __2] = semanticExpressionQuestion; return type_value; },
[&](const SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, type_value, __3] = semanticExpressionLambda; return type_value; },
[&](const SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, type_value, __4] = semanticExpressionWith; return type_value; }
}, (*expression));
}
ast::Span sexpr_span(std::shared_ptr<SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const SemanticExpressionInt& semanticExpressionInt) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionInt; return span_value; },
[&](const SemanticExpressionStr& semanticExpressionStr) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionStr; return span_value; },
[&](const SemanticExpressionFloat& semanticExpressionFloat) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionFloat; return span_value; },
[&](const SemanticExpressionI64& semanticExpressionI64) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionI64; return span_value; },
[&](const SemanticExpressionU8& semanticExpressionU8) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionU8; return span_value; },
[&](const SemanticExpressionUsize& semanticExpressionUsize) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionUsize; return span_value; },
[&](const SemanticExpressionChar& semanticExpressionChar) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionChar; return span_value; },
[&](const SemanticExpressionBool& semanticExpressionBool) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionBool; return span_value; },
[&](const SemanticExpressionUnit& semanticExpressionUnit) -> ast::Span { auto [__0, span_value] = semanticExpressionUnit; return span_value; },
[&](const SemanticExpressionExtern& semanticExpressionExtern) -> ast::Span { auto [__0, span_value] = semanticExpressionExtern; return span_value; },
[&](const SemanticExpressionIdent& semanticExpressionIdent) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionIdent; return span_value; },
[&](const SemanticExpressionBin& semanticExpressionBin) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionBin; return span_value; },
[&](const SemanticExpressionUn& semanticExpressionUn) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionUn; return span_value; },
[&](const SemanticExpressionCall& semanticExpressionCall) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionCall; return span_value; },
[&](const SemanticExpressionMethod& semanticExpressionMethod) -> ast::Span { auto [__0, __1, __2, __3, __4, span_value] = semanticExpressionMethod; return span_value; },
[&](const SemanticExpressionField& semanticExpressionField) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionField; return span_value; },
[&](const SemanticExpressionIndex& semanticExpressionIndex) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionIndex; return span_value; },
[&](const SemanticExpressionIf& semanticExpressionIf) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionIf; return span_value; },
[&](const SemanticExpressionBlock& semanticExpressionBlock) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionBlock; return span_value; },
[&](const SemanticExpressionWhile& semanticExpressionWhile) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionWhile; return span_value; },
[&](const SemanticExpressionFor& semanticExpressionFor) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionFor; return span_value; },
[&](const SemanticExpressionMatch& semanticExpressionMatch) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionMatch; return span_value; },
[&](const SemanticExpressionRecord& semanticExpressionRecord) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionRecord; return span_value; },
[&](const SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionRecordUpdate; return span_value; },
[&](const SemanticExpressionArray& semanticExpressionArray) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionArray; return span_value; },
[&](const SemanticExpressionTuple& semanticExpressionTuple) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionTuple; return span_value; },
[&](const SemanticExpressionQuestion& semanticExpressionQuestion) -> ast::Span { auto [__0, __1, span_value] = semanticExpressionQuestion; return span_value; },
[&](const SemanticExpressionLambda& semanticExpressionLambda) -> ast::Span { auto [__0, __1, __2, span_value] = semanticExpressionLambda; return span_value; },
[&](const SemanticExpressionWith& semanticExpressionWith) -> ast::Span { auto [__0, __1, __2, __3, span_value] = semanticExpressionWith; return span_value; }
}, (*expression));
}
std::shared_ptr<SemanticDeclaration> sdecl_inner(std::shared_ptr<SemanticDeclaration> declaration) noexcept{
  return [&]() -> std::shared_ptr<SemanticDeclaration> {
auto __match_subject = declaration;
if (std::holds_alternative<SemanticDeclarationExported>((*__match_subject))) {
const SemanticDeclarationExported& semanticDeclarationExported = std::get<SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return inner_declaration;
}
return declaration;
std::abort();
}();
}
ast::Span sdecl_span(std::shared_ptr<SemanticDeclaration> declaration) noexcept{
  return [&]() -> ast::Span {
auto __match_subject = declaration;
if (std::holds_alternative<SemanticDeclarationFn>((*__match_subject))) {
const SemanticDeclarationFn& semanticDeclarationFn = std::get<SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, __5, __6, __7, span] = semanticDeclarationFn; return span;
}
if (std::holds_alternative<SemanticDeclarationType>((*__match_subject))) {
const SemanticDeclarationType& semanticDeclarationType = std::get<SemanticDeclarationType>((*__match_subject));
auto [__0, __1, __2, __3, span] = semanticDeclarationType; return span;
}
if (std::holds_alternative<SemanticDeclarationTypeAlias>((*__match_subject))) {
const SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias = std::get<SemanticDeclarationTypeAlias>((*__match_subject));
auto [__0, __1, __2, span] = semanticDeclarationTypeAlias; return span;
}
if (std::holds_alternative<SemanticDeclarationTrait>((*__match_subject))) {
const SemanticDeclarationTrait& semanticDeclarationTrait = std::get<SemanticDeclarationTrait>((*__match_subject));
auto [__0, __1, __2, span] = semanticDeclarationTrait; return span;
}
if (std::holds_alternative<SemanticDeclarationExtend>((*__match_subject))) {
const SemanticDeclarationExtend& semanticDeclarationExtend = std::get<SemanticDeclarationExtend>((*__match_subject));
auto [__0, __1, __2, span] = semanticDeclarationExtend; return span;
}
if (std::holds_alternative<SemanticDeclarationAssocBind>((*__match_subject))) {
const SemanticDeclarationAssocBind& semanticDeclarationAssocBind = std::get<SemanticDeclarationAssocBind>((*__match_subject));
auto [__0, __1, span] = semanticDeclarationAssocBind; return span;
}
return ast::span_unknown();
std::abort();
}();
}

} // namespace semantic_ir

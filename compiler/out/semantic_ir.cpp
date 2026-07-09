#include "semantic_ir.hpp"

#include "registry.hpp"
#include "ast.hpp"

namespace semantic_ir {

using namespace registry;
using namespace ast;
using namespace ast_tokens;

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> sdecl_inner(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

ast::Span sdecl_span(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionInt& semanticexpressionint) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionint; return type_value; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionstr; return type_value; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionfloat; return type_value; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressioni64; return type_value; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionu8; return type_value; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionusize; return type_value; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionchar; return type_value; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionbool; return type_value; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> std::shared_ptr<registry::Type> { auto [type_value, _w0] = semanticexpressionunit; return type_value; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> std::shared_ptr<registry::Type> { auto [type_value, _w0] = semanticexpressionextern; return type_value; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionident; return type_value; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressionbin; return type_value; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionun; return type_value; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressioncall; return type_value; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, _w3, type_value, _w4] = semanticexpressionmethod; return type_value; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionfield; return type_value; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionindex; return type_value; },
  [&](const SemanticExpressionIf& semanticexpressionif) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressionif; return type_value; },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionblock; return type_value; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionwhile; return type_value; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressionfor; return type_value; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionmatch; return type_value; },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionrecord; return type_value; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressionrecordupdate; return type_value; },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionarray; return type_value; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressiontuple; return type_value; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> std::shared_ptr<registry::Type> { auto [_w0, type_value, _w1] = semanticexpressionquestion; return type_value; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, type_value, _w2] = semanticexpressionlambda; return type_value; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, type_value, _w3] = semanticexpressionwith; return type_value; }
}, (*expression)._);}

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionInt& semanticexpressionint) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionint; return span_value; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionstr; return span_value; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionfloat; return span_value; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressioni64; return span_value; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionu8; return span_value; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionusize; return span_value; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionchar; return span_value; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionbool; return span_value; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> ast::Span { auto [_w0, span_value] = semanticexpressionunit; return span_value; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> ast::Span { auto [_w0, span_value] = semanticexpressionextern; return span_value; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionident; return span_value; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressionbin; return span_value; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionun; return span_value; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressioncall; return span_value; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> ast::Span { auto [_w0, _w1, _w2, _w3, _w4, span_value] = semanticexpressionmethod; return span_value; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionfield; return span_value; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionindex; return span_value; },
  [&](const SemanticExpressionIf& semanticexpressionif) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressionif; return span_value; },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionblock; return span_value; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionwhile; return span_value; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressionfor; return span_value; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionmatch; return span_value; },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionrecord; return span_value; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressionrecordupdate; return span_value; },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionarray; return span_value; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressiontuple; return span_value; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> ast::Span { auto [_w0, _w1, span_value] = semanticexpressionquestion; return span_value; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> ast::Span { auto [_w0, _w1, _w2, span_value] = semanticexpressionlambda; return span_value; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> ast::Span { auto [_w0, _w1, _w2, _w3, span_value] = semanticexpressionwith; return span_value; }
}, (*expression)._);}

std::shared_ptr<semantic_ir::SemanticDeclaration> sdecl_inner(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SemanticDeclaration> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner_declaration] = _v_semanticdeclarationexported; return inner_declaration; } return declaration; }();}

ast::Span sdecl_span(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> ast::Span { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*declaration))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*declaration)); auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6, span] = _v_semanticdeclarationfn; return span; } if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*declaration))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*declaration)); auto [_w0, _w1, _w2, _w3, span] = _v_semanticdeclarationtype; return span; } if (std::holds_alternative<semantic_ir::SemanticDeclarationTypeAlias>((*declaration))) { auto _v_semanticdeclarationtypealias = std::get<semantic_ir::SemanticDeclarationTypeAlias>((*declaration)); auto [_w0, _w1, _w2, span] = _v_semanticdeclarationtypealias; return span; } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*declaration))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*declaration)); auto [_w0, _w1, _w2, span] = _v_semanticdeclarationtrait; return span; } if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [_w0, _w1, _w2, span] = _v_semanticdeclarationextend; return span; } if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*declaration))) { auto _v_semanticdeclarationassocbind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*declaration)); auto [_w0, _w1, span] = _v_semanticdeclarationassocbind; return span; } return ast::span_unknown(); }();}

} // namespace semantic_ir

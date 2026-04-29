#include "semantic_ir.hpp"

#include "registry.hpp"
#include "ast.hpp"

namespace semantic_ir {

using namespace registry;
using namespace ast;
using namespace ast_tokens;

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SExpr> expression) noexcept;

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SExpr> expression) noexcept;

std::shared_ptr<semantic_ir::SDecl> sdecl_inner(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept;

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SExpr> expression) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprint; return t; },
  [&](const SExprStr& sexprstr) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprstr; return t; },
  [&](const SExprFloat& sexprfloat) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprfloat; return t; },
  [&](const SExprI64& sexpri64) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexpri64; return t; },
  [&](const SExprU8& sexpru8) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexpru8; return t; },
  [&](const SExprUsize& sexprusize) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprusize; return t; },
  [&](const SExprChar& sexprchar) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprchar; return t; },
  [&](const SExprBool& sexprbool) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprbool; return t; },
  [&](const SExprUnit& sexprunit) -> std::shared_ptr<registry::Type> { auto [t, _w0] = sexprunit; return t; },
  [&](const SExprExtern& sexprextern) -> std::shared_ptr<registry::Type> { auto [t, _w0] = sexprextern; return t; },
  [&](const SExprIdent& sexprident) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprident; return t; },
  [&](const SExprBin& sexprbin) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprbin; return t; },
  [&](const SExprUn& sexprun) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprun; return t; },
  [&](const SExprCall& sexprcall) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprcall; return t; },
  [&](const SExprMethod& sexprmethod) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprmethod; return t; },
  [&](const SExprField& sexprfield) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprfield; return t; },
  [&](const SExprIndex& sexprindex) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprindex; return t; },
  [&](const SExprIf& sexprif) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprif; return t; },
  [&](const SExprBlock& sexprblock) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprblock; return t; },
  [&](const SExprWhile& sexprwhile) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprwhile; return t; },
  [&](const SExprFor& sexprfor) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprfor; return t; },
  [&](const SExprMatch& sexprmatch) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprmatch; return t; },
  [&](const SExprRecord& sexprrecord) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprrecord; return t; },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprrecordupdate; return t; },
  [&](const SExprArray& sexprarray) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprarray; return t; },
  [&](const SExprTuple& sexprtuple) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprtuple; return t; },
  [&](const SExprQuestion& sexprquestion) -> std::shared_ptr<registry::Type> { auto [_w0, t, _w1] = sexprquestion; return t; },
  [&](const SExprLambda& sexprlambda) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, t, _w2] = sexprlambda; return t; },
  [&](const SExprWith& sexprwith) -> std::shared_ptr<registry::Type> { auto [_w0, _w1, _w2, t, _w3] = sexprwith; return t; }
}, (*expression)._);}

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SExpr> expression) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> ast::Span { auto [_w0, _w1, s] = sexprint; return s; },
  [&](const SExprStr& sexprstr) -> ast::Span { auto [_w0, _w1, s] = sexprstr; return s; },
  [&](const SExprFloat& sexprfloat) -> ast::Span { auto [_w0, _w1, s] = sexprfloat; return s; },
  [&](const SExprI64& sexpri64) -> ast::Span { auto [_w0, _w1, s] = sexpri64; return s; },
  [&](const SExprU8& sexpru8) -> ast::Span { auto [_w0, _w1, s] = sexpru8; return s; },
  [&](const SExprUsize& sexprusize) -> ast::Span { auto [_w0, _w1, s] = sexprusize; return s; },
  [&](const SExprChar& sexprchar) -> ast::Span { auto [_w0, _w1, s] = sexprchar; return s; },
  [&](const SExprBool& sexprbool) -> ast::Span { auto [_w0, _w1, s] = sexprbool; return s; },
  [&](const SExprUnit& sexprunit) -> ast::Span { auto [_w0, s] = sexprunit; return s; },
  [&](const SExprExtern& sexprextern) -> ast::Span { auto [_w0, s] = sexprextern; return s; },
  [&](const SExprIdent& sexprident) -> ast::Span { auto [_w0, _w1, s] = sexprident; return s; },
  [&](const SExprBin& sexprbin) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprbin; return s; },
  [&](const SExprUn& sexprun) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprun; return s; },
  [&](const SExprCall& sexprcall) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprcall; return s; },
  [&](const SExprMethod& sexprmethod) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprmethod; return s; },
  [&](const SExprField& sexprfield) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprfield; return s; },
  [&](const SExprIndex& sexprindex) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprindex; return s; },
  [&](const SExprIf& sexprif) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprif; return s; },
  [&](const SExprBlock& sexprblock) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprblock; return s; },
  [&](const SExprWhile& sexprwhile) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprwhile; return s; },
  [&](const SExprFor& sexprfor) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprfor; return s; },
  [&](const SExprMatch& sexprmatch) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprmatch; return s; },
  [&](const SExprRecord& sexprrecord) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprrecord; return s; },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprrecordupdate; return s; },
  [&](const SExprArray& sexprarray) -> ast::Span { auto [_w0, _w1, s] = sexprarray; return s; },
  [&](const SExprTuple& sexprtuple) -> ast::Span { auto [_w0, _w1, s] = sexprtuple; return s; },
  [&](const SExprQuestion& sexprquestion) -> ast::Span { auto [_w0, _w1, s] = sexprquestion; return s; },
  [&](const SExprLambda& sexprlambda) -> ast::Span { auto [_w0, _w1, _w2, s] = sexprlambda; return s; },
  [&](const SExprWith& sexprwith) -> ast::Span { auto [_w0, _w1, _w2, _w3, s] = sexprwith; return s; }
}, (*expression)._);}

std::shared_ptr<semantic_ir::SDecl> sdecl_inner(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SDecl> { if (std::holds_alternative<semantic_ir::SDeclExported>((*declaration))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*declaration)); auto [inner] = _v_sdeclexported; return inner; } return declaration; }();}

} // namespace semantic_ir

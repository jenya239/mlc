#include "types.hpp"

#include "preds.hpp"

namespace types {

using namespace preds;

preds::TypeResult parse_type(preds::Parser parser) noexcept;

preds::TypesResult parse_paren_types(preds::Parser parser) noexcept;

preds::TypeResult parse_base_type(preds::Parser parser) noexcept;

preds::TypesResult parse_type_args(preds::Parser parser) noexcept;

preds::TypeResult parse_type(preds::Parser parser) noexcept{
preds::TypeResult base = parse_base_type(parser);
return preds::TKind_is_arrow(preds::Parser_kind(base.parser)) ? [&]() -> preds::TypeResult { 
  preds::TypeResult ret = parse_type(preds::Parser_advance(base.parser));
  mlc::Array<std::shared_ptr<ast::TypeExpr>> params = mlc::Array<std::shared_ptr<ast::TypeExpr>>{base.type_expr};
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(params, ret.type_expr)), ret.parser};
 }() : base;
}

preds::TypesResult parse_paren_types(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
return preds::TKind_is_rparen(preds::Parser_kind(parser)) ? preds::TypesResult{types, preds::Parser_advance(parser)} : [&]() -> preds::TypesResult { 
  preds::TypeResult first = parse_type(parser);
  types.push_back(first.type_expr);
  preds::Parser state = first.parser;
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::TypeResult next = parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
  return preds::TypesResult{types, preds::TKind_is_rparen(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state};
 }();
}

preds::TypeResult parse_base_type(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_lparen(kind) ? [&]() -> preds::TypeResult { 
  preds::TypesResult paren_types = parse_paren_types(preds::Parser_advance(parser));
  return preds::TKind_is_arrow(preds::Parser_kind(paren_types.parser)) ? [&]() -> preds::TypeResult { 
  preds::TypeResult ret = parse_type(preds::Parser_advance(paren_types.parser));
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(paren_types.types, ret.type_expr)), ret.parser};
 }() : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), paren_types.parser};
 }() : preds::TKind_is_lbracket(kind) ? [&]() -> preds::TypeResult { 
  preds::TypeResult inner = parse_type(preds::Parser_advance(parser));
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyArray(inner.type_expr)), preds::Parser_advance(inner.parser)};
 }() : preds::TKind_is_ident(kind) ? [&]() -> preds::TypeResult { 
  mlc::String name = preds::TKind_ident(kind);
  return name == mlc::String("i32") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyI32{})), preds::Parser_advance(parser)} : name == mlc::String("string") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyString{})), preds::Parser_advance(parser)} : name == mlc::String("bool") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyBool{})), preds::Parser_advance(parser)} : name == mlc::String("unit") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(parser)} : name == mlc::String("ref") ? [&]() -> preds::TypeResult { 
  preds::Parser after_ref = preds::TKind_is_mut(preds::Parser_kind(preds::Parser_advance(parser))) ? preds::Parser_advance_by(parser, 2) : preds::Parser_advance(parser);
  preds::TypeResult inner = parse_type(after_ref);
  mlc::Array<std::shared_ptr<ast::TypeExpr>> targs = mlc::Array<std::shared_ptr<ast::TypeExpr>>{inner.type_expr};
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyGeneric(mlc::String("ref"), targs)), inner.parser};
 }() : [&]() -> preds::TypeResult { 
  preds::Parser after = preds::Parser_advance(parser);
  return preds::TKind_is_op(preds::Parser_kind(after)) && preds::TKind_op_val(preds::Parser_kind(after)) == mlc::String("<") ? [&]() -> preds::TypeResult { 
  preds::TypesResult targs = parse_type_args(preds::Parser_advance(after));
  std::shared_ptr<ast::TypeExpr> type_expr = name == mlc::String("Shared") && targs.types.size() == 1 ? std::make_shared<ast::TypeExpr>(ast::TyShared(targs.types[0])) : std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, targs.types));
  return preds::TypeResult{type_expr, targs.parser};
 }() : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyNamed(name)), after};
 }();
 }() : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance_by(parser, 2)};
}

preds::TypesResult parse_type_args(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
preds::TypeResult first = parse_type(parser);
types.push_back(first.type_expr);
preds::Parser state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::TypeResult next = parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
preds::Parser end_state = preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">") ? preds::Parser_advance(state) : state;
return preds::TypesResult{types, end_state};
}

} // namespace types

#include "types.hpp"

#include "preds.hpp"

namespace types {

using namespace preds;

preds::TypeResult parse_type(preds::Parser parser) noexcept;

preds::TypeResult parse_base_type(preds::Parser parser) noexcept;

preds::TypesResult parse_type_args(preds::Parser parser) noexcept;

preds::TypeResult parse_type(preds::Parser parser) noexcept{
preds::TypeResult base = types::parse_base_type(parser);
return preds::TKind_is_arrow(preds::Parser_kind(base.parser)) ? [&]() -> preds::TypeResult { 
  preds::TypeResult ret = types::parse_type(preds::Parser_advance(base.parser));
  mlc::Array<std::shared_ptr<ast::TypeExpr>> params = mlc::Array<std::shared_ptr<ast::TypeExpr>>{base.type_expr};
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(params, ret.type_expr)), ret.parser};
 }() : base;
}

preds::TypeResult parse_base_type(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_lbracket(kind) ? [&]() -> preds::TypeResult { 
  preds::TypeResult inner = types::parse_type(preds::Parser_advance(parser));
  return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyArray(inner.type_expr)), preds::Parser_advance(inner.parser)};
 }() : preds::TKind_is_ident(kind) ? [&]() -> preds::TypeResult { 
  mlc::String name = preds::TKind_ident(kind);
  return name == mlc::String("i32") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyI32{})), preds::Parser_advance(parser)} : name == mlc::String("string") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyString{})), preds::Parser_advance(parser)} : name == mlc::String("bool") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyBool{})), preds::Parser_advance(parser)} : name == mlc::String("unit") ? preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(parser)} : [&]() -> preds::TypeResult { 
  preds::Parser after = preds::Parser_advance(parser);
  return preds::TKind_is_op(preds::Parser_kind(after)) && preds::TKind_op_val(preds::Parser_kind(after)) == mlc::String("<") ? [&]() -> preds::TypeResult { 
  preds::TypesResult args = types::parse_type_args(preds::Parser_advance(after));
  std::shared_ptr<ast::TypeExpr> type_expr = name == mlc::String("Shared") && args.types.size() == 1 ? std::make_shared<ast::TypeExpr>(ast::TyShared(args.types[0])) : std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, args.types));
  return preds::TypeResult{type_expr, args.parser};
 }() : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyNamed(name)), after};
 }();
 }() : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance_by(parser, 2)};
}

preds::TypesResult parse_type_args(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
preds::TypeResult first = types::parse_type(parser);
types.push_back(first.type_expr);
preds::Parser state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::TypeResult next = types::parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
preds::Parser end_state = preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">") ? preds::Parser_advance(state) : state;
return preds::TypesResult{types, end_state};
}

} // namespace types

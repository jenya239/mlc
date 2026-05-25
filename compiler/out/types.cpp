#include "types.hpp"

#include "predicates.hpp"

namespace types {

using namespace predicates;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept;

predicates::TypesResult comma_separated_type_suffix_from_first(predicates::TypeResult first) noexcept;

predicates::TypesResult parse_paren_types(predicates::Parser parser) noexcept;

predicates::TypeResult parse_base_type(predicates::Parser parser) noexcept;

predicates::TypesResult parse_type_args(predicates::Parser parser) noexcept;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept{
predicates::TypeResult base = parse_base_type(parser);
return predicates::TKind_is_arrow(predicates::Parser_kind(base.parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult ret = parse_type(predicates::Parser_advance(base.parser));
  mlc::Array<std::shared_ptr<ast::TypeExpr>> params = mlc::Array<std::shared_ptr<ast::TypeExpr>>{base.type_expr};
  return predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(params, ret.type_expr)), ret.parser};
 }() : base;
}

predicates::TypesResult comma_separated_type_suffix_from_first(predicates::TypeResult first) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
types.push_back(first.type_expr);
predicates::Parser state = first.parser;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
predicates::TypeResult next = parse_type(predicates::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
return predicates::TypesResult{types, state};
}

predicates::TypesResult parse_paren_types(predicates::Parser parser) noexcept{return predicates::TKind_is_rparen(predicates::Parser_kind(parser)) ? predicates::TypesResult{{}, predicates::Parser_advance(parser)} : [&]() -> predicates::TypesResult { 
  predicates::TypesResult suffix = comma_separated_type_suffix_from_first(parse_type(parser));
  return predicates::TypesResult{suffix.types, predicates::TKind_is_rparen(predicates::Parser_kind(suffix.parser)) ? predicates::Parser_advance(suffix.parser) : suffix.parser};
 }();}

predicates::TypeResult parse_base_type(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_lparen(kind) ? [&]() -> predicates::TypeResult { 
  predicates::TypesResult paren_types = parse_paren_types(predicates::Parser_advance(parser));
  return predicates::TKind_is_arrow(predicates::Parser_kind(paren_types.parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult ret = parse_type(predicates::Parser_advance(paren_types.parser));
  return predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyFn(paren_types.types, ret.type_expr)), ret.parser};
 }() : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), paren_types.parser};
 }() : predicates::TKind_is_lbracket(kind) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult inner = parse_type(predicates::Parser_advance(parser));
  return predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyArray(inner.type_expr)), predicates::Parser_advance(inner.parser)};
 }() : predicates::TKind_is_ident(kind) ? [&]() -> predicates::TypeResult { 
  mlc::String name = predicates::TKind_ident(kind);
  return name == mlc::String("i32") ? predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyI32{})), predicates::Parser_advance(parser)} : name == mlc::String("string") ? predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyString{})), predicates::Parser_advance(parser)} : name == mlc::String("bool") ? predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyBool{})), predicates::Parser_advance(parser)} : name == mlc::String("unit") ? predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(parser)} : name == mlc::String("ref") ? [&]() -> predicates::TypeResult { 
  predicates::Parser after_ref = predicates::TKind_is_mut(predicates::Parser_kind(predicates::Parser_advance(parser))) ? predicates::Parser_advance_by(parser, 2) : predicates::Parser_advance(parser);
  predicates::TypeResult inner = parse_type(after_ref);
  mlc::Array<std::shared_ptr<ast::TypeExpr>> targs = mlc::Array<std::shared_ptr<ast::TypeExpr>>{inner.type_expr};
  return predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyGeneric(mlc::String("ref"), targs)), inner.parser};
 }() : [&]() -> predicates::TypeResult { 
  predicates::Parser after = predicates::Parser_advance(parser);
  return predicates::TKind_is_op(predicates::Parser_kind(after)) && predicates::TKind_op_val(predicates::Parser_kind(after)) == mlc::String("<") ? [&]() -> predicates::TypeResult { 
  predicates::TypesResult targs = parse_type_args(predicates::Parser_advance(after));
  std::shared_ptr<ast::TypeExpr> type_expr = name == mlc::String("Shared") && targs.types.size() == 1 ? std::make_shared<ast::TypeExpr>(ast::TyShared(targs.types[0])) : std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, targs.types));
  return predicates::TypeResult{type_expr, targs.parser};
 }() : predicates::TKind_is_dot(predicates::Parser_kind(after)) && predicates::TKind_is_ident(predicates::Parser_kind(predicates::Parser_advance(after))) ? [&]() -> predicates::TypeResult { 
  mlc::String assoc_name = predicates::TKind_ident(predicates::Parser_kind(predicates::Parser_advance(after)));
  return predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyAssoc(name, assoc_name)), predicates::Parser_advance_by(after, 2)};
 }() : predicates::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyNamed(name)), after};
 }();
 }() : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance_by(parser, 2)};
}

predicates::TypesResult parse_type_args(predicates::Parser parser) noexcept{
predicates::TypesResult suffix = comma_separated_type_suffix_from_first(parse_type(parser));
predicates::Parser closing_state = predicates::TKind_is_op(predicates::Parser_kind(suffix.parser)) && predicates::TKind_op_val(predicates::Parser_kind(suffix.parser)) == mlc::String(">") ? predicates::Parser_advance(suffix.parser) : suffix.parser;
return predicates::TypesResult{suffix.types, closing_state};
}

} // namespace types

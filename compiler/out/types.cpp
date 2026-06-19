#include "types.hpp"

#include "predicates.hpp"
#include "comma_separated.hpp"

namespace types {

using namespace predicates;
using namespace comma_separated;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept;

predicates::TypesResult parse_paren_types(predicates::Parser parser) noexcept;

predicates::TypeResult parse_base_type(predicates::Parser parser) noexcept;

predicates::TypesResult parse_type_args(predicates::Parser parser) noexcept;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept{
predicates::TypeResult base_parsed = parse_base_type(parser);
return predicates::TokenKind_is_arrow(predicates::Parser_kind(base_parsed.parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult return_type_parsed = parse_type(predicates::Parser_advance(base_parsed.parser));
  mlc::Array<std::shared_ptr<ast::TypeExpr>> params = mlc::Array<std::shared_ptr<ast::TypeExpr>>{base_parsed.value};
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyFn(params, return_type_parsed.value)), return_type_parsed.parser);
 }() : base_parsed;
}

predicates::TypesResult parse_paren_types(predicates::Parser parser) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)) ? [&]() -> predicates::TypesResult { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> empty_types = {};
  return predicates::types_parse_result(empty_types, predicates::Parser_advance(parser));
 }() : [&]() -> predicates::TypesResult { 
  predicates::TypesResult suffix_parsed = comma_separated::comma_separated_types_suffix_from_first(parse_type(parser), parse_type);
  return predicates::types_parse_result(suffix_parsed.value, predicates::TokenKind_is_rparen(predicates::Parser_kind(suffix_parsed.parser)) ? predicates::Parser_advance(suffix_parsed.parser) : suffix_parsed.parser);
 }();}

predicates::TypeResult parse_base_type(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_lparen(kind) ? [&]() -> predicates::TypeResult { 
  predicates::TypesResult paren_types_parsed = parse_paren_types(predicates::Parser_advance(parser));
  return predicates::TokenKind_is_arrow(predicates::Parser_kind(paren_types_parsed.parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult return_type_parsed = parse_type(predicates::Parser_advance(paren_types_parsed.parser));
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyFn(paren_types_parsed.value, return_type_parsed.value)), return_type_parsed.parser);
 }() : predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), paren_types_parsed.parser);
 }() : predicates::TokenKind_is_lbracket(kind) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult inner_parsed = parse_type(predicates::Parser_advance(parser));
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyArray(inner_parsed.value)), predicates::Parser_advance(inner_parsed.parser));
 }() : predicates::TokenKind_is_ident(kind) ? [&]() -> predicates::TypeResult { 
  mlc::String name = predicates::TokenKind_ident(kind);
  return [&]() -> predicates::TypeResult { if (name == mlc::String("i32")) { return predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyI32{})), predicates::Parser_advance(parser)); } if (name == mlc::String("string")) { return predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyString{})), predicates::Parser_advance(parser)); } if (name == mlc::String("bool")) { return predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyBool{})), predicates::Parser_advance(parser)); } if (name == mlc::String("unit")) { return predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(parser)); } if (name == mlc::String("ref")) { return [&]() -> predicates::TypeResult { 
  predicates::Parser after_ref = predicates::TokenKind_is_mut(predicates::Parser_kind(predicates::Parser_advance(parser))) ? predicates::Parser_advance_by(parser, 2) : predicates::Parser_advance(parser);
  predicates::TypeResult inner_parsed = parse_type(after_ref);
  mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments = mlc::Array<std::shared_ptr<ast::TypeExpr>>{inner_parsed.value};
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyGeneric(mlc::String("ref"), type_arguments)), inner_parsed.parser);
 }(); } return [&]() -> predicates::TypeResult { 
  predicates::Parser after = predicates::Parser_advance(parser);
  return predicates::TokenKind_is_op(predicates::Parser_kind(after)) && predicates::TokenKind_op_val(predicates::Parser_kind(after)) == mlc::String("<") ? [&]() -> predicates::TypeResult { 
  predicates::TypesResult type_args_parsed = parse_type_args(predicates::Parser_advance(after));
  std::shared_ptr<ast::TypeExpr> type_expression = name == mlc::String("Shared") && type_args_parsed.value.size() == 1 ? std::make_shared<ast::TypeExpr>(ast::TyShared(type_args_parsed.value[0])) : std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, type_args_parsed.value));
  return predicates::type_parse_result(type_expression, type_args_parsed.parser);
 }() : predicates::TokenKind_is_dot(predicates::Parser_kind(after)) && predicates::TokenKind_is_ident(predicates::Parser_kind(predicates::Parser_advance(after))) ? [&]() -> predicates::TypeResult { 
  mlc::String assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(after)));
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyAssoc(name, assoc_name)), predicates::Parser_advance_by(after, 2));
 }() : predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyNamed(name)), after);
 }(); }();
 }() : predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance_by(parser, 2));
}

predicates::TypesResult parse_type_args(predicates::Parser parser) noexcept{
predicates::TypesResult suffix_parsed = comma_separated::comma_separated_types_suffix_from_first(parse_type(parser), parse_type);
predicates::Parser closing_state = predicates::TokenKind_is_op(predicates::Parser_kind(suffix_parsed.parser)) && predicates::TokenKind_op_val(predicates::Parser_kind(suffix_parsed.parser)) == mlc::String(">") ? predicates::Parser_advance(suffix_parsed.parser) : suffix_parsed.parser;
return predicates::types_parse_result(suffix_parsed.value, closing_state);
}

} // namespace types

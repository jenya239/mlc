#define main mlc_user_main
#include "types.hpp"

#include "predicates.hpp"
#include "comma_separated.hpp"

namespace types {

using namespace predicates;
using namespace comma_separated;

predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_type(predicates::Parser parser) noexcept{
  auto base_parsed = parse_base_type(parser);
  if ((!predicates::TokenKind_is_arrow(predicates::Parser_kind(base_parsed.parser))))   {
    return base_parsed;
  }
  auto return_type_parsed = parse_type(predicates::Parser_advance(base_parsed.parser));
  auto params = mlc::Array<std::shared_ptr<ast::TypeExpr>>{base_parsed.value};
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyFn{params, return_type_parsed.value}), return_type_parsed.parser);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_paren_types(predicates::Parser parser) noexcept{
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)))   {
    auto empty_types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
    return predicates::types_parse_result(empty_types, predicates::Parser_advance(parser));
  }
  auto suffix_parsed = comma_separated::comma_separated_types_suffix_from_first(parse_type(parser), parse_type);
  if (predicates::TokenKind_is_rparen(predicates::Parser_kind(suffix_parsed.parser)))   {
    return predicates::types_parse_result(suffix_parsed.value, predicates::Parser_advance(suffix_parsed.parser));
  }
  return predicates::types_parse_result(suffix_parsed.value, suffix_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_ref_type(predicates::Parser parser) noexcept{
  auto state = predicates::Parser_advance(parser);
  if (predicates::TokenKind_is_mut(predicates::Parser_kind(state)))   {
    (state = predicates::Parser_advance(state));
  }
  auto inner_parsed = parse_type(state);
  auto type_arguments = mlc::Array<std::shared_ptr<ast::TypeExpr>>{inner_parsed.value};
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyGeneric{mlc::String("ref", 3), type_arguments}), inner_parsed.parser);
}
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_type_after_ident_name(predicates::Parser parser, mlc::String name) noexcept{
  auto after = predicates::Parser_advance(parser);
  if ((predicates::TokenKind_is_op(predicates::Parser_kind(after)) && (predicates::TokenKind_op_val(predicates::Parser_kind(after)) == mlc::String("<", 1))))   {
    auto type_args_parsed = parse_type_args(predicates::Parser_advance(after));
    if (((name == mlc::String("Shared", 6)) && (type_args_parsed.value.length() == 1)))     {
      return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyShared{type_args_parsed.value[0]}), type_args_parsed.parser);
    }
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyGeneric{name, type_args_parsed.value}), type_args_parsed.parser);
  }
  if ((predicates::TokenKind_is_dot(predicates::Parser_kind(after)) && predicates::TokenKind_is_ident(predicates::Parser_kind(predicates::Parser_advance(after)))))   {
    auto assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(after)));
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyAssoc{name, assoc_name}), predicates::Parser_advance_by(after, 2));
  }
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyNamed{name}), after);
}
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_base_type_ident(predicates::Parser parser, mlc::String name) noexcept{
  return [&]() -> predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> {
auto __match_subject = name;
if ((__match_subject == mlc::String("i32", 3))) {
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyI32{}), predicates::Parser_advance(parser));
} else if ((__match_subject == mlc::String("string", 6))) {
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyString{}), predicates::Parser_advance(parser));
} else if ((__match_subject == mlc::String("bool", 4))) {
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyBool{}), predicates::Parser_advance(parser));
} else if ((__match_subject == mlc::String("unit", 4))) {
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), predicates::Parser_advance(parser));
} else if ((__match_subject == mlc::String("ref", 3))) {
  return parse_ref_type(parser);
} else {
  return parse_type_after_ident_name(parser, name);
}
}();
}
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_base_type(predicates::Parser parser) noexcept{
  auto kind = predicates::Parser_kind(parser);
  if (predicates::TokenKind_is_lparen(kind))   {
    auto paren_types_parsed = parse_paren_types(predicates::Parser_advance(parser));
    if (predicates::TokenKind_is_arrow(predicates::Parser_kind(paren_types_parsed.parser)))     {
      auto return_type_parsed = parse_type(predicates::Parser_advance(paren_types_parsed.parser));
      return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyFn{paren_types_parsed.value, return_type_parsed.value}), return_type_parsed.parser);
    }
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), paren_types_parsed.parser);
  }
  if (predicates::TokenKind_is_lbracket(kind))   {
    auto inner_parsed = parse_type(predicates::Parser_advance(parser));
    return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyArray{inner_parsed.value}), predicates::Parser_advance(inner_parsed.parser));
  }
  if (predicates::TokenKind_is_ident(kind))   {
    return parse_base_type_ident(parser, predicates::TokenKind_ident(kind));
  }
  return predicates::type_parse_result(std::make_shared<ast::TypeExpr>(ast::TyUnit{}), predicates::Parser_advance_by(parser, 2));
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_type_args(predicates::Parser parser) noexcept{
  auto suffix_parsed = comma_separated::comma_separated_types_suffix_from_first(parse_type(parser), parse_type);
  if ((predicates::TokenKind_is_op(predicates::Parser_kind(suffix_parsed.parser)) && (predicates::TokenKind_op_val(predicates::Parser_kind(suffix_parsed.parser)) == mlc::String(">", 1))))   {
    return predicates::types_parse_result(suffix_parsed.value, predicates::Parser_advance(suffix_parsed.parser));
  }
  return predicates::types_parse_result(suffix_parsed.value, suffix_parsed.parser);
}

} // namespace types

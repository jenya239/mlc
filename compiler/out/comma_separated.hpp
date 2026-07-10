#ifndef COMMA_SEPARATED_HPP
#define COMMA_SEPARATED_HPP

#include "mlc.hpp"
#include <variant>
#include "predicates.hpp"
#include "ast.hpp"
namespace comma_separated {

template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> comma_separated_types_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> first, __F1 parse_one) noexcept;
template<typename __F2>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> comma_separated_params_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Param>> first, int parameter_slot_index, __F2 parse_one) noexcept;
template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> comma_separated_pats_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Pattern>> first, __F1 parse_one) noexcept;
template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> comma_separated_exprs_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Expr>> first, __F1 parse_one) noexcept;
template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> comma_separated_types_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> first, __F1 parse_one) noexcept{
  auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
  types.push_back(first.value);
  auto state = first.parser;
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    auto next_parsed = parse_one(predicates::Parser_advance(state));
    types.push_back(next_parsed.value);
    (state = next_parsed.parser);
  }
  return predicates::types_parse_result(types, state);
}
template<typename __F2>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> comma_separated_params_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Param>> first, int parameter_slot_index, __F2 parse_one) noexcept{
  auto parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
  parameters.push_back(first.value);
  auto state = first.parser;
  auto slot_index = parameter_slot_index;
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    auto next = parse_one(predicates::Parser_advance(state), slot_index);
    parameters.push_back(next.value);
    (state = next.parser);
    (slot_index = mlc::arith::checked_add(slot_index, 1));
  }
  return predicates::parameters_parse_result(parameters, state);
}
template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> comma_separated_pats_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Pattern>> first, __F1 parse_one) noexcept{
  auto patterns = mlc::Array<std::shared_ptr<ast::Pattern>>{};
  patterns.push_back(first.value);
  auto state = first.parser;
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    auto next = parse_one(predicates::Parser_advance(state));
    patterns.push_back(next.value);
    (state = next.parser);
  }
  return predicates::patterns_parse_result(patterns, state);
}
template<typename __F1>
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> comma_separated_exprs_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Expr>> first, __F1 parse_one) noexcept{
  auto expressions = mlc::Array<std::shared_ptr<ast::Expr>>{};
  expressions.push_back(first.value);
  auto state = first.parser;
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    auto next = parse_one(predicates::Parser_advance(state));
    expressions.push_back(next.value);
    (state = next.parser);
  }
  return predicates::expressions_parse_result(expressions, state);
}

} // namespace comma_separated

#endif // COMMA_SEPARATED_HPP

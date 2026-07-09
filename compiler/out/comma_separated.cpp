#define main mlc_user_main
#include "comma_separated.hpp"

#include "predicates.hpp"
#include "ast.hpp"

namespace comma_separated {

using namespace predicates;
using namespace ast;
using namespace ast_tokens;

predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> comma_separated_types_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::TypeExpr>>(predicates::Parser)> parse_one) noexcept{
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
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> comma_separated_params_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Param>> first, int parameter_slot_index, std::function<predicates::ParseResult<std::shared_ptr<ast::Param>>(predicates::Parser, int)> parse_one) noexcept{
  auto parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
  parameters.push_back(first.value);
  auto state = first.parser;
  auto slot_index = parameter_slot_index;
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state)))   {
    auto next = parse_one(predicates::Parser_advance(state), slot_index);
    parameters.push_back(next.value);
    (state = next.parser);
    (slot_index = (slot_index + 1));
  }
  return predicates::parameters_parse_result(parameters, state);
}
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> comma_separated_pats_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Pattern>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::Pattern>>(predicates::Parser)> parse_one) noexcept{
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
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> comma_separated_exprs_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Expr>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::Expr>>(predicates::Parser)> parse_one) noexcept{
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

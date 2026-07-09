#include "comma_separated.hpp"

#include "predicates.hpp"
#include "ast.hpp"

namespace comma_separated {

using namespace predicates;
using namespace ast;
using namespace ast_tokens;

predicates::TypesResult comma_separated_types_suffix_from_first(predicates::TypeResult first, std::function<predicates::TypeResult(predicates::Parser)> parse_one) noexcept;

predicates::ParamsResult comma_separated_params_suffix_from_first(predicates::ParamResult first, int parameter_slot_index, std::function<predicates::ParamResult(predicates::Parser, int)> parse_one) noexcept;

predicates::PatternsResult comma_separated_pats_suffix_from_first(predicates::PatternResult first, std::function<predicates::PatternResult(predicates::Parser)> parse_one) noexcept;

predicates::ExprsResult comma_separated_exprs_suffix_from_first(predicates::ExprResult first, std::function<predicates::ExprResult(predicates::Parser)> parse_one) noexcept;

predicates::TypesResult comma_separated_types_suffix_from_first(predicates::TypeResult first, std::function<predicates::TypeResult(predicates::Parser)> parse_one) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
types.push_back(first.value);
predicates::Parser state = first.parser;
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
predicates::TypeResult next_parsed = parse_one(predicates::Parser_advance(state));
types.push_back(next_parsed.value);
state = next_parsed.parser;
}
}
return predicates::types_parse_result(types, state);
}

predicates::ParamsResult comma_separated_params_suffix_from_first(predicates::ParamResult first, int parameter_slot_index, std::function<predicates::ParamResult(predicates::Parser, int)> parse_one) noexcept{
mlc::Array<std::shared_ptr<ast::Param>> parameters = {};
parameters.push_back(first.value);
predicates::Parser state = first.parser;
int slot_index = parameter_slot_index;
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
predicates::ParamResult next = parse_one(predicates::Parser_advance(state), slot_index);
parameters.push_back(next.value);
state = next.parser;
slot_index = slot_index + 1;
}
}
return predicates::parameters_parse_result(parameters, state);
}

predicates::PatternsResult comma_separated_pats_suffix_from_first(predicates::PatternResult first, std::function<predicates::PatternResult(predicates::Parser)> parse_one) noexcept{
mlc::Array<std::shared_ptr<ast::Pattern>> patterns = {};
patterns.push_back(first.value);
predicates::Parser state = first.parser;
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
predicates::PatternResult next = parse_one(predicates::Parser_advance(state));
patterns.push_back(next.value);
state = next.parser;
}
}
return predicates::patterns_parse_result(patterns, state);
}

predicates::ExprsResult comma_separated_exprs_suffix_from_first(predicates::ExprResult first, std::function<predicates::ExprResult(predicates::Parser)> parse_one) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> expressions = {};
expressions.push_back(first.value);
predicates::Parser state = first.parser;
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
predicates::ExprResult next = parse_one(predicates::Parser_advance(state));
expressions.push_back(next.value);
state = next.parser;
}
}
return predicates::expressions_parse_result(expressions, state);
}

} // namespace comma_separated

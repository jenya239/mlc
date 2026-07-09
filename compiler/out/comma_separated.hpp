#ifndef COMMA_SEPARATED_HPP
#define COMMA_SEPARATED_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"
#include "ast.hpp"

namespace comma_separated {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

predicates::TypesResult comma_separated_types_suffix_from_first(predicates::TypeResult first, std::function<predicates::TypeResult(predicates::Parser)> parse_one) noexcept;

predicates::ParamsResult comma_separated_params_suffix_from_first(predicates::ParamResult first, int parameter_slot_index, std::function<predicates::ParamResult(predicates::Parser, int)> parse_one) noexcept;

predicates::PatternsResult comma_separated_pats_suffix_from_first(predicates::PatternResult first, std::function<predicates::PatternResult(predicates::Parser)> parse_one) noexcept;

predicates::ExprsResult comma_separated_exprs_suffix_from_first(predicates::ExprResult first, std::function<predicates::ExprResult(predicates::Parser)> parse_one) noexcept;

} // namespace comma_separated

#endif // COMMA_SEPARATED_HPP

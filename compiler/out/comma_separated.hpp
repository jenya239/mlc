#ifndef COMMA_SEPARATED_HPP
#define COMMA_SEPARATED_HPP

#include "mlc.hpp"
#include <variant>
#include "predicates.hpp"
#include "ast.hpp"
namespace comma_separated {

predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> comma_separated_types_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::TypeExpr>>(predicates::Parser)> parse_one) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> comma_separated_params_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Param>> first, int parameter_slot_index, std::function<predicates::ParseResult<std::shared_ptr<ast::Param>>(predicates::Parser, int)> parse_one) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> comma_separated_pats_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Pattern>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::Pattern>>(predicates::Parser)> parse_one) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> comma_separated_exprs_suffix_from_first(predicates::ParseResult<std::shared_ptr<ast::Expr>> first, std::function<predicates::ParseResult<std::shared_ptr<ast::Expr>>(predicates::Parser)> parse_one) noexcept;

} // namespace comma_separated

#endif // COMMA_SEPARATED_HPP

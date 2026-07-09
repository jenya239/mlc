#ifndef TYPES_HPP
#define TYPES_HPP

#include "mlc.hpp"
#include <variant>
#include "predicates.hpp"
#include "comma_separated.hpp"
namespace types {

predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_type(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_paren_types(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_ref_type(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_type_after_ident_name(predicates::Parser parser, mlc::String name) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_base_type_ident(predicates::Parser parser, mlc::String name) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_base_type(predicates::Parser parser) noexcept;
bool parser_at_generic_close(predicates::Parser parser) noexcept;
predicates::Parser parser_advance_generic_close(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_type_args(predicates::Parser parser) noexcept;

} // namespace types

#endif // TYPES_HPP

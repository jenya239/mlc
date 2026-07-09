#ifndef TYPES_HPP
#define TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"
#include "comma_separated.hpp"

namespace types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept;

bool parser_at_generic_close(predicates::Parser parser) noexcept;

predicates::Parser parser_advance_generic_close(predicates::Parser parser) noexcept;

predicates::TypesResult parse_type_args(predicates::Parser parser) noexcept;

} // namespace types

#endif // TYPES_HPP

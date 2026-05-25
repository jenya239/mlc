#ifndef TYPES_HPP
#define TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"

namespace types {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

predicates::TypeResult parse_type(predicates::Parser parser) noexcept;

} // namespace types

#endif // TYPES_HPP

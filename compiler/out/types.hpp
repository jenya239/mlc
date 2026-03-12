#ifndef TYPES_HPP
#define TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"

namespace types {

struct Expr;
struct Stmt;

preds::TypeResult parse_type(preds::Parser parser) noexcept;

} // namespace types

#endif // TYPES_HPP

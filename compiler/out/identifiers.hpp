#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include "mlc.hpp"
#include <variant>

#include "context.hpp"

namespace identifiers {

struct Expr;
struct Stmt;

mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept;

} // namespace identifiers

#endif // IDENTIFIERS_HPP

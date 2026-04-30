#ifndef TYPES_HPP
#define TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"

namespace types {

preds::TypeResult parse_type(preds::Parser parser) noexcept;
preds::TypesResult parse_paren_types(preds::Parser parser) noexcept;
preds::TypeResult parse_base_type(preds::Parser parser) noexcept;
preds::TypesResult parse_type_args(preds::Parser parser) noexcept;

} // namespace types

#endif // TYPES_HPP

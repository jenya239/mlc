#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include "mlc.hpp"
#include <variant>

#include "context.hpp"
#include "cpp_naming.hpp"

namespace identifiers {

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept;
mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept;

} // namespace identifiers

#endif // IDENTIFIERS_HPP

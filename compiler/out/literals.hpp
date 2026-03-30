#ifndef LITERALS_HPP
#define LITERALS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_naming.hpp"

namespace literals {

struct Expr;
struct Stmt;

mlc::String gen_integer_literal(int integer_value) noexcept;

mlc::String gen_string_literal(mlc::String string_value) noexcept;

mlc::String gen_boolean_literal(bool boolean_value) noexcept;

mlc::String gen_unit_literal() noexcept;

mlc::String gen_extern_placeholder() noexcept;

} // namespace literals

#endif // LITERALS_HPP

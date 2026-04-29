#ifndef LITERALS_HPP
#define LITERALS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_naming.hpp"

namespace literals {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_integer_literal(int integer_value) noexcept;

mlc::String gen_string_literal(mlc::String string_value) noexcept;

mlc::String gen_boolean_literal(bool boolean_value) noexcept;

mlc::String gen_unit_literal() noexcept;

mlc::String gen_extern_placeholder() noexcept;

mlc::String gen_float_literal(mlc::String v) noexcept;

mlc::String gen_i64_literal(mlc::String v) noexcept;

mlc::String gen_u8_literal(mlc::String v) noexcept;

mlc::String gen_usize_literal(mlc::String v) noexcept;

mlc::String gen_char_literal(mlc::String v) noexcept;

} // namespace literals

#endif // LITERALS_HPP

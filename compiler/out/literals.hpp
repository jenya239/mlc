#ifndef LITERALS_HPP
#define LITERALS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "cpp_naming.hpp"
namespace literals {

mlc::String gen_integer_literal(int integer_value) noexcept;
mlc::String gen_string_literal(mlc::String string_value) noexcept;
mlc::String gen_boolean_literal(bool boolean_value) noexcept;
mlc::String gen_unit_literal() noexcept;
mlc::String gen_extern_placeholder() noexcept;
mlc::String gen_float_literal(mlc::String v) noexcept;
mlc::String gen_i64_literal(mlc::String v) noexcept;
mlc::String gen_u8_literal(mlc::String v) noexcept;
mlc::String gen_usize_literal(mlc::String v) noexcept;
mlc::String char_literal_printable_codepoint(mlc::String character) noexcept;
mlc::String gen_char_literal(mlc::String v) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_integer_literal_cpp(int integer_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_string_literal_cpp(mlc::String string_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_boolean_literal_cpp(bool boolean_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_unit_literal_cpp() noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_extern_placeholder_cpp() noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_float_literal_cpp(mlc::String v) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_i64_literal_cpp(mlc::String v) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_u8_literal_cpp(mlc::String v) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_usize_literal_cpp(mlc::String v) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_char_literal_cpp(mlc::String v) noexcept;

} // namespace literals

#endif // LITERALS_HPP

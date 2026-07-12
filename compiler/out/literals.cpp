#define main mlc_user_main
#include "literals.hpp"

#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "cpp_naming.hpp"

namespace literals {

using namespace cpp_ast;
using namespace emit_helpers;
using namespace cpp_naming;

mlc::String gen_integer_literal(int integer_value) noexcept{
  return mlc::to_string(integer_value);
}
mlc::String gen_string_literal(mlc::String string_value) noexcept{
  return ((((mlc::String("mlc::String(\"", 13) + cpp_naming::escape_str(string_value)) + mlc::String("\", ", 3)) + mlc::to_string(string_value.byte_size())) + mlc::String(")", 1));
}
mlc::String gen_boolean_literal(bool boolean_value) noexcept{
  if (boolean_value)   {
    return mlc::String("true", 4);
  } else   {
    return mlc::String("false", 5);
  }
}
mlc::String gen_unit_literal() noexcept{
  return mlc::String("std::make_tuple()", 17);
}
mlc::String gen_extern_placeholder() noexcept{
  return mlc::String("", 0);
}
mlc::String gen_float_literal(mlc::String v) noexcept{
  return v;
}
mlc::String gen_i64_literal(mlc::String v) noexcept{
  return v;
}
mlc::String gen_u8_literal(mlc::String v) noexcept{
  return ((mlc::String("static_cast<uint8_t>(", 21) + v) + mlc::String(")", 1));
}
mlc::String gen_usize_literal(mlc::String v) noexcept{
  return ((mlc::String("static_cast<size_t>(", 20) + v) + mlc::String(")", 1));
}
mlc::String char_literal_printable_codepoint(mlc::String character) noexcept{
  auto printable = (((((((mlc::String(" !\"#$%&'()*+,-./0123456789:;", 28) + mlc::String("<", 1)) + mlc::String("=", 1)) + mlc::String(">", 1)) + mlc::String("?", 1)) + mlc::String("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", 32)) + mlc::String("`", 1)) + mlc::String("abcdefghijklmnopqrstuvwxyz{|}~", 30));
  auto found_index = printable.index_of(character);
  if ((found_index >= 0))   {
    return mlc::to_string((32 + found_index));
  }
  return mlc::String("0", 1);
}
mlc::String gen_char_literal(mlc::String v) noexcept{
  auto codepoint = ((v == mlc::String("\n", 1)) ? (mlc::String("10", 2)) : (((v == mlc::String("\t", 1)) ? (mlc::String("9", 1)) : (((v == mlc::String("\r", 1)) ? (mlc::String("13", 2)) : (((v == mlc::String("\0", 1)) ? (mlc::String("0", 1)) : (((v.length() == 1) ? (char_literal_printable_codepoint(v)) : (mlc::String("0", 1)))))))))));
  return ((mlc::String("static_cast<char32_t>(", 22) + codepoint) + mlc::String(")", 1));
}
std::shared_ptr<cpp_ast::CppExpression> gen_integer_literal_cpp(int integer_value) noexcept{
  return emit_helpers::make_integer_cpp_expression(integer_value);
}
std::shared_ptr<cpp_ast::CppExpression> gen_string_literal_cpp(mlc::String string_value) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_string_literal(string_value));
}
std::shared_ptr<cpp_ast::CppExpression> gen_boolean_literal_cpp(bool boolean_value) noexcept{
  return emit_helpers::make_boolean_cpp_expression(boolean_value);
}
std::shared_ptr<cpp_ast::CppExpression> gen_unit_literal_cpp() noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_unit_literal());
}
std::shared_ptr<cpp_ast::CppExpression> gen_extern_placeholder_cpp() noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_extern_placeholder());
}
std::shared_ptr<cpp_ast::CppExpression> gen_float_literal_cpp(mlc::String v) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_float_literal(v));
}
std::shared_ptr<cpp_ast::CppExpression> gen_i64_literal_cpp(mlc::String v) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_i64_literal(v));
}
std::shared_ptr<cpp_ast::CppExpression> gen_u8_literal_cpp(mlc::String v) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_u8_literal(v));
}
std::shared_ptr<cpp_ast::CppExpression> gen_usize_literal_cpp(mlc::String v) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_usize_literal(v));
}
std::shared_ptr<cpp_ast::CppExpression> gen_char_literal_cpp(mlc::String v) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_char_literal(v));
}

} // namespace literals

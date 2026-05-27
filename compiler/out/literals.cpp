#include "literals.hpp"

#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "cpp_naming.hpp"

namespace literals {

using namespace cpp_ast;
using namespace emit_helpers;
using namespace cpp_naming;

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

std::shared_ptr<cpp_ast::CppExpr> gen_integer_literal_cpp(int integer_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_string_literal_cpp(mlc::String string_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_boolean_literal_cpp(bool boolean_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_unit_literal_cpp() noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_extern_placeholder_cpp() noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_float_literal_cpp(mlc::String v) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_i64_literal_cpp(mlc::String v) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_u8_literal_cpp(mlc::String v) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_usize_literal_cpp(mlc::String v) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_char_literal_cpp(mlc::String v) noexcept;

mlc::String gen_integer_literal(int integer_value) noexcept{return mlc::to_string(integer_value);}

mlc::String gen_string_literal(mlc::String string_value) noexcept{return mlc::String("mlc::String(\"") + cpp_naming::escape_str(string_value) + mlc::String("\", ") + mlc::to_string(string_value.length()) + mlc::String(")");}

mlc::String gen_boolean_literal(bool boolean_value) noexcept{return boolean_value ? mlc::String("true") : mlc::String("false");}

mlc::String gen_unit_literal() noexcept{return mlc::String("std::make_tuple()");}

mlc::String gen_extern_placeholder() noexcept{return mlc::String("");}

mlc::String gen_float_literal(mlc::String v) noexcept{return v;}

mlc::String gen_i64_literal(mlc::String v) noexcept{return v;}

mlc::String gen_u8_literal(mlc::String v) noexcept{return mlc::String("static_cast<uint8_t>(") + v + mlc::String(")");}

mlc::String gen_usize_literal(mlc::String v) noexcept{return mlc::String("static_cast<size_t>(") + v + mlc::String(")");}

mlc::String gen_char_literal(mlc::String v) noexcept{return mlc::String("static_cast<char32_t>(") + v + mlc::String(")");}

std::shared_ptr<cpp_ast::CppExpr> gen_integer_literal_cpp(int integer_value) noexcept{return emit_helpers::make_integer_cpp_expression(integer_value);}

std::shared_ptr<cpp_ast::CppExpr> gen_string_literal_cpp(mlc::String string_value) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_string_literal(string_value));}

std::shared_ptr<cpp_ast::CppExpr> gen_boolean_literal_cpp(bool boolean_value) noexcept{return emit_helpers::make_boolean_cpp_expression(boolean_value);}

std::shared_ptr<cpp_ast::CppExpr> gen_unit_literal_cpp() noexcept{return emit_helpers::make_identifier_cpp_expression(gen_unit_literal());}

std::shared_ptr<cpp_ast::CppExpr> gen_extern_placeholder_cpp() noexcept{return emit_helpers::make_identifier_cpp_expression(gen_extern_placeholder());}

std::shared_ptr<cpp_ast::CppExpr> gen_float_literal_cpp(mlc::String v) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_float_literal(v));}

std::shared_ptr<cpp_ast::CppExpr> gen_i64_literal_cpp(mlc::String v) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_i64_literal(v));}

std::shared_ptr<cpp_ast::CppExpr> gen_u8_literal_cpp(mlc::String v) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_u8_literal(v));}

std::shared_ptr<cpp_ast::CppExpr> gen_usize_literal_cpp(mlc::String v) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_usize_literal(v));}

std::shared_ptr<cpp_ast::CppExpr> gen_char_literal_cpp(mlc::String v) noexcept{return emit_helpers::make_identifier_cpp_expression(gen_char_literal(v));}

} // namespace literals

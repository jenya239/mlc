#ifndef LITERALS_HPP
#define LITERALS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "cpp_naming.hpp"

namespace literals {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

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

} // namespace literals

#endif // LITERALS_HPP

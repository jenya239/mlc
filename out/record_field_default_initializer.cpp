#define main mlc_user_main
#include "record_field_default_initializer.hpp"

namespace record_field_default_initializer {

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept{
return (((((operation == mlc::String("+", 1)) || (operation == mlc::String("-", 1))) || (operation == mlc::String("*", 1))) || (operation == mlc::String("/", 1))) || (operation == mlc::String("%", 1)));
}
bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return true; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return true; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return true; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return true; },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return true; },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return true; },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return true; },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return true; },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return true; },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return true; },
[&](const ast::ExprBin& exprBin) { auto [operation, left, right, __3] = exprBin; return ((binary_operator_permitted_for_record_field_default_initializer(operation) && record_field_default_expression_acceptable_for_codegen(left)) && record_field_default_expression_acceptable_for_codegen(right)); },
[&](const ast::ExprUn& exprUn) { auto [operation, inner, __2] = exprUn; return (((operation == mlc::String("-", 1)) || (operation == mlc::String("+", 1))) && record_field_default_expression_acceptable_for_codegen(inner)); },
[&](const auto& __v) { return false; }
}, (*expression));
}
mlc::String record_field_default_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept{
return CodegenContext_resolve(context, cpp_naming::map_builtin_identifier_reference(name));
}
mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [integer_value, __1] = exprInt; return literals::gen_integer_literal(integer_value); },
[&](const ast::ExprStr& exprStr) { auto [text_value, __1] = exprStr; return literals::gen_string_literal(text_value); },
[&](const ast::ExprBool& exprBool) { auto [boolean_value, __1] = exprBool; return literals::gen_boolean_literal(boolean_value); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return literals::gen_unit_literal(); },
[&](const ast::ExprFloat& exprFloat) { auto [raw, __1] = exprFloat; return literals::gen_float_literal(raw); },
[&](const ast::ExprI64& exprI64) { auto [raw, __1] = exprI64; return literals::gen_i64_literal(raw); },
[&](const ast::ExprU8& exprU8) { auto [raw, __1] = exprU8; return literals::gen_u8_literal(raw); },
[&](const ast::ExprUsize& exprUsize) { auto [raw, __1] = exprUsize; return literals::gen_usize_literal(raw); },
[&](const ast::ExprChar& exprChar) { auto [raw, __1] = exprChar; return literals::gen_char_literal(raw); },
[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return record_field_default_identifier_cpp(name, context); },
[&](const ast::ExprBin& exprBin) { auto [operation, left, right, __3] = exprBin; return expr::parenthesized_binary(record_field_default_expression_cpp_initializer(left, context), operation, record_field_default_expression_cpp_initializer(right, context)); },
[&](const ast::ExprUn& exprUn) { auto [operation, inner, __2] = exprUn; return expr::parenthesized_unary(operation, record_field_default_expression_cpp_initializer(inner, context)); },
[&](const auto& __v) { return mlc::String("0", 1); }
}, (*expression));
}

} // namespace record_field_default_initializer

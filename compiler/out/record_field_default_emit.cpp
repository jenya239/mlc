#define main mlc_user_main
#include "record_field_default_emit.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "expr.hpp"
#include "cpp_naming.hpp"

namespace record_field_default_emit {

using namespace ast;
using namespace context;
using namespace literals;
using namespace expr;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::String record_field_default_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept{
  return context::CodegenContext_resolve(context, cpp_naming::map_builtin_identifier_reference(name));
}
mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [integer_value, __1] = exprInt; return literals::gen_integer_literal(integer_value);
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [text_value, __1] = exprStr; return literals::gen_string_literal(text_value);
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [boolean_value, __1] = exprBool; return literals::gen_boolean_literal(boolean_value);
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return literals::gen_unit_literal();
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [raw, __1] = exprFloat; return literals::gen_float_literal(raw);
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [raw, __1] = exprI64; return literals::gen_i64_literal(raw);
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [raw, __1] = exprU8; return literals::gen_u8_literal(raw);
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [raw, __1] = exprUsize; return literals::gen_usize_literal(raw);
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [raw, __1] = exprChar; return literals::gen_char_literal(raw);
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return record_field_default_identifier_cpp(name, context);
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [operation, left_expression, right_expression, __3] = exprBin; return expr::parenthesized_binary(record_field_default_expression_cpp_initializer(left_expression, context), operation, record_field_default_expression_cpp_initializer(right_expression, context));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [operation, inner_expression, __2] = exprUn; return expr::parenthesized_unary(operation, record_field_default_expression_cpp_initializer(inner_expression, context));
}
return mlc::to_string(0);
std::abort();
}();
}

} // namespace record_field_default_emit

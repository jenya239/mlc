#define main mlc_user_main
#include "record_field_default_validate.hpp"

#include "ast.hpp"

namespace record_field_default_validate {

using namespace ast;
using namespace ast_tokens;

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept{
  return (((((operation == mlc::String("+", 1)) || (operation == mlc::String("-", 1))) || (operation == mlc::String("*", 1))) || (operation == mlc::String("/", 1))) || (operation == mlc::String("%", 1)));
}
bool record_field_default_expression_is_static_initializer(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [__0, __1] = exprInt; return true;
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [__0, __1] = exprStr; return true;
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [__0, __1] = exprBool; return true;
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return true;
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [__0, __1] = exprFloat; return true;
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [__0, __1] = exprI64; return true;
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [__0, __1] = exprU8; return true;
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [__0, __1] = exprUsize; return true;
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [__0, __1] = exprChar; return true;
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [__0, __1] = exprIdent; return true;
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [operation, left_expression, right_expression, __3] = exprBin; return ((binary_operator_permitted_for_record_field_default_initializer(operation) && record_field_default_expression_is_static_initializer(left_expression)) && record_field_default_expression_is_static_initializer(right_expression));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [operation, inner_expression, __2] = exprUn; return (((operation == mlc::String("-", 1)) || (operation == mlc::String("+", 1))) && record_field_default_expression_is_static_initializer(inner_expression));
}
return false;
std::abort();
}();
}

} // namespace record_field_default_validate

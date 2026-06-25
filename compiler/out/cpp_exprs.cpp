#include "cpp_exprs.hpp"

#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "lexer.hpp"
#include "cpp_predicates.hpp"

namespace cpp_exprs {

using namespace cpp_ast;
using namespace cpp_tokens;
using namespace lexer;
using namespace cpp_predicates;

struct CppExpressionListParse {mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions;int position;};

cpp_exprs::CppExpressionParseResult cpp_expr_error_result(int position, mlc::String message) noexcept;

int cpp_expr_hex_digit_value(mlc::String character) noexcept;

int cpp_expr_parse_hex_integer(mlc::String hex_text) noexcept;

mlc::String cpp_expr_infix_symbol(cpp_tokens::CppTokenKind kind) noexcept;

int cpp_expr_infix_precedence(mlc::String symbol) noexcept;

bool cpp_expr_is_right_associative(mlc::String symbol) noexcept;

bool cpp_expr_is_prefix_unary(mlc::String symbol) noexcept;

cpp_exprs::CppExpressionListParse cpp_expr_parse_argument_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_exprs::CppExpressionParseResult cpp_expr_parse_primary(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_exprs::CppExpressionParseResult cpp_expr_parse_prefix(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_exprs::CppExpressionParseResult cpp_expr_parse_postfix(mlc::Array<cpp_tokens::CppToken> tokens, cpp_exprs::CppExpressionParseResult parsed) noexcept;

cpp_exprs::CppExpressionParseResult cpp_expr_parse_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position, int minimum_precedence) noexcept;

cpp_exprs::CppExpressionParseResult parse_cpp_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_exprs::CppExpressionParseResult cpp_expr_error_result(int position, mlc::String message) noexcept{return cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(0)), position, message};}

int cpp_expr_hex_digit_value(mlc::String character) noexcept{
return lexer::is_digit(character) ? character.to_i() : character == mlc::String("a") || character == mlc::String("A") ? 10 : character == mlc::String("b") || character == mlc::String("B") ? 11 : character == mlc::String("c") || character == mlc::String("C") ? 12 : character == mlc::String("d") || character == mlc::String("D") ? 13 : character == mlc::String("e") || character == mlc::String("E") ? 14 : character == mlc::String("f") || character == mlc::String("F") ? 15 : 0;
}

int cpp_expr_parse_hex_integer(mlc::String hex_text) noexcept{
int value = 0;
int index = 2;
while (index < hex_text.length()){
{
value = value * 16 + cpp_expr_hex_digit_value(hex_text.char_at(index));
index = index + 1;
}
}
return value;
}

mlc::String cpp_expr_infix_symbol(cpp_tokens::CppTokenKind kind) noexcept{
return cpp_predicates::cpp_is_or_operator(kind) ? mlc::String("||") : cpp_predicates::cpp_is_and_operator(kind) ? mlc::String("&&") : cpp_predicates::cpp_is_equals_operator(kind) ? mlc::String("==") : cpp_predicates::cpp_is_not_equals_operator(kind) ? mlc::String("!=") : cpp_predicates::cpp_is_less_equals_operator(kind) ? mlc::String("<=") : cpp_predicates::cpp_is_greater_equals_operator(kind) ? mlc::String(">=") : cpp_predicates::cpp_is_left_shift_operator(kind) ? mlc::String("<<") : cpp_predicates::cpp_is_right_shift_operator(kind) ? mlc::String(">>") : cpp_predicates::cpp_get_operator(kind);
}

int cpp_expr_infix_precedence(mlc::String symbol) noexcept{
return symbol == mlc::String("=") ? 1 : symbol == mlc::String("||") ? 3 : symbol == mlc::String("&&") ? 4 : symbol == mlc::String("|") ? 5 : symbol == mlc::String("^") ? 6 : symbol == mlc::String("&") ? 7 : symbol == mlc::String("==") || symbol == mlc::String("!=") ? 8 : symbol == mlc::String("<") || symbol == mlc::String(">") || symbol == mlc::String("<=") || symbol == mlc::String(">=") ? 9 : symbol == mlc::String("<<") || symbol == mlc::String(">>") ? 10 : symbol == mlc::String("+") || symbol == mlc::String("-") ? 11 : symbol == mlc::String("*") || symbol == mlc::String("/") || symbol == mlc::String("%") ? 12 : 0;
}

bool cpp_expr_is_right_associative(mlc::String symbol) noexcept{return symbol == mlc::String("=");}

bool cpp_expr_is_prefix_unary(mlc::String symbol) noexcept{return symbol == mlc::String("+") || symbol == mlc::String("-") || symbol == mlc::String("!") || symbol == mlc::String("~") || symbol == mlc::String("*");}

cpp_exprs::CppExpressionParseResult cpp_expr_parse_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position, int minimum_precedence) noexcept;

cpp_exprs::CppExpressionListParse cpp_expr_parse_argument_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments = {};
int scan_position = position;
if (!cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
cpp_exprs::CppExpressionParseResult first = cpp_expr_parse_expression(tokens, scan_position, 0);
arguments.push_back(first.expression);
scan_position = first.position;
[&]() { 
  while (cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
cpp_exprs::CppExpressionParseResult next = cpp_expr_parse_expression(tokens, scan_position + 1, 0);
arguments.push_back(next.expression);
scan_position = next.position;
}
}
 }();
}
}
if (cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
return cpp_exprs::CppExpressionListParse{arguments, scan_position};
}

cpp_exprs::CppExpressionParseResult cpp_expr_parse_primary(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, position);
return cpp_predicates::cpp_is_integer_literal(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(cpp_predicates::cpp_get_integer(kind))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_hex_literal(kind) ? [&]() -> cpp_exprs::CppExpressionParseResult { 
  mlc::String hex_text = cpp_predicates::cpp_get_hex_literal(kind);
  return cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(cpp_expr_parse_hex_integer(hex_text))), position + 1, mlc::String("")};
 }() : cpp_predicates::cpp_is_float_literal(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppFloatLiteral(cpp_predicates::cpp_get_float_literal(kind))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_char_literal(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCharLiteral(cpp_predicates::cpp_get_char_literal(kind))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_string(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppStr(cpp_predicates::cpp_get_string(kind))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_true_keyword(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBool(true)), position + 1, mlc::String("")} : cpp_predicates::cpp_is_false_keyword(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBool(false)), position + 1, mlc::String("")} : cpp_predicates::cpp_is_nullptr_keyword(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("nullptr"))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_identifier(kind) ? cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(cpp_predicates::cpp_get_identifier(kind))), position + 1, mlc::String("")} : cpp_predicates::cpp_is_left_paren(kind) ? [&]() -> cpp_exprs::CppExpressionParseResult { 
  cpp_exprs::CppExpressionParseResult inner = cpp_expr_parse_expression(tokens, position + 1, 0);
  int after_paren = cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, inner.position)) ? inner.position + 1 : inner.position;
  return cpp_exprs::CppExpressionParseResult{inner.expression, after_paren, inner.error_message};
 }() : cpp_expr_error_result(position, mlc::String("expected expression"));
}

cpp_exprs::CppExpressionParseResult cpp_expr_parse_prefix(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, position);
mlc::String operator_symbol = cpp_expr_infix_symbol(kind);
return cpp_expr_is_prefix_unary(operator_symbol) ? [&]() -> cpp_exprs::CppExpressionParseResult { 
  cpp_exprs::CppExpressionParseResult operand = cpp_expr_parse_expression(tokens, position + 1, 12);
  return cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(operator_symbol, operand.expression)), operand.position, operand.error_message};
 }() : cpp_expr_parse_primary(tokens, position);
}

cpp_exprs::CppExpressionParseResult cpp_expr_parse_postfix(mlc::Array<cpp_tokens::CppToken> tokens, cpp_exprs::CppExpressionParseResult parsed) noexcept{
cpp_exprs::CppExpressionParseResult current = std::move(parsed);
bool continue_loop = true;
while (continue_loop && current.error_message == mlc::String("")){
{
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, current.position);
if (cpp_predicates::cpp_is_left_paren(kind)){
{
cpp_exprs::CppExpressionListParse arguments = cpp_expr_parse_argument_list(tokens, current.position + 1);
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(current.expression, arguments.expressions)), arguments.position, mlc::String("")};
}
} else {
{
if (cpp_predicates::cpp_is_dot(kind)){
mlc::String member_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, current.position + 1));
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(current.expression, member_name, false)), current.position + 2, mlc::String("")};
} else {
if (cpp_predicates::cpp_is_arrow(kind)){
mlc::String member_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, current.position + 1));
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(current.expression, member_name, true)), current.position + 2, mlc::String("")};
} else {
if (cpp_predicates::cpp_is_left_bracket(kind)){
cpp_exprs::CppExpressionParseResult index = cpp_expr_parse_expression(tokens, current.position + 1, 0);
int after_index = cpp_predicates::cpp_is_right_bracket(cpp_predicates::cpp_token_kind_at(tokens, index.position)) ? index.position + 1 : index.position;
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIndex(current.expression, index.expression)), after_index, index.error_message};
} else {
continue_loop = false;
}
}
}
}
}
}
}
return current;
}

cpp_exprs::CppExpressionParseResult cpp_expr_parse_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position, int minimum_precedence) noexcept{
cpp_exprs::CppExpressionParseResult prefix = cpp_expr_parse_prefix(tokens, position);
cpp_exprs::CppExpressionParseResult current = cpp_expr_parse_postfix(tokens, prefix);
bool continue_loop = true;
while (continue_loop && current.error_message == mlc::String("")){
{
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, current.position);
if (cpp_predicates::cpp_is_question(kind)){
{
if (minimum_precedence > 2){
continue_loop = false;
} else {
cpp_exprs::CppExpressionParseResult then_branch = cpp_expr_parse_expression(tokens, current.position + 1, 0);
int after_question = then_branch.position;
int else_position = cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, after_question)) ? after_question + 1 : after_question;
cpp_exprs::CppExpressionParseResult else_branch = cpp_expr_parse_expression(tokens, else_position, 2);
mlc::String combined_error = then_branch.error_message != mlc::String("") ? then_branch.error_message : else_branch.error_message;
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppTernary(current.expression, then_branch.expression, else_branch.expression)), else_branch.position, combined_error};
}
}
} else {
{
mlc::String operator_symbol = cpp_expr_infix_symbol(kind);
int operator_precedence = cpp_expr_infix_precedence(operator_symbol);
if (operator_precedence == 0 || operator_precedence < minimum_precedence){
continue_loop = false;
} else {
int right_minimum = cpp_expr_is_right_associative(operator_symbol) ? operator_precedence : operator_precedence + 1;
cpp_exprs::CppExpressionParseResult right = cpp_expr_parse_expression(tokens, current.position + 1, right_minimum);
current = cpp_exprs::CppExpressionParseResult{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(operator_symbol, current.expression, right.expression)), right.position, right.error_message};
}
}
}
}
}
return current;
}

cpp_exprs::CppExpressionParseResult parse_cpp_expression(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_expr_parse_expression(tokens, position, 0);}

} // namespace cpp_exprs

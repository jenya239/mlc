#include "ast_printer.hpp"

#include "ast.hpp"

namespace ast_printer {

using namespace ast;
using namespace ast_tokens;

mlc::String escape_string_literal_content(mlc::String input) noexcept;

mlc::String print_string_literal(mlc::String value) noexcept;

mlc::String print_expr(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::String escape_string_literal_content(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < input.length()){
{
mlc::String character = input.char_at(index);
if (character == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (character == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (character == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (character == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (character == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
parts.push_back(character);
}
}
}
}
}
}
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String print_string_literal(mlc::String value) noexcept{return mlc::String("\"") + escape_string_literal_content(value) + mlc::String("\"");}

mlc::String print_expr(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [value, _w0] = _v_exprint; return mlc::to_string(value); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [value, _w0] = _v_exprstr; return print_string_literal(value); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [true, _w0] = _v_exprbool; return mlc::String("true"); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [false, _w0] = _v_exprbool; return mlc::String("false"); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return mlc::String("()"); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String("<unsupported-expr>"); }();}

} // namespace ast_printer

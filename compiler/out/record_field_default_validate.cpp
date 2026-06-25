#include "record_field_default_validate.hpp"

#include "ast.hpp"

namespace record_field_default_validate {

using namespace ast;
using namespace ast_tokens;

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept;

bool record_field_default_expression_is_static_initializer(std::shared_ptr<ast::Expr> expression) noexcept;

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept{return operation == mlc::String("+") || operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%");}

bool record_field_default_expression_is_static_initializer(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [_w0, _w1] = _v_exprint; return true; } else if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); return true; } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [_w0, _w1] = _v_exprfloat; return true; } else if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); return true; } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [_w0, _w1] = _v_exprident; return true; } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left_expression, right_expression, _w0] = _v_exprbin; return binary_operator_permitted_for_record_field_default_initializer(operation) && record_field_default_expression_is_static_initializer(left_expression) && record_field_default_expression_is_static_initializer(right_expression); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner_expression, _w0] = _v_exprun; return operation == mlc::String("-") || operation == mlc::String("+") && record_field_default_expression_is_static_initializer(inner_expression); } return false; }();}

} // namespace record_field_default_validate

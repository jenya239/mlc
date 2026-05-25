#include "record_defaults.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "expr.hpp"
#include "cpp_naming.hpp"

namespace record_defaults {

using namespace ast;
using namespace context;
using namespace literals;
using namespace expr;
using namespace cpp_naming;
using namespace ast_tokens;

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept;

bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::String record_field_default_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept;

mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept{return operation == mlc::String("+") || operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%");}

bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [_w0, _w1] = _v_exprint; return true; } else if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); return true; } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [_w0, _w1] = _v_exprfloat; return true; } else if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); return true; } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [_w0, _w1] = _v_exprident; return true; } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, _w0] = _v_exprbin; return binary_operator_permitted_for_record_field_default_initializer(operation) && record_field_default_expression_acceptable_for_codegen(left) && record_field_default_expression_acceptable_for_codegen(right); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner, _w0] = _v_exprun; return operation == mlc::String("-") || operation == mlc::String("+") && record_field_default_expression_acceptable_for_codegen(inner); } return false; }();}

mlc::String record_field_default_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept{return context::CodegenContext_resolve(context, cpp_naming::map_builtin_identifier_reference(name));}

mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [integer_value, _w0] = _v_exprint; return literals::gen_integer_literal(integer_value); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [text_value, _w0] = _v_exprstr; return literals::gen_string_literal(text_value); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [boolean_value, _w0] = _v_exprbool; return literals::gen_boolean_literal(boolean_value); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return literals::gen_unit_literal(); } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [raw, _w0] = _v_exprfloat; return literals::gen_float_literal(raw); } if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); auto [raw, _w0] = _v_expri64; return literals::gen_i64_literal(raw); } if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); auto [raw, _w0] = _v_expru8; return literals::gen_u8_literal(raw); } if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); auto [raw, _w0] = _v_exprusize; return literals::gen_usize_literal(raw); } if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); auto [raw, _w0] = _v_exprchar; return literals::gen_char_literal(raw); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return record_field_default_identifier_cpp(name, context); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, _w0] = _v_exprbin; return expr::parenthesized_binary(record_field_default_expression_cpp_initializer(left, context), operation, record_field_default_expression_cpp_initializer(right, context)); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner, _w0] = _v_exprun; return expr::parenthesized_unary(operation, record_field_default_expression_cpp_initializer(inner, context)); } return mlc::String("0"); }();}

} // namespace record_defaults

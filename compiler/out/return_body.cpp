#include "return_body.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "eval.hpp"
#include "stmt_cpp.hpp"

namespace return_body {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace context;
using namespace expr;
using namespace eval;
using namespace stmt_cpp;

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SExpr> body_expression, context::CodegenContext context) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expr, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); } return expr::else_brace_block(gen_return_body(else_expr, context)); }();
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
 }(); } return expr::if_always_true_block(gen_return_body(expr, context)); }();}

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBlock>((*expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expr)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  context::GenStmtsWithContext statements_with_context = eval::gen_stmts_str_with_try(statements, context, 0);
  context::CodegenContext final_context = statements_with_context.codegen_context;
  mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.partial_result);
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprQuestion>((*result_expr)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*result_expr)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return statements_code + eval::gen_try_unwrap(inner_expr, final_context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expr)._); auto [_w0, _w1] = _v_sexprunit; return statements_code; } if (std::holds_alternative<semantic_ir::SExprIf>((*result_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<semantic_ir::SExprMatch>((*result_expr)._)) { auto _v_sexprmatch = std::get<semantic_ir::SExprMatch>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_sexprmatch; return statements_code + gen_return_body(result_expr, final_context); } if (std::holds_alternative<semantic_ir::SExprBlock>((*result_expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*result_expr)._); auto [_w0, _w1, _w2, _w3] = _v_sexprblock; return statements_code + gen_return_body(result_expr, final_context); } return statements_code + expr::return_line(eval::gen_expr(result_expr, final_context)); }();
 }(); } if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [condition, then_expr, else_expr, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expr, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); } return expr::else_brace_block(gen_return_body(else_expr, context)); }();
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
 }(); } if (std::holds_alternative<semantic_ir::SExprUnit>((*expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*expr)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprQuestion>((*expr)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expr)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return eval::gen_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } return expr::return_line(eval::gen_expr(expr, context)); }();}

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept{return gen_return_body(body_expr, context);}

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SExpr> body_expression, context::CodegenContext context) noexcept{return stmt_cpp::cpp_stmts_from_string_output(gen_return_body(body_expression, context));}

} // namespace return_body

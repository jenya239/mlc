#include "return_body.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "eval.hpp"
#include "stmt_cpp.hpp"
#include "expr_visitor_cpp.hpp"

namespace return_body {

using namespace registry;
using namespace semantic_ir;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace expr;
using namespace eval;
using namespace stmt_cpp;
using namespace expr_visitor_cpp;

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

mlc::String gen_stmts_for_return_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_expr_for_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> try_result_identifier_expression(mlc::String try_identifier) noexcept;

std::shared_ptr<cpp_ast::CppExpression> try_result_address_expression(mlc::String try_identifier) noexcept;

std::shared_ptr<cpp_ast::CppExpression> try_result_err_pointer_expression(mlc::String try_identifier) noexcept;

std::shared_ptr<cpp_ast::CppExpression> try_result_ok_field0_expression(mlc::String try_identifier) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_try_unwrap_return_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_leaf_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppStatement> gen_return_body_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppStatement> gen_return_body_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppStatement> gen_return_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_if_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_block_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_match_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> match_arms, std::shared_ptr<registry::Type> match_type, ast::Span span, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expression, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return expr::else_fragment_raw(gen_return_if_stmt(else_expression, context)); } return expr::else_brace_block(gen_return_body(else_expression, context)); }();
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
 }(); } return expr::if_always_true_block(gen_return_body(expression, context)); }();}

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> mlc::String { 
  context::GenStmtsWithContext statements_with_context = eval::gen_stmts_str_with_try(statements, context, 0);
  context::CodegenContext final_context = statements_with_context.codegen_context;
  mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*result_expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*result_expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return statements_code + eval::gen_try_unwrap(inner_expression, final_context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return statements_code; } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return statements_code + gen_return_body(result_expression, final_context); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*result_expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*result_expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionmatch; return statements_code + gen_return_body(result_expression, final_context); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*result_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*result_expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionblock; return statements_code + gen_return_body(result_expression, final_context); } return statements_code + expr::return_line(eval::gen_expr(result_expression, final_context)); }();
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expression, context));
  mlc::String else_string = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return expr::else_fragment_raw(gen_return_if_stmt(else_expression, context)); } return expr::else_brace_block(gen_return_body(else_expression, context)); }();
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return [&]() -> mlc::String { 
  mlc::String try_identifier = mlc::String("__try_ret");
  return eval::gen_try_unwrap(inner_expression, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
 }(); } return expr::return_line(eval::gen_expr(expression, context)); }();}

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept{return gen_return_body(body_expression, context);}

mlc::String gen_stmts_for_return_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return stmt_cpp::print_cpp_statements(stmt_cpp::gen_stmts_cpp(statements, context));}

std::shared_ptr<cpp_ast::CppExpression> gen_expr_for_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_for_return_body_cpp);}

std::shared_ptr<cpp_ast::CppExpression> try_result_identifier_expression(mlc::String try_identifier) noexcept{return emit_helpers::make_identifier_cpp_expression(try_identifier);}

std::shared_ptr<cpp_ast::CppExpression> try_result_address_expression(mlc::String try_identifier) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(mlc::String("&"), try_result_identifier_expression(try_identifier)));}

std::shared_ptr<cpp_ast::CppExpression> try_result_err_pointer_expression(mlc::String try_identifier) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::get_if<1>"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{try_result_address_expression(try_identifier)}));}

std::shared_ptr<cpp_ast::CppExpression> try_result_ok_field0_expression(mlc::String try_identifier) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::get<0>"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{try_result_identifier_expression(try_identifier)})), mlc::String("field0"), false));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_try_unwrap_return_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context) noexcept{
mlc::String try_identifier = mlc::String("__try_ret");
std::shared_ptr<cpp_ast::CppExpression> inner_cpp = gen_expr_for_return_body_cpp(inner_expression, context);
std::shared_ptr<cpp_ast::CppExpression> error_pointer = try_result_err_pointer_expression(try_identifier);
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(emit_helpers::make_auto_cpp_statement(try_identifier, inner_cpp));
statements.push_back(emit_helpers::make_if_cpp_statement(error_pointer, emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(mlc::String("*"), error_pointer)))}), emit_helpers::make_block_cpp_statement({})));
statements.push_back(emit_helpers::make_return_cpp_statement(try_result_ok_field0_expression(try_identifier)));
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_leaf_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*body_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*body_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  return statements;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*body_expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*body_expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return gen_try_unwrap_return_statements_cpp(inner_expression, context); } return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(emit_helpers::make_return_cpp_statement(gen_expr_for_return_body_cpp(body_expression, context)));
  return statements;
 }(); }();}

std::shared_ptr<cpp_ast::CppStatement> gen_return_body_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context) noexcept{return emit_helpers::make_block_cpp_statement(gen_return_body_cpp(branch_expression, context));}

std::shared_ptr<cpp_ast::CppStatement> gen_return_body_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return emit_helpers::make_block_cpp_statement({}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [condition, then_expression, nested_else_expression, _w0, _w1] = _v_semanticexpressionif; return gen_return_if_cpp_statement(condition, then_expression, nested_else_expression, context); } return gen_return_body_branch_block(else_expression, context); }();}

std::shared_ptr<cpp_ast::CppStatement> gen_return_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept{return emit_helpers::make_if_cpp_statement(gen_expr_for_return_body_cpp(condition, context), gen_return_body_branch_block(then_expression, context), gen_return_body_else_branch(else_expression, context));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_if_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(gen_return_if_cpp_statement(condition, then_expression, else_expression, context));
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_block_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context) noexcept{
stmt_cpp::GenStmtsCppWithContext statements_with_context = stmt_cpp::gen_stmts_cpp_with_context(block_statements, context, 0);
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output_statements = {};
int prefix_index = 0;
while (prefix_index < statements_with_context.statements.size()){
{
output_statements.push_back(statements_with_context.statements[prefix_index]);
prefix_index = prefix_index + 1;
}
}
context::CodegenContext final_context = statements_with_context.codegen_context;
return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*result_expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*result_expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> tail_statements = gen_try_unwrap_return_statements_cpp(inner_expression, final_context);
  int tail_index = 0;
  while (tail_index < tail_statements.size()){
{
output_statements.push_back(tail_statements[tail_index]);
tail_index = tail_index + 1;
}
}
  return output_statements;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return output_statements; } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> tail_statements = gen_return_body_cpp(result_expression, final_context);
  int tail_index = 0;
  while (tail_index < tail_statements.size()){
{
output_statements.push_back(tail_statements[tail_index]);
tail_index = tail_index + 1;
}
}
  return output_statements;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*result_expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*result_expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionmatch; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> tail_statements = gen_return_body_cpp(result_expression, final_context);
  int tail_index = 0;
  while (tail_index < tail_statements.size()){
{
output_statements.push_back(tail_statements[tail_index]);
tail_index = tail_index + 1;
}
}
  return output_statements;
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*result_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*result_expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionblock; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> tail_statements = gen_return_body_cpp(result_expression, final_context);
  int tail_index = 0;
  while (tail_index < tail_statements.size()){
{
output_statements.push_back(tail_statements[tail_index]);
tail_index = tail_index + 1;
}
}
  return output_statements;
 }(); } return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  output_statements.push_back(emit_helpers::make_return_cpp_statement(gen_expr_for_return_body_cpp(result_expression, final_context)));
  return output_statements;
 }(); }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_match_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> match_arms, std::shared_ptr<registry::Type> match_type, ast::Span span, context::CodegenContext context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(emit_helpers::make_return_cpp_statement(gen_expr_for_return_body_cpp(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(subject, match_arms, match_type, span)), context)));
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*body_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*body_expression)._); auto [block_statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return gen_return_block_body_cpp(block_statements, result_expression, context); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*body_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*body_expression)._); auto [condition, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return gen_return_if_body_cpp(condition, then_expression, else_expression, context); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*body_expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*body_expression)._); auto [subject, match_arms, match_type, span] = _v_semanticexpressionmatch; return gen_return_match_body_cpp(subject, match_arms, match_type, span, context); } return gen_return_body_leaf_cpp(body_expression, context); }();}

} // namespace return_body

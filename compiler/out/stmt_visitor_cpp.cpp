#include "stmt_visitor_cpp.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "stmt_eval.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_visitor_cpp {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace cpp_printer;
using namespace emit_helpers;
using namespace context;
using namespace cpp_naming;
using namespace stmt_eval;
using namespace expr_visitor_cpp;

struct StmtsCppFoldState {mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements;context::CodegenContext codegen_context;int next_try;};

struct GenStmtCppResult {std::shared_ptr<cpp_ast::CppStmt> statement;int next_try;};

mlc::String strip_trailing_newline(mlc::String text) noexcept;

mlc::String strip_trailing_semicolon(mlc::String text) noexcept;

std::shared_ptr<cpp_ast::CppStmt> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept;

mlc::String gen_expr_string_for_stmt_bridge(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

context::GenStmtResult stmt_string_via_eval(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

stmt_visitor_cpp::GenStmtCppResult stmt_via_string_bridge(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

stmt_visitor_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

stmt_visitor_cpp::StmtsCppFoldState stmts_cpp_fold_step(stmt_visitor_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

stmt_visitor_cpp::StmtsCppFoldState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> eval_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

mlc::String eval_stmts_cpp_as_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept;

mlc::String strip_trailing_newline(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String("\n") ? text.substring(0, text.length() - 1) : text;}

mlc::String strip_trailing_semicolon(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String(";") ? text.substring(0, text.length() - 1) : text;}

std::shared_ptr<cpp_ast::CppStmt> cpp_stmt_from_string_output(mlc::String output) noexcept{
mlc::String without_newline = strip_trailing_newline(output);
mlc::String without_semicolon = strip_trailing_semicolon(without_newline);
return emit_helpers::make_expression_cpp_statement(emit_helpers::make_identifier_cpp_expression(without_semicolon));
}

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> cpp_stmts_from_string_output(mlc::String output) noexcept{return output.length() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{} : mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{cpp_stmt_from_string_output(output)};}

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < statements.size()){
{
parts.push_back(cpp_printer::print_statement(statements[index]) + mlc::String("\n"));
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String gen_expr_string_for_stmt_bridge(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return expr_visitor_cpp::eval_expr_cpp_as_string(expression, context, gen_stmts_string_fn);}

context::GenStmtResult stmt_string_via_eval(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return stmt_eval::eval_stmt_with_try(statement, context, try_counter, [gen_stmts_string_fn](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr_string_for_stmt_bridge(expression, codegen_context, gen_stmts_string_fn); });}

stmt_visitor_cpp::GenStmtCppResult stmt_via_string_bridge(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{
context::GenStmtResult string_result = stmt_string_via_eval(statement, context, try_counter, gen_stmts_string_fn);
return stmt_visitor_cpp::GenStmtCppResult{cpp_stmt_from_string_output(string_result.output), string_result.next_try};
}

stmt_visitor_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return std::visit(overloaded{
  [&](const SStmtBreak& sstmtbreak) -> stmt_visitor_cpp::GenStmtCppResult { auto [_w0] = sstmtbreak; return stmt_visitor_cpp::GenStmtCppResult{emit_helpers::make_break_cpp_statement(), try_counter}; },
  [&](const SStmtContinue& sstmtcontinue) -> stmt_visitor_cpp::GenStmtCppResult { auto [_w0] = sstmtcontinue; return stmt_visitor_cpp::GenStmtCppResult{emit_helpers::make_continue_cpp_statement(), try_counter}; },
  [&](const SStmtReturn& sstmtreturn) -> stmt_visitor_cpp::GenStmtCppResult { auto [expression, _w0] = sstmtreturn; return [&]() -> stmt_visitor_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [_w0, _w1, _w2] = _v_sexprquestion; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); } return stmt_visitor_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_fn(expression, context)), try_counter}; }(); },
  [&](const SStmtLet& sstmtlet) -> stmt_visitor_cpp::GenStmtCppResult { auto [name, _w0, value, _w1, _w2] = sstmtlet; return [&]() -> stmt_visitor_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*value)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*value)._); auto [_w0, _w1, _w2] = _v_sexprquestion; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); } if (std::holds_alternative<semantic_ir::SExprArray>((*value)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*value)._); auto [_w0, _w1, _w2] = _v_sexprarray; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); } if (std::holds_alternative<semantic_ir::SExprMethod>((*value)._)) { auto _v_sexprmethod = std::get<semantic_ir::SExprMethod>((*value)._); auto [_w0, method_name, _w1, _w2, _w3, _w4] = _v_sexprmethod; return method_name == mlc::String("new") ? stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn) : stmt_visitor_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_fn(value, context)), try_counter}; } return stmt_visitor_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_fn(value, context)), try_counter}; }(); },
  [&](const SStmtLetPat& sstmtletpat) -> stmt_visitor_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = sstmtletpat; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); },
  [&](const SStmtLetConst& sstmtletconst) -> stmt_visitor_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = sstmtletconst; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); },
  [&](const SStmtExpr& sstmtexpr) -> stmt_visitor_cpp::GenStmtCppResult { auto [expression, _w0] = sstmtexpr; return [&]() -> stmt_visitor_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [_w0, _w1, _w2] = _v_sexprquestion; return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); } return stmt_via_string_bridge(statement, context, try_counter, gen_stmts_string_fn); }(); }
}, (*statement)._);}

stmt_visitor_cpp::StmtsCppFoldState stmts_cpp_fold_step(stmt_visitor_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{
stmt_visitor_cpp::GenStmtCppResult statement_result = eval_stmt_cpp(statement, fold_state.codegen_context, fold_state.next_try, gen_expr_cpp_fn, gen_stmts_string_fn);
mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements = fold_state.statements;
statements.push_back(statement_result.statement);
return stmt_visitor_cpp::StmtsCppFoldState{statements, context::update_context_from_statement(statement, fold_state.codegen_context), statement_result.next_try};
}

stmt_visitor_cpp::StmtsCppFoldState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return statements.fold(stmt_visitor_cpp::StmtsCppFoldState{{}, context, try_counter}, [gen_expr_cpp_fn, gen_stmts_string_fn](stmt_visitor_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement) mutable { return stmts_cpp_fold_step(fold_state, statement, gen_expr_cpp_fn, gen_stmts_string_fn); });}

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> eval_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return eval_stmts_cpp_with_try(statements, context, 0, gen_expr_cpp_fn, gen_stmts_string_fn).statements;}

mlc::String eval_stmts_cpp_as_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_cpp_fn, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts_string_fn) noexcept{return print_cpp_statements(eval_stmts_cpp(statements, context, gen_expr_cpp_fn, gen_stmts_string_fn));}

} // namespace stmt_visitor_cpp

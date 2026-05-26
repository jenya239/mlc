#include "stmt_cpp.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "stmt_eval.hpp"
#include "eval.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_cpp {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace cpp_printer;
using namespace emit_helpers;
using namespace context;
using namespace cpp_naming;
using namespace stmt_eval;
using namespace eval;
using namespace expr_visitor_cpp;

struct StmtsCppFoldState {mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements;context::CodegenContext codegen_context;int next_try;};

struct GenStmtCppResult {std::shared_ptr<cpp_ast::CppStmt> statement;int next_try;};

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_stmts_cpp_as_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppStmt> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept;

mlc::String strip_trailing_newline(mlc::String text) noexcept;

mlc::String strip_trailing_semicolon(mlc::String text) noexcept;

mlc::String gen_expr_string_for_stmt_bridge(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

stmt_cpp::GenStmtCppResult stmt_via_string_bridge(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::StmtsCppFoldState stmts_cpp_fold_step(stmt_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement) noexcept;

stmt_cpp::StmtsCppFoldState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return eval_stmts_cpp_with_try(statements, context, 0).statements;}

mlc::String gen_stmts_cpp_as_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return print_cpp_statements(gen_stmts_cpp(statements, context));}

std::shared_ptr<cpp_ast::CppStmt> cpp_stmt_from_string_output(mlc::String output) noexcept{
mlc::String without_newline = strip_trailing_newline(output);
mlc::String without_semicolon = strip_trailing_semicolon(without_newline);
return emit_helpers::make_expression_cpp_statement(emit_helpers::make_identifier_cpp_expression(without_semicolon));
}

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> cpp_stmts_from_string_output(mlc::String output) noexcept{return output.length() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements = {};
  statements.push_back(cpp_stmt_from_string_output(output));
  return statements;
 }();}

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

mlc::String strip_trailing_newline(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String("\n") ? text.substring(0, text.length() - 1) : text;}

mlc::String strip_trailing_semicolon(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String(";") ? text.substring(0, text.length() - 1) : text;}

mlc::String gen_expr_string_for_stmt_bridge(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp_as_string(expression, context, gen_stmts_str);}

stmt_cpp::GenStmtCppResult stmt_via_string_bridge(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept{
context::GenStmtResult string_result = stmt_eval::eval_stmt_with_try(statement, context, try_counter, [](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr_string_for_stmt_bridge(expression, codegen_context); });
return stmt_cpp::GenStmtCppResult{cpp_stmt_from_string_output(string_result.output), string_result.next_try};
}

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SStmtBreak& sstmtbreak) -> stmt_cpp::GenStmtCppResult { auto [_w0] = sstmtbreak; return stmt_cpp::GenStmtCppResult{emit_helpers::make_break_cpp_statement(), try_counter}; },
  [&](const SStmtContinue& sstmtcontinue) -> stmt_cpp::GenStmtCppResult { auto [_w0] = sstmtcontinue; return stmt_cpp::GenStmtCppResult{emit_helpers::make_continue_cpp_statement(), try_counter}; },
  [&](const SStmtReturn& sstmtreturn) -> stmt_cpp::GenStmtCppResult { auto [expression, _w0] = sstmtreturn; return [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [_w0, _w1, _w2] = _v_sexprquestion; return stmt_via_string_bridge(statement, context, try_counter); } return stmt_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(eval::gen_expr_cpp(expression, context)), try_counter}; }(); },
  [&](const SStmtLet& sstmtlet) -> stmt_cpp::GenStmtCppResult { auto [name, _w0, value, _w1, _w2] = sstmtlet; return [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*value)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*value)._); auto [_w0, _w1, _w2] = _v_sexprquestion; return stmt_via_string_bridge(statement, context, try_counter); } if (std::holds_alternative<semantic_ir::SExprArray>((*value)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*value)._); auto [_w0, _w1, _w2] = _v_sexprarray; return stmt_via_string_bridge(statement, context, try_counter); } if (std::holds_alternative<semantic_ir::SExprMethod>((*value)._)) { auto _v_sexprmethod = std::get<semantic_ir::SExprMethod>((*value)._); auto [_w0, method_name, _w1, _w2, _w3, _w4] = _v_sexprmethod; return method_name == mlc::String("new") ? stmt_via_string_bridge(statement, context, try_counter) : stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), eval::gen_expr_cpp(value, context)), try_counter}; } return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), eval::gen_expr_cpp(value, context)), try_counter}; }(); },
  [&](const SStmtLetPat& sstmtletpat) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = sstmtletpat; return stmt_via_string_bridge(statement, context, try_counter); },
  [&](const SStmtLetConst& sstmtletconst) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = sstmtletconst; return stmt_via_string_bridge(statement, context, try_counter); },
  [&](const SStmtExpr& sstmtexpr) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = sstmtexpr; return stmt_via_string_bridge(statement, context, try_counter); }
}, (*statement)._);}

stmt_cpp::StmtsCppFoldState stmts_cpp_fold_step(stmt_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement) noexcept{
stmt_cpp::GenStmtCppResult statement_result = eval_stmt_cpp(statement, fold_state.codegen_context, fold_state.next_try);
mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements = fold_state.statements;
statements.push_back(statement_result.statement);
return stmt_cpp::StmtsCppFoldState{statements, context::update_context_from_statement(statement, fold_state.codegen_context), statement_result.next_try};
}

stmt_cpp::StmtsCppFoldState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept{return statements.fold(stmt_cpp::StmtsCppFoldState{{}, context, try_counter}, [](stmt_cpp::StmtsCppFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement) mutable { return stmts_cpp_fold_step(fold_state, statement); });}

} // namespace stmt_cpp

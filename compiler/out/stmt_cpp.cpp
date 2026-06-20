#include "stmt_cpp.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "let_pat_cpp.hpp"
#include "statement_context.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_cpp {

using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace let_pat_cpp;
using namespace statement_context;
using namespace expr_visitor_cpp;

struct StmtsCppAccumState {mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements;int next_try;context::CodegenContext codegen_context;};

struct GenStmtCppResult {std::shared_ptr<cpp_ast::CppStatement> statement;int next_try;context::CodegenContext codegen_context;};

mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

stmt_cpp::GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept;

mlc::String strip_trailing_newline(mlc::String text) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept;

stmt_cpp::GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;

bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return print_cpp_statements(gen_stmts_cpp(statements, context));}

std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_for_cpp_codegen);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{return eval_stmts_cpp_with_try(statements, self, 0).statements;}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return CodegenContext_gen_stmts_cpp(context, statements);}

stmt_cpp::GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
stmt_cpp::StmtsCppAccumState accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
return stmt_cpp::GenStmtsCppWithContext{accumulated.statements, accumulated.codegen_context};
}

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept{return cpp_stmt_fragment_from_string_output(output);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept{return output.length() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(cpp_stmt_from_string_output(output));
  return statements;
 }();}

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{return statements.map([](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print_cpp_statement_line(statement); }).join(mlc::String(""));}

mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppStatementFragment>((*statement)._)) { auto _v_cppstatementfragment = std::get<cpp_ast::CppStatementFragment>((*statement)._); auto [fragment] = _v_cppstatementfragment; return fragment.length() >= 1 && fragment.char_at(fragment.length() - 1) == mlc::String("\n") ? fragment : fragment + mlc::String("\n"); } return cpp_ast::print_statement(statement) + mlc::String("\n"); }();}

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept{return emit_helpers::make_fragment_cpp_statement(strip_trailing_newline(output));}

mlc::String strip_trailing_newline(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String("\n") ? text.substring(0, text.length() - 1) : text;}

std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::HashMap<") + key_type_cpp + mlc::String(", ") + value_type_cpp + mlc::String(">")), {}));}

std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [type_name, type_args] = _v_tgeneric; return type_name == mlc::String("Map") && type_args.size() == 2 ? gen_typed_hash_map_empty_cpp(type_gen::sem_type_to_cpp(context, type_args[0]), type_gen::sem_type_to_cpp(context, type_args[1])) : fallback; } return fallback; }();}

stmt_cpp::GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> iterator_cpp = gen_expr_cpp_for_stmt_codegen(iterator, context);
stmt_cpp::StmtsCppAccumState body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_for_cpp_statement(cpp_naming::cpp_safe(variable_name), iterator_cpp, body_accumulated.statements), body_accumulated.next_try, body_accumulated.codegen_context};
}

stmt_cpp::GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
stmt_cpp::StmtsCppAccumState body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_while_cpp_statement(condition_cpp, emit_helpers::make_block_cpp_statement(body_accumulated.statements)), body_accumulated.next_try, body_accumulated.codegen_context};
}

bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return true; } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*else_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*else_expression)._); auto [_w0, result_expression, _w1, _w2] = _v_semanticexpressionblock; return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return true; } return false; }(); } return false; }();}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output = block_statements;
return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return output; } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  output.push_back(gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, final_context, try_counter).statement);
  return output;
 }(); } return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  output.push_back(emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(result_expression, final_context)));
  return output;
 }(); }();
}

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*branch_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*branch_expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  stmt_cpp::StmtsCppAccumState accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
  return emit_helpers::make_block_cpp_statement(append_block_trailing_cpp(accumulated.statements, result_expression, final_context, accumulated.next_try));
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*branch_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*branch_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter).statement; } return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(branch_expression, context))}); }();}

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return emit_helpers::make_block_cpp_statement({}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [condition, then_expression, nested_else_expression, semantic_type, _w0] = _v_semanticexpressionif; return gen_unit_if_statement_cpp(condition, then_expression, nested_else_expression, semantic_type, context, try_counter).statement; } return unit_if_branch_block_cpp(else_expression, context, try_counter); }();}

stmt_cpp::GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_if_cpp_statement(condition_cpp, unit_if_branch_block_cpp(then_expression, context, try_counter), unit_if_else_branch_cpp(else_expression, context, try_counter)), try_counter, context};
}

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SemanticStatementBreak& semanticstatementbreak) -> stmt_cpp::GenStmtCppResult { auto [_w0] = semanticstatementbreak; return stmt_cpp::GenStmtCppResult{emit_helpers::make_break_cpp_statement(), try_counter, context}; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> stmt_cpp::GenStmtCppResult { auto [_w0] = semanticstatementcontinue; return stmt_cpp::GenStmtCppResult{emit_helpers::make_continue_cpp_statement(), try_counter, context}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> stmt_cpp::GenStmtCppResult { auto [expression, _w0] = semanticstatementreturn; return [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [_w0, _w1, _w2] = _v_semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter + 1, context}; } return stmt_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context}; }(); },
  [&](const SemanticStatementLet& semanticstatementlet) -> stmt_cpp::GenStmtCppResult { auto [name, _w0, value, value_type, _w1] = semanticstatementlet; return [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*value)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*value)._); auto [_w0, _w1, _w2] = _v_semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter + 1, context}; } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*value)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*value)._); auto [_w0, _w1, _w2] = _v_semanticexpressionarray; return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*value)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*value)._); auto [map_object, method_name, _w0, _w1, _w2, _w3] = _v_semanticexpressionmethod; return method_name == mlc::String("new") ? [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*map_object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*map_object)._); auto [object_name, _w0, _w1] = _v_semanticexpressionident; return object_name == mlc::String("Map") ? stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_map_new_let_value_cpp(value_type, context, gen_expr_cpp_for_stmt_codegen(value, context))), try_counter, context} : stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; } return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; }() : stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; } return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; }(); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> stmt_cpp::GenStmtCppResult { auto [pat, _w0, value, value_type, has_else, else_body, _w1] = semanticstatementletpattern; return [&]() -> stmt_cpp::GenStmtCppResult { 
  let_pat_cpp::GenLetPatternCppResult pattern_parsed = let_pat_cpp::gen_let_pattern_cpp_statement(pat, value, value_type, has_else, else_body, context, gen_expr_cpp_for_stmt_codegen);
  return stmt_cpp::GenStmtCppResult{pattern_parsed.statement, try_counter, pattern_parsed.codegen_context};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> stmt_cpp::GenStmtCppResult { auto [name, value, _w0, _w1] = semanticstatementletconst; return stmt_cpp::GenStmtCppResult{emit_helpers::make_constexpr_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> stmt_cpp::GenStmtCppResult { auto [expression, _w0] = semanticstatementexpr; return [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [_w0, _w1, _w2] = _v_semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter + 1, context}; } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, body_statements, _w0, _w1] = _v_semanticexpressionwhile; return gen_while_statement_cpp(condition, body_statements, context, try_counter); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable_name, iterator, body_statements, _w0, _w1] = _v_semanticexpressionfor; return gen_for_statement_cpp(variable_name, iterator, body_statements, context, try_counter); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return if_statement_has_unit_else(else_expression) ? gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter) : [&]() -> stmt_cpp::GenStmtCppResult { if (std::holds_alternative<registry::TUnit>((*semantic_type))) {  return gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter); } return stmt_cpp::GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context}; }(); } return stmt_cpp::GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context}; }(); }
}, (*statement)._);}

stmt_cpp::StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output_statements = {};
int next_try = try_counter;
context::CodegenContext codegen_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
std::shared_ptr<semantic_ir::SemanticStatement> statement = statements[index];
stmt_cpp::GenStmtCppResult statement_parsed = eval_stmt_cpp(statement, codegen_context, next_try);
output_statements.push_back(statement_parsed.statement);
codegen_context = statement_parsed.codegen_context;
context::mutate_context_from_statement(statement, codegen_context);
next_try = statement_parsed.next_try;
index = index + 1;
}
}
return stmt_cpp::StmtsCppAccumState{output_statements, next_try, codegen_context};
}

} // namespace stmt_cpp

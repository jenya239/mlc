#include "stmt_eval.hpp"

#include "semantic_ir.hpp"
#include "let_pat.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace stmt_eval {

using namespace semantic_ir;
using namespace let_pat;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace expression_support;
using namespace type_gen;
using namespace literals;
using namespace statement_context;
using namespace expr;

struct StmtsFoldState {context::GenStmtsResult partial_result;context::CodegenContext codegen_context;};

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_if_else_suffix(std::shared_ptr<semantic_ir::SExpr> else_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SExpr> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

stmt_eval::StmtsFoldState stmts_fold_step(stmt_eval::StmtsFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtsResult eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBlock>((*value)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*value)._); auto [inner_statements, result_expression, _w0, _w1] = _v_sexprblock; return inner_statements.size() == 0 ? constexpr_binding_value_code(result_expression, binding_semantic_type, context, gen_expr_fn) : gen_expr_fn(value, context); } if (std::holds_alternative<semantic_ir::SExprIf>((*value)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*value)._); auto [condition, then_branch, else_branch, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String cond_code = gen_expr_fn(condition, context);
  mlc::String then_code = gen_expr_fn(then_branch, context);
  mlc::String else_code = expression_support::generate_conditional_else_with_empty_array_coercion(then_branch, else_branch, binding_semantic_type, context, gen_expr_fn);
  return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*binding_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*binding_semantic_type)); auto [type_name] = _v_tnamed; return [&]() -> mlc::String { 
  mlc::String type_cpp = context::CodegenContext_resolve(context, type_name);
  return type_cpp == mlc::String("auto") ? expr::ternary_conditional(cond_code, then_code, else_code) : mlc::String("[&]() -> ") + type_cpp + mlc::String(" {\nif (") + cond_code + mlc::String(") {\nreturn ") + then_code + mlc::String(";\n} else {\nreturn ") + else_code + mlc::String(";\n}\n}()");
 }(); } return expr::ternary_conditional(cond_code, then_code, else_code); }();
 }(); } return gen_expr_fn(value, context); }();}

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::String inner_code = gen_expr_fn(inner_expr, context);
return expr::try_unwrap_result_block(try_identifier, inner_code, success_line);
}

mlc::String eval_stmt_if_else_suffix(std::shared_ptr<semantic_ir::SExpr> else_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*else_expression)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*else_expression)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SExprBlock>((*else_expression)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*else_expression)._); auto [statements, result_expression, _w0, _w1] = _v_sexprblock; return statements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expression)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expression)._); auto [_w0, _w1] = _v_sexprunit; return mlc::String(""); } return expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); }() : expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SExprIf>((*else_expression)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*else_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return expr::else_fragment_raw(eval_stmt_if_nested(else_expression, context, gen_expr_fn)); } return expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); }();}

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SExpr> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIf>((*if_expression)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*if_expression)._); auto [condition_expression, then_expression, else_expression, _w0, _w1] = _v_sexprif; return [&]() -> mlc::String { 
  mlc::String opening = expr::if_brace_block(gen_expr_fn(condition_expression, context), eval_block_body(then_expression, context, gen_expr_fn));
  mlc::String else_suffix = eval_stmt_if_else_suffix(else_expression, context, gen_expr_fn);
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(opening, else_suffix));
 }(); } return expr::if_always_true_block(eval_block_body(if_expression, context, gen_expr_fn)); }();}

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBlock>((*expr)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expr)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = eval_stmts_str(statements, context, gen_expr_fn);
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIf>((*result_expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*result_expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return statements_code + eval_stmt_if_nested(result_expr, final_context, gen_expr_fn); } if (std::holds_alternative<semantic_ir::SExprUnit>((*result_expr)._)) { auto _v_sexprunit = std::get<semantic_ir::SExprUnit>((*result_expr)._); auto [_w0, _w1] = _v_sexprunit; return statements_code; } return [&]() -> mlc::String { 
  mlc::String result_code = gen_expr_fn(result_expr, final_context);
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); }();
 }(); } if (std::holds_alternative<semantic_ir::SExprIf>((*expr)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expr)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return eval_stmt_if_nested(expr, context, gen_expr_fn); } return expr::suffix_semicolon_newline(gen_expr_fn(expr, context)); }();}

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprBin>((*expression)._)) { auto _v_sexprbin = std::get<semantic_ir::SExprBin>((*expression)._); auto [operation, left_expr, right_expr, _w0, _w1] = _v_sexprbin; return operation == mlc::String("=") ? expr::assignment_statement(gen_expr_fn(left_expr, context), gen_expr_fn(right_expr, context)) : expr::expression_operation_statement(operation, gen_expr_fn(left_expr, context), gen_expr_fn(right_expr, context)); } if (std::holds_alternative<semantic_ir::SExprIf>((*expression)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_sexprif; return eval_stmt_if_nested(expression, context, gen_expr_fn); } if (std::holds_alternative<semantic_ir::SExprWhile>((*expression)._)) { auto _v_sexprwhile = std::get<semantic_ir::SExprWhile>((*expression)._); auto [condition, statements, _w0, _w1] = _v_sexprwhile; return expr::while_loop_statement(gen_expr_fn(condition, context), eval_stmts_str(statements, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SExprFor>((*expression)._)) { auto _v_sexprfor = std::get<semantic_ir::SExprFor>((*expression)._); auto [variable, iterator, statements, _w0, _w1] = _v_sexprfor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr_fn(iterator, context), eval_stmts_str(statements, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SExprWith>((*expression)._)) { auto _v_sexprwith = std::get<semantic_ir::SExprWith>((*expression)._); auto [resource, binder, stmts, _w0, _w1] = _v_sexprwith; return expr::with_block_statement(gen_expr_fn(resource, context), cpp_naming::cpp_safe(binder), eval_stmts_str(stmts, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SExprBlock>((*expression)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expression)._); auto [statements, result_expr, _w0, _w1] = _v_sexprblock; return [&]() -> mlc::String { 
  mlc::String statements_code = eval_stmts_str(statements, context, gen_expr_fn);
  mlc::String result_code = gen_expr_fn(result_expr, statement_context::stmts_final_ctx(statements, context));
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); } return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }();}

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return elements.map([gen_expr_fn, context](std::shared_ptr<semantic_ir::SExpr> element)  { return gen_expr_fn(element, context); }).join(mlc::String(", "));}

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SStmtLet& sstmtlet) -> context::GenStmtResult { auto [name, _w0, value, value_type, _w1] = sstmtlet; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  int next_counter = try_counter + 1;
  context::GenStmtResult result = [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*value)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*value)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), try_identifier), gen_expr_fn), next_counter}; } if (std::holds_alternative<semantic_ir::SExprArray>((*value)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*value)._); auto [elements, array_type, _w0] = _v_sexprarray; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*array_type))) { auto _v_tarray = std::get<registry::TArray>((*array_type)); auto [inner_type] = _v_tarray; return [&]() -> mlc::String { 
  mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  return elements.size() == 0 ? expr::typed_array_empty_or_untyped_empty(element_cpp) : element_cpp == mlc::String("auto") ? expr::array_literal(eval_elements_code(elements, context, gen_expr_fn)) : expr::typed_array_braced_initializer(element_cpp, eval_elements_code(elements, context, gen_expr_fn));
 }(); } return elements.size() == 0 ? expr::array_string_fallback_empty() : expr::constructor_call_braces(mlc::String("mlc::Array"), eval_elements_code(elements, context, gen_expr_fn)); }();
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
 }(); } if (std::holds_alternative<semantic_ir::SExprMethod>((*value)._)) { auto _v_sexprmethod = std::get<semantic_ir::SExprMethod>((*value)._); auto [map_object, method_name, _w0, _w1, _w2] = _v_sexprmethod; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = method_name == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*map_object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*map_object)._); auto [object_name, _w0, _w1] = _v_sexprident; return object_name == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [type_name, type_args] = _v_tgeneric; return type_name == mlc::String("Map") && type_args.size() == 2 ? expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_args[0]), type_gen::sem_type_to_cpp(context, type_args[1])) : gen_expr_fn(value, context); } return gen_expr_fn(value, context); }() : gen_expr_fn(value, context); } return gen_expr_fn(value, context); }() : gen_expr_fn(value, context);
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
 }(); } return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr_fn(value, context)), try_counter}; }();
  return result;
 }(); },
  [&](const SStmtLetPat& sstmtletpat) -> context::GenStmtResult { auto [pat, _w0, value, value_type, has_else, else_body, _w1] = sstmtletpat; return context::GenStmtResult{let_pat::gen_let_pattern_statement(pat, value, value_type, has_else, else_body, context, gen_expr_fn), try_counter}; },
  [&](const SStmtLetConst& sstmtletconst) -> context::GenStmtResult { auto [name, value, binding_semantic_type, _w0] = sstmtletconst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), constexpr_binding_value_code(value, binding_semantic_type, context, gen_expr_fn)), try_counter}; },
  [&](const SStmtExpr& sstmtexpr) -> context::GenStmtResult { auto [expression, _w0] = sstmtexpr; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::discard_try_ok_field0_statement(try_identifier), gen_expr_fn), try_counter + 1};
 }(); } return context::GenStmtResult{eval_stmt_expr(expression, context, gen_expr_fn), try_counter}; }(); },
  [&](const SStmtBreak& sstmtbreak) -> context::GenStmtResult { auto [_w0] = sstmtbreak; return context::GenStmtResult{expr::break_line(), try_counter}; },
  [&](const SStmtContinue& sstmtcontinue) -> context::GenStmtResult { auto [_w0] = sstmtcontinue; return context::GenStmtResult{expr::continue_line(), try_counter}; },
  [&](const SStmtReturn& sstmtreturn) -> context::GenStmtResult { auto [expression, _w0] = sstmtreturn; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SExprQuestion>((*expression)._)) { auto _v_sexprquestion = std::get<semantic_ir::SExprQuestion>((*expression)._); auto [inner_expr, _w0, _w1] = _v_sexprquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_ret_") + mlc::to_string(try_counter);
  return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier), gen_expr_fn), try_counter + 1};
 }(); } return context::GenStmtResult{expr::return_line(gen_expr_fn(expression, context)), try_counter}; }(); }
}, (*stmt)._);}

stmt_eval::StmtsFoldState stmts_fold_step(stmt_eval::StmtsFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
context::GenStmtResult statement_result = eval_stmt_with_try(statement, fold_state.codegen_context, fold_state.partial_result.next_try, gen_expr_fn);
return stmt_eval::StmtsFoldState{context::GenStmtsResult_append_stmt(fold_state.partial_result, statement_result), context::update_context_from_statement(statement, fold_state.codegen_context)};
}

context::GenStmtsResult eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{return statements.fold(stmt_eval::StmtsFoldState{context::GenStmtsResult{mlc::String(""), try_counter}, context}, [gen_expr_fn](stmt_eval::StmtsFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement)  { return stmts_fold_step(fold_state, statement, gen_expr_fn); }).partial_result;}

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
context::GenStmtsResult result = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
return result.code;
}

} // namespace stmt_eval

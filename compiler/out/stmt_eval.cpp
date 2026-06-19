#include "stmt_eval.hpp"

#include "semantic_ir.hpp"
#include "let_pat.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
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
using namespace expr;

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_if_else_suffix(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*value)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*value)._); auto [inner_statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return inner_statements.size() == 0 ? constexpr_binding_value_code(result_expression, binding_semantic_type, context, gen_expr_fn) : gen_expr_fn(value, context); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*value)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*value)._); auto [condition, then_branch, else_branch, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String cond_code = gen_expr_fn(condition, context);
  mlc::String then_code = gen_expr_fn(then_branch, context);
  mlc::String else_code = expression_support::generate_conditional_else_with_empty_array_coercion(then_branch, else_branch, binding_semantic_type, context, gen_expr_fn);
  return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*binding_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*binding_semantic_type)); auto [type_name] = _v_tnamed; return [&]() -> mlc::String { 
  mlc::String type_cpp = context::CodegenContext_resolve(context, type_name);
  return type_cpp == mlc::String("auto") ? expr::ternary_conditional(cond_code, then_code, else_code) : mlc::String("[&]() -> ") + type_cpp + mlc::String(" {\nif (") + cond_code + mlc::String(") {\nreturn ") + then_code + mlc::String(";\n} else {\nreturn ") + else_code + mlc::String(";\n}\n}()");
 }(); } return expr::ternary_conditional(cond_code, then_code, else_code); }();
 }(); } return gen_expr_fn(value, context); }();}

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::String inner_code = gen_expr_fn(inner_expression, context);
return expr::try_unwrap_result_block(try_identifier, inner_code, success_line);
}

mlc::String eval_stmt_if_else_suffix(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*else_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*else_expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return statements.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } return expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); }() : expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return expr::else_fragment_raw(eval_stmt_if_nested(else_expression, context, gen_expr_fn)); } return expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn)); }();}

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*if_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*if_expression)._); auto [condition_expression, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String opening = expr::if_brace_block(gen_expr_fn(condition_expression, context), eval_block_body(then_expression, context, gen_expr_fn));
  mlc::String else_suffix = eval_stmt_if_else_suffix(else_expression, context, gen_expr_fn);
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(opening, else_suffix));
 }(); } return expr::if_always_true_block(eval_block_body(if_expression, context, gen_expr_fn)); }();}

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> mlc::String { 
  context::GenStmtsWithContext statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
  mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
  context::CodegenContext final_context = statements_with_context.codegen_context;
  return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return statements_code + eval_stmt_if_nested(result_expression, final_context, gen_expr_fn); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return statements_code; } return [&]() -> mlc::String { 
  mlc::String result_code = gen_expr_fn(result_expression, final_context);
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); }();
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return eval_stmt_if_nested(expression, context, gen_expr_fn); } return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }();}

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left_expression, right_expression, _w0, _w1] = _v_semanticexpressionbin; return operation == mlc::String("=") ? expr::assignment_statement(gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context)) : expr::expression_operation_statement(operation, gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return eval_stmt_if_nested(expression, context, gen_expr_fn); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, _w0, _w1] = _v_semanticexpressionwhile; return expr::while_loop_statement(gen_expr_fn(condition, context), eval_stmts_str(statements, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable, iterator, statements, _w0, _w1] = _v_semanticexpressionfor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr_fn(iterator, context), eval_stmts_str(statements, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [resource, binder, with_statements, _w0, _w1] = _v_semanticexpressionwith; return expr::with_block_statement(gen_expr_fn(resource, context), cpp_naming::cpp_safe(binder), eval_stmts_str(with_statements, context, gen_expr_fn)); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> mlc::String { 
  context::GenStmtsWithContext statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
  mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
  mlc::String result_code = gen_expr_fn(result_expression, statements_with_context.codegen_context);
  return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
 }(); } return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }();}

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return elements.map([gen_expr_fn, context](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return gen_expr_fn(element, context); }).join(mlc::String(", "));}

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> context::GenStmtResult { auto [name, _w0, value, value_type, _w1] = semanticstatementlet; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  int next_counter = try_counter + 1;
  context::GenStmtResult statement_parsed = [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*value)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*value)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, try_identifier, expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), try_identifier), gen_expr_fn), next_counter, context}; } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*value)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*value)._); auto [elements, array_type, _w0] = _v_semanticexpressionarray; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*array_type))) { auto _v_tarray = std::get<registry::TArray>((*array_type)); auto [inner_type] = _v_tarray; return [&]() -> mlc::String { 
  mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  return elements.size() == 0 ? expr::typed_array_empty_or_untyped_empty(element_cpp) : element_cpp == mlc::String("auto") ? expr::array_literal(eval_elements_code(elements, context, gen_expr_fn)) : expr::typed_array_braced_initializer(element_cpp, eval_elements_code(elements, context, gen_expr_fn));
 }(); } return elements.size() == 0 ? expr::array_string_fallback_empty() : expr::constructor_call_braces(mlc::String("mlc::Array"), eval_elements_code(elements, context, gen_expr_fn)); }();
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter, context};
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*value)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*value)._); auto [map_object, method_name, _w0, _w1, _w2, _w3] = _v_semanticexpressionmethod; return [&]() -> context::GenStmtResult { 
  mlc::String value_code = method_name == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*map_object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*map_object)._); auto [object_name, _w0, _w1] = _v_semanticexpressionident; return object_name == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [type_name, type_args] = _v_tgeneric; return type_name == mlc::String("Map") && type_args.size() == 2 ? expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_args[0]), type_gen::sem_type_to_cpp(context, type_args[1])) : gen_expr_fn(value, context); } return gen_expr_fn(value, context); }() : gen_expr_fn(value, context); } return gen_expr_fn(value, context); }() : gen_expr_fn(value, context);
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter, context};
 }(); } return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr_fn(value, context)), try_counter, context}; }();
  return statement_parsed;
 }(); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> context::GenStmtResult { auto [pattern, _w0, value, value_type, has_else, else_body, _w1] = semanticstatementletpattern; return [&]() -> context::GenStmtResult { 
  let_pat::GenLetPatternResult pattern_parsed = let_pat::gen_let_pattern_statement(pattern, value, value_type, has_else, else_body, context, gen_expr_fn);
  return context::GenStmtResult{pattern_parsed.output, try_counter, pattern_parsed.codegen_context};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> context::GenStmtResult { auto [name, value, binding_semantic_type, _w0] = semanticstatementletconst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), constexpr_binding_value_code(value, binding_semantic_type, context, gen_expr_fn)), try_counter, context}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> context::GenStmtResult { auto [expression, _w0] = semanticstatementexpr; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_") + mlc::to_string(try_counter);
  return context::GenStmtResult{eval_try_unwrap(inner_expression, context, try_identifier, expr::discard_try_ok_field0_statement(try_identifier), gen_expr_fn), try_counter + 1, context};
 }(); } return context::GenStmtResult{eval_stmt_expr(expression, context, gen_expr_fn), try_counter, context}; }(); },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> context::GenStmtResult { auto [_w0] = semanticstatementbreak; return context::GenStmtResult{expr::break_line(), try_counter, context}; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> context::GenStmtResult { auto [_w0] = semanticstatementcontinue; return context::GenStmtResult{expr::continue_line(), try_counter, context}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> context::GenStmtResult { auto [expression, _w0] = semanticstatementreturn; return [&]() -> context::GenStmtResult { if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, _w0, _w1] = _v_semanticexpressionquestion; return [&]() -> context::GenStmtResult { 
  mlc::String try_identifier = mlc::String("__try_ret_") + mlc::to_string(try_counter);
  return context::GenStmtResult{eval_try_unwrap(inner_expression, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier), gen_expr_fn), try_counter + 1, context};
 }(); } return context::GenStmtResult{expr::return_line(gen_expr_fn(expression, context)), try_counter, context}; }(); }
}, (*statement)._);}

context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
context::GenStmtsResult statements_parsed = context::GenStmtsResult{{}, try_counter};
context::CodegenContext codegen_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
std::shared_ptr<semantic_ir::SemanticStatement> statement = statements[index];
context::GenStmtResult statement_parsed = eval_stmt_with_try(statement, codegen_context, statements_parsed.next_try, gen_expr_fn);
statements_parsed = context::GenStmtsResult_append_stmt(statements_parsed, statement_parsed);
codegen_context = statement_parsed.codegen_context;
context::mutate_context_from_statement(statement, codegen_context);
index = index + 1;
}
}
return context::GenStmtsWithContext{statements_parsed, codegen_context};
}

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return context::GenStmtsResult_joined_code(eval_stmts_str_with_try(statements, context, 0, gen_expr_fn).statements_parsed);}

} // namespace stmt_eval

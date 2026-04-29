#define main mlc_user_main
#include "stmt_eval.hpp"

namespace stmt_eval {

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
auto inner_code = gen_expr_fn(inner_expr, context);
return expr::try_unwrap_result_block(try_identifier, inner_code, success_line);
}
mlc::String eval_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return [&]() {
auto statements_code = eval_stmts_str(statements, context, gen_expr_fn);
auto final_context = statement_context::stmts_final_ctx(statements, context);
return std::visit(overloaded{[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return (statements_code + eval_stmt_expr(result_expr, final_context, gen_expr_fn)); },
[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return statements_code; },
[&](const auto& __v) { return [&]() {
auto result_code = gen_expr_fn(result_expr, final_context);
return ((result_code == literals::gen_unit_literal()) ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code));
}(); }
}, (*result_expr));
}(); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return eval_stmt_expr(expr, context, gen_expr_fn); },
[&](const auto& __v) { return expr::suffix_semicolon_newline(gen_expr_fn(expr, context)); }
}, (*expr));
}
mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprBin& sExprBin) { auto [operation, left_expr, right_expr, __3, __4] = sExprBin; return ((operation == mlc::String("=", 1)) ? expr::assignment_statement(gen_expr_fn(left_expr, context), gen_expr_fn(right_expr, context)) : expr::expression_operation_statement(operation, gen_expr_fn(left_expr, context), gen_expr_fn(right_expr, context))); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, __3, __4] = sExprIf; return [&]() {
auto output = expr::if_brace_block(gen_expr_fn(condition, context), eval_block_body(then_expr, context, gen_expr_fn));
auto else_string = std::visit(overloaded{[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return mlc::String("", 0); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return ((statements.length() == 0) ? std::visit(overloaded{[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return mlc::String("", 0); },
[&](const auto& __v) { return expr::else_brace_block(eval_block_body(else_expr, context, gen_expr_fn)); }
}, (*result_expr)) : expr::else_brace_block(eval_block_body(else_expr, context, gen_expr_fn))); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return expr::else_fragment_raw(eval_stmt_expr(else_expr, context, gen_expr_fn)); },
[&](const auto& __v) { return expr::else_brace_block(eval_block_body(else_expr, context, gen_expr_fn)); }
}, (*else_expr));
return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
}(); },
[&](const semantic_ir::SExprWhile& sExprWhile) { auto [condition, statements, __2, __3] = sExprWhile; return expr::while_loop_statement(gen_expr_fn(condition, context), eval_stmts_str(statements, context, gen_expr_fn)); },
[&](const semantic_ir::SExprFor& sExprFor) { auto [variable, iterator, statements, __3, __4] = sExprFor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr_fn(iterator, context), eval_stmts_str(statements, context, gen_expr_fn)); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return [&]() {
auto statements_code = eval_stmts_str(statements, context, gen_expr_fn);
auto result_code = gen_expr_fn(result_expr, statement_context::stmts_final_ctx(statements, context));
return ((result_code == literals::gen_unit_literal()) ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code));
}(); },
[&](const auto& __v) { return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }
}, (*expression));
}
mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < elements.length())) {
parts.push_back(gen_expr_fn(elements[index], context));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SStmtLet& sStmtLet) { auto [name, __1, value, value_type, __4] = sStmtLet; return [&]() {
auto try_identifier = (mlc::String("__try_", 6) + mlc::to_string(try_counter));
auto next_counter = (try_counter + 1);
auto result = std::visit(overloaded{[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), try_identifier), gen_expr_fn), next_counter}; },
[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, array_type, __2] = sExprArray; return [&]() {
auto value_code = ((elements.length() == 0) ? std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_type] = tArray; return expr::typed_array_empty(type_gen::sem_type_to_cpp(context, inner_type)); },
[&](const auto& __v) { return expr::array_string_fallback_empty(); }
}, (*array_type)) : expr::constructor_call_braces(mlc::String("mlc::Array", 10), eval_elements_code(elements, context, gen_expr_fn)));
return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
}(); },
[&](const semantic_ir::SExprMethod& sExprMethod) { auto [map_object, method_name, __2, __3, __4] = sExprMethod; return [&]() {
auto value_code = ((method_name == mlc::String("new", 3)) ? std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [object_name, __1, __2] = sExprIdent; return ((object_name == mlc::String("Map", 3)) ? std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [type_name, type_args] = tGeneric; return (((type_name == mlc::String("Map", 3)) && (type_args.length() == 2)) ? expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_args[0]), type_gen::sem_type_to_cpp(context, type_args[1])) : gen_expr_fn(value, context)); },
[&](const auto& __v) { return gen_expr_fn(value, context); }
}, (*value_type)) : gen_expr_fn(value, context)); },
[&](const auto& __v) { return gen_expr_fn(value, context); }
}, (*map_object)) : gen_expr_fn(value, context));
return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), value_code), try_counter};
}(); },
[&](const auto& __v) { return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr_fn(value, context)), try_counter}; }
}, (*value));
return result;
}(); },
[&](const semantic_ir::SStmtLetPat& sStmtLetPat) { auto [pat, __1, value, value_type, __4] = sStmtLetPat; return context::GenStmtResult{let_pat::gen_let_pattern_statement(pat, value, value_type, context, gen_expr_fn), try_counter}; },
[&](const semantic_ir::SStmtLetConst& sStmtLetConst) { auto [name, value, __2, __3] = sStmtLetConst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), gen_expr_fn(value, context)), try_counter}; },
[&](const semantic_ir::SStmtExpr& sStmtExpr) { auto [expression, __1] = sStmtExpr; return std::visit(overloaded{[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return [&]() {
auto try_identifier = (mlc::String("__try_", 6) + mlc::to_string(try_counter));
return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::discard_try_ok_field0_statement(try_identifier), gen_expr_fn), (try_counter + 1)};
}(); },
[&](const auto& __v) { return context::GenStmtResult{eval_stmt_expr(expression, context, gen_expr_fn), try_counter}; }
}, (*expression)); },
[&](const semantic_ir::SStmtBreak& sStmtBreak) { auto [__0] = sStmtBreak; return context::GenStmtResult{expr::break_line(), try_counter}; },
[&](const semantic_ir::SStmtContinue& sStmtContinue) { auto [__0] = sStmtContinue; return context::GenStmtResult{expr::continue_line(), try_counter}; },
[&](const semantic_ir::SStmtReturn& sStmtReturn) { auto [expression, __1] = sStmtReturn; return std::visit(overloaded{[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return [&]() {
auto try_identifier = (mlc::String("__try_ret_", 10) + mlc::to_string(try_counter));
return context::GenStmtResult{eval_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier), gen_expr_fn), (try_counter + 1)};
}(); },
[&](const auto& __v) { return context::GenStmtResult{expr::return_line(gen_expr_fn(expression, context)), try_counter}; }
}, (*expression)); }
}, (*stmt));
}
context::GenStmtsResult eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
auto current_context = context;
auto counter = try_counter;
while ((index < statements.length())) {
auto result = eval_stmt_with_try(statements[index], current_context, counter, gen_expr_fn);
parts.push_back(result.output);
counter = result.next_try;
current_context = context::update_context_from_statement(statements[index], current_context);
index = (index + 1);
}
return context::GenStmtsResult{parts.join(mlc::String("", 0)), counter};
}
mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept{
auto result = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
return result.code;
}

} // namespace stmt_eval

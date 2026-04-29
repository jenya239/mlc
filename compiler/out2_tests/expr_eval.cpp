#define main mlc_user_main
#include "expr_eval.hpp"

namespace expr_eval {

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < expressions.length())) {
parts.push_back(eval_expr(expressions[index], context, gen_stmts));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::parenthesized_binary(eval_expr(left_expr, context, gen_stmts), operation, eval_expr(right_expr, context, gen_stmts));
}
mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::parenthesized_unary(operation, eval_expr(inner_expr, context, gen_stmts));
}
mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return context::context_resolve(context, cpp_naming::map_builtin(name)); },
[&](const auto& __v) { return eval_expr(function_expr, context, gen_stmts); }
}, (*function_expr));
}
mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto function_code = gen_call_function_code(function_expr, context, gen_stmts);
auto argument_list = eval_argument_list(arguments, context, gen_stmts);
if (expression_support::is_constructor_call(function_expr)) {
return expr::constructor_call_braces(function_code, argument_list);
} else {
return expr::function_call_parentheses(function_code, argument_list);
}
}
mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto object_code = eval_expr(object, context, gen_stmts);
auto operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}
mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::index_subscript(eval_expr(object, context, gen_stmts), eval_expr(index_expr, context, gen_stmts));
}
mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::ternary_conditional(eval_expr(condition, context, gen_stmts), eval_expr(then_expr, context, gen_stmts), eval_expr(else_expr, context, gen_stmts));
}
mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
if ((statements.length() == 0)) {
return eval_expr(result_expr, context, gen_stmts);
} else {
auto body = (gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(eval_expr(result_expr, statement_context::stmts_final_ctx(statements, context), gen_stmts)));
return expr::block_as_immediate_invoked_function_expression(body);
}
}
mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::while_loop_immediate_invoked_function_expression(eval_expr(condition, context, gen_stmts), gen_stmts(statements, context));
}
mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), eval_expr(iterator, context, gen_stmts), gen_stmts(statements, context));
}
mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::array_literal(eval_argument_list(elements, context, gen_stmts));
}
mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto inner_code = eval_expr(inner_expr, context, gen_stmts);
return expr::question_try_result(inner_code);
}
mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::lambda_with_return(expression_support::cpp_lambda_header_prefix(parameters), eval_expr(body_expr, context, gen_stmts));
}
mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprInt& sExprInt) { auto [integer_value, __1, __2] = sExprInt; return literals::gen_integer_literal(integer_value); },
[&](const semantic_ir::SExprStr& sExprStr) { auto [string_value, __1, __2] = sExprStr; return literals::gen_string_literal(string_value); },
[&](const semantic_ir::SExprBool& sExprBool) { auto [boolean_value, __1, __2] = sExprBool; return literals::gen_boolean_literal(boolean_value); },
[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return literals::gen_unit_literal(); },
[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return literals::gen_extern_placeholder(); },
[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return identifiers::gen_identifier(name, context); },
[&](const semantic_ir::SExprBin& sExprBin) { auto [operation, left_expr, right_expr, __3, __4] = sExprBin; return gen_binary_expr(operation, left_expr, right_expr, context, gen_stmts); },
[&](const semantic_ir::SExprUn& sExprUn) { auto [operation, inner_expr, __2, __3] = sExprUn; return gen_unary_expr(operation, inner_expr, context, gen_stmts); },
[&](const semantic_ir::SExprCall& sExprCall) { auto [function_expr, arguments, __2, __3] = sExprCall; return gen_call_expr(function_expr, arguments, context, gen_stmts); },
[&](const semantic_ir::SExprMethod& sExprMethod) { auto [object, method_name, arguments, __3, __4] = sExprMethod; return method_gen::gen_method_expr(object, method_name, arguments, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprField& sExprField) { auto [object, field_name, __2, __3] = sExprField; return gen_field_expr(object, field_name, context, gen_stmts); },
[&](const semantic_ir::SExprIndex& sExprIndex) { auto [object, index_expr, __2, __3] = sExprIndex; return gen_index_expr(object, index_expr, context, gen_stmts); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, __3, __4] = sExprIf; return gen_if_expr(condition, then_expr, else_expr, context, gen_stmts); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return gen_block_expr(statements, result_expr, context, gen_stmts); },
[&](const semantic_ir::SExprWhile& sExprWhile) { auto [condition, statements, __2, __3] = sExprWhile; return gen_while_expr(condition, statements, context, gen_stmts); },
[&](const semantic_ir::SExprFor& sExprFor) { auto [variable, iterator, statements, __3, __4] = sExprFor; return gen_for_expr(variable, iterator, statements, context, gen_stmts); },
[&](const semantic_ir::SExprMatch& sExprMatch) { auto [subject, arms, __2, __3] = sExprMatch; return match_gen::gen_match(subject, arms, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecord& sExprRecord) { auto [type_name, field_values, __2, __3] = sExprRecord; return record_gen::gen_record_expr(type_name, field_values, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecordUpdate& sExprRecordUpdate) { auto [type_name, base_expr, overrides, __3, __4] = sExprRecordUpdate; return record_gen::gen_record_update_expr(type_name, base_expr, overrides, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, __1, __2] = sExprArray; return gen_array_expr(elements, context, gen_stmts); },
[&](const semantic_ir::SExprTuple& sExprTuple) { auto [elements, __1, __2] = sExprTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto ki = 0;
while ((ki < elements.length())) {
parts.push_back(eval_expr(elements[ki], context, gen_stmts));
ki = (ki + 1);
}
return ((mlc::String("std::make_tuple(", 16) + parts.join(mlc::String(", ", 2))) + mlc::String(")", 1));
}(); },
[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return gen_question_expr(inner_expr, context, gen_stmts); },
[&](const semantic_ir::SExprLambda& sExprLambda) { auto [parameters, body_expr, __2, __3] = sExprLambda; return gen_lambda_expr(parameters, body_expr, context, gen_stmts); }
}, (*expr));
}

} // namespace expr_eval

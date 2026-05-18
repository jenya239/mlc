#define main mlc_user_main
#include "expr_eval.hpp"

namespace expr_eval {

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expressions.map([=](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return eval_expr(expression, context, gen_stmts); }).join(mlc::String(", ", 2));
}
mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto method = semantic_type_structure::operator_method_for(operation);
auto left_type = semantic_ir::sexpr_type(left_expr);
auto type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [n] = tNamed; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*left_type));
if (((method != mlc::String("", 0)) && (type_name != mlc::String("", 0)))) {
return ((((((((mlc::String("", 0) + mlc::to_string(type_name)) + mlc::String("_", 1)) + mlc::to_string(method)) + mlc::String("(", 1)) + mlc::to_string(eval_expr(left_expr, context, gen_stmts))) + mlc::String(", ", 2)) + mlc::to_string(eval_expr(right_expr, context, gen_stmts))) + mlc::String(")", 1));
} else {
return expr::parenthesized_binary(eval_expr(left_expr, context, gen_stmts), operation, eval_expr(right_expr, context, gen_stmts));
}
}
mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::parenthesized_unary(operation, eval_expr(inner_expr, context, gen_stmts));
}
mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return CodegenContext_resolve(context, cpp_naming::map_builtin(name)); },
[&](const auto& __v) { return eval_expr(function_expr, context, gen_stmts); }
}, (*function_expr));
}
void gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression, mlc::String ) noexcept{
return ((Shared{} < semantic_ir::SExpr{}) > /* unit */);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::index_subscript(eval_expr(object, context, gen_stmts), eval_expr(index_expr, context, gen_stmts));
}
mlc::String gen_if_else_code(std::shared_ptr<semantic_ir::SExpr> else_expr, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SExpr> then_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, __1, array_span] = sExprArray; return ((elements.length() > 0) ? (eval_expr(else_expr, context, gen_stmts)) : ([&]() {
auto inner_preferred = semantic_type_structure::array_element_type_from_array_type(if_semantic_type);
auto inner_from_then_branch = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(then_expr));
auto inner_for_empty_array = ((!semantic_type_structure::type_is_unknown(inner_preferred)) ? (inner_preferred) : (inner_from_then_branch));
return (semantic_type_structure::type_is_unknown(inner_for_empty_array) ? (eval_expr(else_expr, context, gen_stmts)) : (eval_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_for_empty_array}), array_span}), context, gen_stmts)));
}())); },
[&](const auto& __v) { return eval_expr(else_expr, context, gen_stmts); }
}, (*else_expr));
}
mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto cond_code = eval_expr(condition, context, gen_stmts);
auto then_code = eval_expr(then_expr, context, gen_stmts);
auto else_code = gen_if_else_code(else_expr, if_semantic_type, then_expr, context, gen_stmts);
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return [&]() {
auto type_cpp = CodegenContext_resolve(context, type_name);
return ((type_cpp == mlc::String("auto", 4)) ? (expr::ternary_conditional(cond_code, then_code, else_code)) : (((((((((mlc::String("[&]() -> ", 9) + mlc::to_string(type_cpp)) + mlc::String(" {\nif (", 7)) + mlc::to_string(cond_code)) + mlc::String(") {\nreturn ", 11)) + mlc::to_string(then_code)) + mlc::String(";\n} else {\nreturn ", 18)) + mlc::to_string(else_code)) + mlc::String(";\n}\n}()", 7))));
}(); },
[&](const auto& __v) { return expr::ternary_conditional(cond_code, then_code, else_code); }
}, (*if_semantic_type));
}
mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
if ((statements.length() == 0)) {
return eval_expr(result_expr, context, gen_stmts);
} else {
auto body = ((((mlc::String("", 0) + mlc::to_string(gen_stmts(statements, context))) + mlc::String("", 0)) + mlc::to_string(expr::block_immediate_function_return_fragment(eval_expr(result_expr, statement_context::stmts_final_ctx(statements, context), gen_stmts)))) + mlc::String("", 0));
return expr::block_as_immediate_invoked_function_expression(body);
}
}
mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::while_loop_immediate_invoked_function_expression(eval_expr(condition, context, gen_stmts), gen_stmts(statements, context));
}
mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), eval_expr(iterator, context, gen_stmts), gen_stmts(statements, context));
}
mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_type] = tArray; return [&]() {
auto element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
return ((elements.length() == 0) ? (expr::typed_array_empty_or_untyped_empty(element_cpp)) : (((element_cpp == mlc::String("auto", 4)) ? (expr::array_literal(eval_argument_list(elements, context, gen_stmts))) : (expr::typed_array_braced_initializer(element_cpp, eval_argument_list(elements, context, gen_stmts))))));
}(); },
[&](const auto& __v) { return expr::array_literal(eval_argument_list(elements, context, gen_stmts)); }
}, (*element_container_semantic_type));
}
mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
auto inner_code = eval_expr(inner_expr, context, gen_stmts);
return expr::question_try_result(inner_code);
}
mlc::String cpp_lambda_header_from_semantic_function_type(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda, context::CodegenContext context) noexcept{
auto capture_clause = ((parameter_binding_names.length() == 0) ? (mlc::String("[]", 2)) : (mlc::String("[=]", 3)));
return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [lambda_parameter_semantic_types, __1] = tFn; return ((lambda_parameter_semantic_types.length() != parameter_binding_names.length()) ? (expression_support::cpp_lambda_header_prefix(parameter_binding_names)) : ([&]() {
auto formatted_parameter_declarations = mlc::Array<mlc::String>{};
auto lambda_parameter_index = 0;
while ((lambda_parameter_index < parameter_binding_names.length())) {
formatted_parameter_declarations.push_back(expr::parameter_declaration_item(type_gen::sem_type_to_cpp(context, lambda_parameter_semantic_types[lambda_parameter_index]), cpp_naming::cpp_safe(parameter_binding_names[lambda_parameter_index])));
lambda_parameter_index = (lambda_parameter_index + 1);
}
return ((((mlc::String("", 0) + mlc::to_string(capture_clause)) + mlc::String("(", 1)) + mlc::to_string(formatted_parameter_declarations.join(mlc::String(", ", 2)))) + mlc::String(") mutable", 9));
}())); },
[&](const auto& __v) { return expression_support::cpp_lambda_header_prefix(parameter_binding_names); }
}, (*semantic_function_type_for_lambda));
}
mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return expr::lambda_with_return(cpp_lambda_header_from_semantic_function_type(parameter_binding_names, semantic_function_type_for_lambda_expression, context), eval_expr(body_expression_under_lambda, context, gen_stmts));
}
mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprInt& sExprInt) { auto [integer_value, __1, __2] = sExprInt; return literals::gen_integer_literal(integer_value); },
[&](const semantic_ir::SExprStr& sExprStr) { auto [string_value, __1, __2] = sExprStr; return literals::gen_string_literal(string_value); },
[&](const semantic_ir::SExprBool& sExprBool) { auto [boolean_value, __1, __2] = sExprBool; return literals::gen_boolean_literal(boolean_value); },
[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return literals::gen_unit_literal(); },
[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return literals::gen_extern_placeholder(); },
[&](const semantic_ir::SExprFloat& sExprFloat) { auto [v, __1, __2] = sExprFloat; return literals::gen_float_literal(v); },
[&](const semantic_ir::SExprI64& sExprI64) { auto [v, __1, __2] = sExprI64; return literals::gen_i64_literal(v); },
[&](const semantic_ir::SExprU8& sExprU8) { auto [v, __1, __2] = sExprU8; return literals::gen_u8_literal(v); },
[&](const semantic_ir::SExprUsize& sExprUsize) { auto [v, __1, __2] = sExprUsize; return literals::gen_usize_literal(v); },
[&](const semantic_ir::SExprChar& sExprChar) { auto [v, __1, __2] = sExprChar; return literals::gen_char_literal(v); },
[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return identifiers::gen_identifier(name, context); },
[&](const semantic_ir::SExprBin& sExprBin) { auto [operation, left_expr, right_expr, __3, __4] = sExprBin; return gen_binary_expr(operation, left_expr, right_expr, context, gen_stmts); },
[&](const semantic_ir::SExprUn& sExprUn) { auto [operation, inner_expr, __2, __3] = sExprUn; return gen_unary_expr(operation, inner_expr, context, gen_stmts); },
[&](const semantic_ir::SExprCall& sExprCall) { auto [function_expression, arguments, call_parameter_mutability_flags, __3, __4] = sExprCall; return gen_call_expr(function_expression, arguments, call_parameter_mutability_flags, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprMethod& sExprMethod) { auto [method_object, method_name, arguments, receiver_and_parameter_mut_flags, __4, __5] = sExprMethod; return method_gen::gen_method_expr(method_object, method_name, arguments, receiver_and_parameter_mut_flags, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprField& sExprField) { auto [object, field_name, __2, __3] = sExprField; return gen_field_expr(object, field_name, context, gen_stmts); },
[&](const semantic_ir::SExprIndex& sExprIndex) { auto [object, index_expr, __2, __3] = sExprIndex; return gen_index_expr(object, index_expr, context, gen_stmts); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, if_semantic_type, __4] = sExprIf; return gen_if_expr(condition, then_expr, else_expr, if_semantic_type, context, gen_stmts); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return gen_block_expr(statements, result_expr, context, gen_stmts); },
[&](const semantic_ir::SExprWhile& sExprWhile) { auto [condition, statements, __2, __3] = sExprWhile; return gen_while_expr(condition, statements, context, gen_stmts); },
[&](const semantic_ir::SExprFor& sExprFor) { auto [variable, iterator, statements, __3, __4] = sExprFor; return gen_for_expr(variable, iterator, statements, context, gen_stmts); },
[&](const semantic_ir::SExprMatch& sExprMatch) { auto [subject, arms, match_semantic_type, __3] = sExprMatch; return match_gen::gen_match(subject, arms, match_semantic_type, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecord& sExprRecord) { auto [type_name, field_values, expr_type, __3] = sExprRecord; return record_gen::gen_record_expr(type_name, field_values, expr_type, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecordUpdate& sExprRecordUpdate) { auto [type_name, base_expr, overrides, __3, __4] = sExprRecordUpdate; return record_gen::gen_record_update_expr(type_name, base_expr, overrides, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, container_semantic_type, __2] = sExprArray; return gen_array_expr(elements, container_semantic_type, context, gen_stmts); },
[&](const semantic_ir::SExprTuple& sExprTuple) { auto [elements, __1, __2] = sExprTuple; return ((mlc::String("std::make_tuple(", 16) + mlc::to_string(elements.map([=](std::shared_ptr<semantic_ir::SExpr> element) mutable { return eval_expr(element, context, gen_stmts); }).join(mlc::String(", ", 2)))) + mlc::String(")", 1)); },
[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return gen_question_expr(inner_expr, context, gen_stmts); },
[&](const semantic_ir::SExprLambda& sExprLambda) { auto [parameters, body_expr, lambda_annotation_type, __3] = sExprLambda; return gen_lambda_expr(parameters, body_expr, lambda_annotation_type, context, gen_stmts); },
[&](const semantic_ir::SExprWith& sExprWith) { auto [resource, binder, stmts, __3, __4] = sExprWith; return [&]() {
auto resource_code = eval_expr(resource, context, gen_stmts);
auto body_code = gen_stmts(stmts, context);
return expr::with_block_statement(resource_code, binder, body_code);
}(); }
}, (*expr));
}

} // namespace expr_eval

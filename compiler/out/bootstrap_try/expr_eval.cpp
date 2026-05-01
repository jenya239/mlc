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
mlc::String eval_expression_else_branch_with_conditional_empty_array_coercion(std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, container_semantic_type, array_span] = sExprArray; return ((elements.length() > 0) ? (eval_expr(else_expression, context, gen_stmts)) : ([&]() {
std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_then] = tArray; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_else] = tArray; return (semantic_type_structure::type_is_unknown(inner_else) ? (eval_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_then}), array_span}), context, gen_stmts)) : (eval_expr(else_expression, context, gen_stmts))); },
[&](const auto& __v) { return eval_expr(else_expression, context, gen_stmts); }
}, (*container_semantic_type)); },
[&](const auto& __v) { return eval_expr(else_expression, context, gen_stmts); }
}, (*semantic_ir::sexpr_type(then_expression)));
/* unit */;
[=](auto _) { return eval_expr(else_expression, context, gen_stmts); };
/* unit */;
/* unit */;
gen_binary_expr(string, (Shared{} < semantic_ir::SExpr{}), /* unit */, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = [&]() {
auto method = semantic_type_structure::operator_method_for(operation);
auto left_type = semantic_ir::sexpr_type(left_expr);
auto type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [n] = tNamed; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*left_type));
return (((method != mlc::String("", 0)) && (type_name != mlc::String("", 0))) ? ((((((((type_name + mlc::String("_", 1)) + method) + mlc::String("(", 1)) + eval_expr(left_expr, context, gen_stmts)) + mlc::String(", ", 2)) + eval_expr(right_expr, context, gen_stmts)) + mlc::String(")", 1))) : (expr::parenthesized_binary(eval_expr(left_expr, context, gen_stmts), operation, eval_expr(right_expr, context, gen_stmts))));
}();
/* unit */;
gen_unary_expr(string, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::parenthesized_unary(operation, eval_expr(inner_expr, context, gen_stmts));
/* unit */;
gen_call_function_code((Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return context::context_resolve(context, cpp_naming::map_builtin(name)); },
[&](const auto& __v) { return eval_expr(function_expr, context, gen_stmts); }
}, function_expr);
/* unit */;
gen_call_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, mlc::Array<bool>{(Shared{} < semantic_ir::SExpr{})}, /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = [&]() {
auto function_code = gen_call_function_code(function_expr, context, gen_stmts);
auto argument_list = eval_argument_list(arguments, context, gen_stmts);
return (expression_support::is_constructor_call(function_expr) ? (expr::constructor_call_braces(function_code, argument_list)) : (expr::function_call_parentheses(function_code, argument_list)));
}();
/* unit */;
gen_field_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, string, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = [&]() {
auto object_code = eval_expr(object, context, gen_stmts);
auto operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}();
/* unit */;
gen_index_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::index_subscript(eval_expr(object, context, gen_stmts), eval_expr(index_expr, context, gen_stmts));
/* unit */;
gen_if_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, (Shared{} < semantic_ir::SExpr{}), /* unit */, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::ternary_conditional(eval_expr(condition, context, gen_stmts), eval_expr(then_expr, context, gen_stmts), eval_expression_else_branch_with_conditional_empty_array_coercion(then_expr, else_expr, context, gen_stmts));
/* unit */;
gen_block_expr(mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, /* unit */, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = ((statements.length() == 0) ? (eval_expr(result_expr, context, gen_stmts)) : ([&]() {
auto body = (gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(eval_expr(result_expr, statement_context::stmts_final_ctx(statements, context), gen_stmts)));
return expr::block_as_immediate_invoked_function_expression(body);
}()));
/* unit */;
gen_while_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::while_loop_immediate_invoked_function_expression(eval_expr(condition, context, gen_stmts), gen_stmts(statements, context));
/* unit */;
gen_for_expr(string, (Shared{} < semantic_ir::SExpr{}), /* unit */, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), eval_expr(iterator, context, gen_stmts), gen_stmts(statements, context));
/* unit */;
gen_array_expr(mlc::Array<bool>{(Shared{} < semantic_ir::SExpr{})}, /* unit */, (Shared{} < registry::Type{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_type] = tArray; return [&]() {
auto element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
return ((elements.length() == 0) ? (expr::typed_array_empty_or_untyped_empty(element_cpp)) : (((element_cpp == mlc::String("auto", 4)) ? (expr::array_literal(eval_argument_list(elements, context, gen_stmts))) : (expr::typed_array_braced_initializer(element_cpp, eval_argument_list(elements, context, gen_stmts))))));
}(); },
[&](const auto& __v) { return expr::array_literal(eval_argument_list(elements, context, gen_stmts)); }
}, element_container_semantic_type);
/* unit */;
gen_question_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = [&]() {
auto inner_code = eval_expr(inner_expr, context, gen_stmts);
return expr::question_try_result(inner_code);
}();
/* unit */;
gen_lambda_expr(mlc::Array{string}, (Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = expr::lambda_with_return(expression_support::cpp_lambda_header_prefix(parameters), eval_expr(body_expr, context, gen_stmts));
export_;
/* unit */;
eval_expr((Shared{} < semantic_ir::SExpr{}), /* unit */, context::CodegenContext{}, mlc::Array<bool>{(Shared{} < semantic_ir::SStmt{})}, context::CodegenContext{});
/* unit */;
string;
/* unit */;
/* unit */;
string = std::visit(overloaded{[&](const semantic_ir::SExprInt& sExprInt) { auto [integer_value, __1, __2] = sExprInt; return literals::gen_integer_literal(integer_value); },
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
[&](const semantic_ir::SExprCall& sExprCall) { auto [function_expr, arguments, __2, __3] = sExprCall; return gen_call_expr(function_expr, arguments, context, gen_stmts); },
[&](const semantic_ir::SExprMethod& sExprMethod) { auto [object, method_name, arguments, __3, __4] = sExprMethod; return method_gen::gen_method_expr(object, method_name, arguments, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprField& sExprField) { auto [object, field_name, __2, __3] = sExprField; return gen_field_expr(object, field_name, context, gen_stmts); },
[&](const semantic_ir::SExprIndex& sExprIndex) { auto [object, index_expr, __2, __3] = sExprIndex; return gen_index_expr(object, index_expr, context, gen_stmts); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, __3, __4] = sExprIf; return gen_if_expr(condition, then_expr, else_expr, context, gen_stmts); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return gen_block_expr(statements, result_expr, context, gen_stmts); },
[&](const semantic_ir::SExprWhile& sExprWhile) { auto [condition, statements, __2, __3] = sExprWhile; return gen_while_expr(condition, statements, context, gen_stmts); },
[&](const semantic_ir::SExprFor& sExprFor) { auto [variable, iterator, statements, __3, __4] = sExprFor; return gen_for_expr(variable, iterator, statements, context, gen_stmts); },
[&](const semantic_ir::SExprMatch& sExprMatch) { auto [subject, arms, __2, __3] = sExprMatch; return match_gen::gen_match(subject, arms, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecord& sExprRecord) { auto [type_name, field_values, expr_type, __3] = sExprRecord; return record_gen::gen_record_expr(type_name, field_values, expr_type, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprRecordUpdate& sExprRecordUpdate) { auto [type_name, base_expr, overrides, __3, __4] = sExprRecordUpdate; return record_gen::gen_record_update_expr(type_name, base_expr, overrides, context, gen_stmts, eval_expr); },
[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, container_semantic_type, __2] = sExprArray; return gen_array_expr(elements, container_semantic_type, context, gen_stmts); },
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
[&](const semantic_ir::SExprLambda& sExprLambda) { auto [parameters, body_expr, __2, __3] = sExprLambda; return gen_lambda_expr(parameters, body_expr, context, gen_stmts); },
[&](const semantic_ir::SExprWith& sExprWith) { auto [resource, binder, stmts, __3, __4] = sExprWith; return [&]() {
auto resource_code = eval_expr(resource, context, gen_stmts);
auto body_code = gen_stmts(stmts, context);
return expr::with_block_statement(resource_code, binder, body_code);
}(); }
}, expr);
return /* unit */;
}())); }
}, (*else_expression));
}

} // namespace expr_eval

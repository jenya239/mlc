#define main mlc_user_main
#include "method_gen.hpp"

namespace method_gen {

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
if ((trailing_arguments_joined.length() == 0)) {
return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), mlc::String("", 0));
} else {
return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments_joined);
}
}
mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto argument_code = eval_expr_fn(argument, context, gen_stmts);
auto type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}
mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept{
auto mangled_name = context.method_owners.get(method_name);
auto function_name = CodegenContext_resolve(context, mangled_name);
if ((trailing_arguments_joined.length() == 0)) {
return expr::function_call_parentheses(function_name, receiver_fragment);
} else {
return expr::function_call_parentheses(function_name, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
}
}
mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
auto call_base = expr::dot_member_access(receiver_fragment, cpp_naming::map_method(method_name));
if ((trailing_arguments_joined.length() == 0)) {
return expr::function_call_parentheses(call_base, mlc::String("", 0));
} else {
return expr::function_call_parentheses(call_base, trailing_arguments_joined);
}
}
mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
auto call_base = ((((mlc::String("", 0) + mlc::to_string(static_prefix)) + mlc::String("", 0)) + mlc::to_string(cpp_naming::map_method(method_name))) + mlc::String("", 0));
if ((trailing_arguments_joined.length() == 0)) {
return expr::function_call_parentheses(call_base, mlc::String("", 0));
} else {
return expr::function_call_parentheses(call_base, trailing_arguments_joined);
}
}
mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("i32", 3); },
[&](const registry::TString& tString) { return mlc::String("string", 6); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*type_value));
}
bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{
return ((context.method_owners.has(method_name) && (type_name != mlc::String("", 0))) && (context.method_owners.get(method_name) == ((((mlc::String("", 0) + mlc::to_string(type_name)) + mlc::String("_", 1)) + mlc::to_string(method_name)) + mlc::String("", 0))));
}
mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept{
return fragments.join(mlc::String(", ", 2));
}
mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SExpr> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto static_receiver_name = std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return name; },
[&](const auto& __v) { return receiver_fragment; }
}, (*original_object_expression));
if ((static_receiver_name == mlc::String("File", 4))) {
return gen_method_file_using_trailing_argument_fragments(method_name, trailing_arguments_joined);
} else if (((static_receiver_name == mlc::String("Map", 3)) && (method_name == mlc::String("new", 3)))) {
return expr::empty_map_initializer();
} else if ((((static_receiver_name == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3))) && (argument_count == 1))) {
return gen_method_shared_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn);
} else if (is_user_defined_method_for_type(method_name, object_type_name, context)) {
return gen_method_owner_call_using_fragments(receiver_fragment, method_name, trailing_arguments_joined, context);
} else if (((method_name == mlc::String("to_string", 9)) && (argument_count == 0))) {
return expr::runtime_to_string_call(receiver_fragment);
} else {
return gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined);
}




}
mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
auto base = (is_option ? (mlc::String("mlc::opt::", 10)) : (mlc::String("mlc::result::", 13)));
auto cpp_name = (((!is_option) && (method_name == mlc::String("ok", 2))) ? (mlc::String("ok_into_optional", 16)) : (method_name));
auto callee = ((((mlc::String("", 0) + mlc::to_string(base)) + mlc::String("", 0)) + mlc::to_string(cpp_name)) + mlc::String("", 0));
if ((trailing_arguments_joined.length() == 0)) {
return expr::function_call_parentheses(callee, receiver_fragment);
} else {
return expr::function_call_parentheses(callee, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
}
}
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SExpr> object, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> trailing_arguments) noexcept{
auto positional = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
positional.push_back(object);
auto argument_walk_index = 0;
while ((argument_walk_index < trailing_arguments.length())) {
positional.push_back(trailing_arguments[argument_walk_index]);
argument_walk_index = (argument_walk_index + 1);
}
return positional;
}
mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto object_type = semantic_ir::sexpr_type(object);
auto object_type_name = object_type_name_for_dispatch(object_type);
auto positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
auto materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_maybe_holding_mut_references(positional_argument_expressions, receiver_and_method_parameter_mutability_flags, context, gen_stmts, eval_expr_fn);
auto receiver_fragment = materialization_outcome.materialized_argument_fragments[0];
auto trailing_argument_fragments = mlc::Array<mlc::String>{};
auto tail_fragment_index = 1;
while ((tail_fragment_index < materialization_outcome.materialized_argument_fragments.length())) {
trailing_argument_fragments.push_back(materialization_outcome.materialized_argument_fragments[tail_fragment_index]);
tail_fragment_index = (tail_fragment_index + 1);
}
auto trailing_arguments_joined = trailing_argument_fragments_join(trailing_argument_fragments);
auto ro_code = std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [gname, __1] = tGeneric; return ((gname == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined)) : (((gname == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined)) : (mlc::String("", 0))))); },
[&](const registry::TNamed& tNamed) { auto [receiver_named] = tNamed; return ((receiver_named == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined)) : (((receiver_named == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined)) : (mlc::String("", 0))))); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*object_type));
auto invoke_expression = ((ro_code != mlc::String("", 0)) ? (ro_code) : ([&]() {
std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [namespace_object_name, __1, __2] = sExprIdent; return (context.namespace_alias_prefixes.has(namespace_object_name) ? (gen_method_namespace_alias_using_fragments(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_arguments_joined)) : (gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.length(), context, object, arguments, gen_stmts, eval_expr_fn))); },
[&](const auto& __v) { return gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.length(), context, object, arguments, gen_stmts, eval_expr_fn); }
}, (*object));
return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}()));
}

} // namespace method_gen

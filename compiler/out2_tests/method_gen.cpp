#define main mlc_user_main
#include "method_gen.hpp"

namespace method_gen {

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
mlc::String gen_arg_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < arguments.length())) {
parts.push_back(eval_expr_fn(arguments[index], context, gen_stmts));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_object_code(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return ((context.self_type.length() > 0) ? expression_support::resolve_object_code_in_self_context(name, context) : context::context_resolve(context, cpp_naming::map_builtin(name))); },
[&](const auto& __v) { return eval_expr_fn(object, context, gen_stmts); }
}, (*object));
}
mlc::String gen_method_file(mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), gen_arg_list(arguments, context, gen_stmts, eval_expr_fn));
}
mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto argument_code = eval_expr_fn(argument, context, gen_stmts);
auto type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}
mlc::String gen_method_owner_call(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto mangled_name = context.method_owners.get(method_name);
auto function_name = context::context_resolve(context, mangled_name);
if ((arguments.length() == 0)) {
return expr::function_call_parentheses(function_name, object_code);
} else {
return expr::function_call_parentheses(function_name, expr::comma_separated_pair(object_code, gen_arg_list(arguments, context, gen_stmts, eval_expr_fn)));
}
}
mlc::String gen_method_builtin(mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto call_base = expr::dot_member_access(object_code, cpp_naming::map_method(method_name));
if ((arguments.length() == 0)) {
return expr::function_call_parentheses(call_base, mlc::String("", 0));
} else {
return expr::function_call_parentheses(call_base, gen_arg_list(arguments, context, gen_stmts, eval_expr_fn));
}
}
mlc::String gen_method_namespace_alias(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto call_base = (static_prefix + cpp_naming::map_method(method_name));
if ((arguments.length() == 0)) {
return expr::function_call_parentheses(call_base, mlc::String("", 0));
} else {
return expr::function_call_parentheses(call_base, gen_arg_list(arguments, context, gen_stmts, eval_expr_fn));
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
return ((context.method_owners.has(method_name) && (type_name != mlc::String("", 0))) && (context.method_owners.get(method_name) == ((type_name + mlc::String("_", 1)) + method_name)));
}
mlc::String gen_method_expr_after_object(mlc::String object_code, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
if ((object_code == mlc::String("File", 4))) {
return gen_method_file(method_name, arguments, context, gen_stmts, eval_expr_fn);
} else if (((object_code == mlc::String("Map", 3)) && (method_name == mlc::String("new", 3)))) {
return expr::empty_map_initializer();
} else if ((((object_code == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3))) && (arguments.length() == 1))) {
return gen_method_shared_new(arguments[0], context, gen_stmts, eval_expr_fn);
} else if (is_user_defined_method_for_type(method_name, object_type_name, context)) {
return gen_method_owner_call(object_code, method_name, arguments, context, gen_stmts, eval_expr_fn);
} else if (((method_name == mlc::String("to_string", 9)) && (arguments.length() == 0))) {
return expr::runtime_to_string_call(object_code);
} else {
return gen_method_builtin(object_code, method_name, arguments, context, gen_stmts, eval_expr_fn);
}




}
mlc::String gen_result_option_combinator_call(bool is_option, mlc::String object_code, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto base = (is_option ? mlc::String("mlc::opt::", 10) : mlc::String("mlc::result::", 13));
auto cpp_name = (((!is_option) && (method_name == mlc::String("ok", 2))) ? mlc::String("ok_into_optional", 16) : method_name);
auto callee = (base + cpp_name);
if ((arguments.length() == 0)) {
return expr::function_call_parentheses(callee, object_code);
} else {
return expr::function_call_parentheses(callee, expr::comma_separated_pair(object_code, gen_arg_list(arguments, context, gen_stmts, eval_expr_fn)));
}
}
mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto object_type = semantic_ir::sexpr_type(object);
auto object_type_name = object_type_name_for_dispatch(object_type);
auto object_code = gen_object_code(object, context, gen_stmts, eval_expr_fn);
auto ro_code = std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [gname, __1] = tGeneric; return ((gname == mlc::String("Result", 6)) ? gen_result_option_combinator_call(false, object_code, method_name, arguments, context, gen_stmts, eval_expr_fn) : ((gname == mlc::String("Option", 6)) ? gen_result_option_combinator_call(true, object_code, method_name, arguments, context, gen_stmts, eval_expr_fn) : mlc::String("", 0))); },
[&](const registry::TNamed& tNamed) { auto [n] = tNamed; return ((n == mlc::String("Result", 6)) ? gen_result_option_combinator_call(false, object_code, method_name, arguments, context, gen_stmts, eval_expr_fn) : ((n == mlc::String("Option", 6)) ? gen_result_option_combinator_call(true, object_code, method_name, arguments, context, gen_stmts, eval_expr_fn) : mlc::String("", 0))); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*object_type));
if ((ro_code != mlc::String("", 0))) {
return ro_code;
} else {
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [object_name, __1, __2] = sExprIdent; return (context.namespace_alias_prefixes.has(object_name) ? gen_method_namespace_alias(context.namespace_alias_prefixes.get(object_name), method_name, arguments, context, gen_stmts, eval_expr_fn) : gen_method_expr_after_object(object_code, object_type_name, method_name, arguments, context, gen_stmts, eval_expr_fn)); },
[&](const auto& __v) { return gen_method_expr_after_object(object_code, object_type_name, method_name, arguments, context, gen_stmts, eval_expr_fn); }
}, (*object));
}
}

} // namespace method_gen

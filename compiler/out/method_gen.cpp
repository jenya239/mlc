#include "method_gen.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "expr.hpp"

namespace method_gen {

using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace expression_support;
using namespace mut_actual_argument;
using namespace expr;

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept;

mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept;

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SExpr> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SExpr> object, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> trailing_arguments) noexcept;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), mlc::String("")) : expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments_joined);}

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String argument_code = eval_expr_fn(argument, context, gen_stmts);
mlc::String type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}

mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept{
mlc::String mangled_name = context.method_owners.get(method_name);
mlc::String function_name = context::CodegenContext_resolve(context, mangled_name);
return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(function_name, receiver_fragment) : expr::function_call_parentheses(function_name, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
}

mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
mlc::String call_base = expr::dot_member_access(receiver_fragment, cpp_naming::map_method(method_name));
return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, trailing_arguments_joined);
}

mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
mlc::String call_base = static_prefix + cpp_naming::map_method(method_name);
return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(call_base, mlc::String("")) : expr::function_call_parentheses(call_base, trailing_arguments_joined);
}

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TI32>((*type_value))) {  return mlc::String("i32"); } if (std::holds_alternative<registry::TString>((*type_value))) {  return mlc::String("string"); } if (std::holds_alternative<registry::TBool>((*type_value))) {  return mlc::String("bool"); } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner))) { auto _v_tnamed = std::get<registry::TNamed>((*inner)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }(); } return mlc::String(""); }();}

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{return context.method_owners.has(method_name) && type_name != mlc::String("") && context.method_owners.get(method_name) == type_name + mlc::String("_") + method_name;}

mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept{return fragments.join(mlc::String(", "));}

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SExpr> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return receiver_fragment == mlc::String("File") ? gen_method_file_using_trailing_argument_fragments(method_name, trailing_arguments_joined) : receiver_fragment == mlc::String("Map") && method_name == mlc::String("new") ? expr::empty_map_initializer() : receiver_fragment == mlc::String("Shared") && method_name == mlc::String("new") && argument_count == 1 ? gen_method_shared_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn) : is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call_using_fragments(receiver_fragment, method_name, trailing_arguments_joined, context) : method_name == mlc::String("to_string") && argument_count == 0 ? expr::runtime_to_string_call(receiver_fragment) : gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined);}

mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
mlc::String base = is_option ? mlc::String("mlc::opt::") : mlc::String("mlc::result::");
mlc::String cpp_name = !is_option && method_name == mlc::String("ok") ? mlc::String("ok_into_optional") : method_name;
mlc::String callee = base + cpp_name;
return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(callee, receiver_fragment) : expr::function_call_parentheses(callee, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SExpr> object, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> trailing_arguments) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> positional = {};
positional.push_back(object);
int argument_walk_index = 0;
while (argument_walk_index < trailing_arguments.size()){
{
positional.push_back(trailing_arguments[argument_walk_index]);
argument_walk_index = argument_walk_index + 1;
}
}
return positional;
}

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
std::shared_ptr<registry::Type> object_type = semantic_ir::sexpr_type(object);
mlc::String object_type_name = object_type_name_for_dispatch(object_type);
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
mut_actual_argument::MutActualArgumentMaterializationOutcome materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_maybe_holding_mut_references(positional_argument_expressions, receiver_and_method_parameter_mutability_flags, context, gen_stmts, eval_expr_fn);
mlc::String receiver_fragment = materialization_outcome.materialized_argument_fragments[0];
mlc::Array<mlc::String> trailing_argument_fragments = {};
int tail_fragment_index = 1;
while (tail_fragment_index < materialization_outcome.materialized_argument_fragments.size()){
{
trailing_argument_fragments.push_back(materialization_outcome.materialized_argument_fragments[tail_fragment_index]);
tail_fragment_index = tail_fragment_index + 1;
}
}
mlc::String trailing_arguments_joined = trailing_argument_fragments_join(trailing_argument_fragments);
mlc::String ro_code = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*object_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*object_type)); auto [gname, _w0] = _v_tgeneric; return gname == mlc::String("Result") ? gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined) : gname == mlc::String("Option") ? gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined) : mlc::String(""); } if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [receiver_named] = _v_tnamed; return receiver_named == mlc::String("Result") ? gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined) : receiver_named == mlc::String("Option") ? gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined) : mlc::String(""); } return mlc::String(""); }();
mlc::String invoke_expression = ro_code != mlc::String("") ? ro_code : [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*object)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*object)._); auto [namespace_object_name, _w0, _w1] = _v_sexprident; return context.namespace_alias_prefixes.has(namespace_object_name) ? gen_method_namespace_alias_using_fragments(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_arguments_joined) : gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); } return gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); }();
return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}

} // namespace method_gen

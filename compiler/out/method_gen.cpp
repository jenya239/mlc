#include "method_gen.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "expr.hpp"

namespace method_gen {

using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace cpp_naming;
using namespace expression_support;
using namespace mut_actual_argument;
using namespace expr;

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_profile_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String type_method_owner_key(mlc::String type_name, mlc::String method_name) noexcept;

mlc::String resolve_method_owner_mangled(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String type_name, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept;

mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept;

mlc::String result_option_method_cpp_name(bool is_option, mlc::String method_name) noexcept;

mlc::String gen_method_expr_fallback_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_fallback_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept;

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> trailing_arguments) noexcept;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> prepend_expression_to_argument_list(std::shared_ptr<cpp_ast::CppExpression> leading_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_expressions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_file_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_profile_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_owner_call_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_builtin_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_namespace_alias_cpp(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_after_object_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_result_option_combinator_call_cpp(bool is_option, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_dispatch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String object_type_name, mlc::String method_name, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_argument_expressions, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), mlc::String("")) : expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments_joined);}

mlc::String gen_method_profile_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(expression_support::cpp_function_name_for_profile_method(method_name), mlc::String("")) : expr::function_call_parentheses(expression_support::cpp_function_name_for_profile_method(method_name), trailing_arguments_joined);}

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String argument_code = eval_expr_fn(argument, context, gen_stmts);
mlc::String type_name = expression_support::infer_shared_new_type_name(argument, context);
return expr::make_shared_call(type_name, argument_code);
}

mlc::String type_method_owner_key(mlc::String type_name, mlc::String method_name) noexcept{return type_name + mlc::String("_") + method_name;}

mlc::String resolve_method_owner_mangled(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{return type_name != mlc::String("") && context.method_owners.has(type_method_owner_key(type_name, method_name)) ? context.method_owners.get(type_method_owner_key(type_name, method_name)) : context.method_owners.get(method_name);}

mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String type_name, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept{
mlc::String mangled_name = resolve_method_owner_mangled(method_name, type_name, context);
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

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{return type_name != mlc::String("") && context.method_owners.has(type_method_owner_key(type_name, method_name)) ? true : context.method_owners.has(method_name) && type_name != mlc::String("") && context.method_owners.get(method_name) == type_method_owner_key(type_name, method_name);}

mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept{return fragments.join(mlc::String(", "));}

mlc::String result_option_method_cpp_name(bool is_option, mlc::String method_name) noexcept{return [&]() -> mlc::String { if (method_name == mlc::String("ok")) { return is_option ? method_name : mlc::String("ok_into_optional"); } return method_name; }();}

mlc::String gen_method_expr_fallback_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept{return is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, context) : [&]() -> mlc::String { if (method_name == mlc::String("to_string")) { return argument_count == 0 ? expr::runtime_to_string_call(receiver_fragment) : gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined); } return gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined); }();}

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_fallback_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept{return is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, context) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (method_name == mlc::String("to_string")) { return argument_count == 0 ? make_callee_call_cpp(mlc::String("mlc::to_string"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{receiver_expression}) : gen_method_builtin_cpp(receiver_expression, method_name, trailing_arguments); } return gen_method_builtin_cpp(receiver_expression, method_name, trailing_arguments); }();}

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String static_receiver_name = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*original_object_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*original_object_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return name; } return receiver_fragment; }();
return [&]() -> mlc::String { if (static_receiver_name == mlc::String("File")) { return gen_method_file_using_trailing_argument_fragments(method_name, trailing_arguments_joined); } if (static_receiver_name == mlc::String("Profile")) { return gen_method_profile_using_trailing_argument_fragments(method_name, trailing_arguments_joined); } if (static_receiver_name == mlc::String("Map")) { return [&]() -> mlc::String { if (method_name == mlc::String("new")) { return expr::empty_map_initializer(); } return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context); }(); } if (static_receiver_name == mlc::String("Shared")) { return [&]() -> mlc::String { if (method_name == mlc::String("new")) { return argument_count == 1 ? gen_method_shared_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn) : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context); } return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context); }(); } return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context); }();
}

mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
mlc::String base = is_option ? mlc::String("mlc::opt::") : mlc::String("mlc::result::");
mlc::String cpp_name = result_option_method_cpp_name(is_option, method_name);
mlc::String callee = base + cpp_name;
return trailing_arguments_joined.length() == 0 ? expr::function_call_parentheses(callee, receiver_fragment) : expr::function_call_parentheses(callee, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
}

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> trailing_arguments) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional = {};
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

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
std::shared_ptr<registry::Type> object_type = semantic_ir::sexpr_type(object);
mlc::String object_type_name = object_type_name_for_dispatch(object_type);
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
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
mlc::String ro_code = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*object_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*object_type)); auto [generic_name, _w0] = _v_tgeneric; return [&]() -> mlc::String { if (generic_name == mlc::String("Result")) { return gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined); } if (generic_name == mlc::String("Option")) { return gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined); } return mlc::String(""); }(); } if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [receiver_named] = _v_tnamed; return [&]() -> mlc::String { if (receiver_named == mlc::String("Result")) { return gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined); } if (receiver_named == mlc::String("Option")) { return gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined); } return mlc::String(""); }(); } return mlc::String(""); }();
mlc::String invoke_expression = ro_code != mlc::String("") ? ro_code : [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*object)._); auto [namespace_object_name, _w0, _w1] = _v_semanticexpressionident; return context.namespace_alias_prefixes.has(namespace_object_name) ? gen_method_namespace_alias_using_fragments(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_arguments_joined) : gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); } return gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); }();
return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}

std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(callee_name), arguments));}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> prepend_expression_to_argument_list(std::shared_ptr<cpp_ast::CppExpression> leading_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_expressions) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{leading_expression};
int index = 0;
while (index < trailing_expressions.size()){
{
arguments.push_back(trailing_expressions[index]);
index = index + 1;
}
}
return arguments;
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_file_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return make_callee_call_cpp(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_profile_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return make_callee_call_cpp(expression_support::cpp_function_name_for_profile_method(method_name), trailing_arguments);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
std::shared_ptr<cpp_ast::CppExpression> argument_expression = evaluate_expression(argument, context, gen_stmts);
mlc::String element_type = expression_support::infer_shared_new_type_name(argument, context);
return make_callee_call_cpp(mlc::String("std::make_shared<") + element_type + mlc::String(">"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{argument_expression});
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_owner_call_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, context::CodegenContext context) noexcept{
mlc::String mangled_name = resolve_method_owner_mangled(method_name, type_name, context);
return make_callee_call_cpp(context::CodegenContext_resolve(context, mangled_name), prepend_expression_to_argument_list(receiver_expression, trailing_arguments));
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_builtin_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, cpp_naming::map_method(method_name), false)), trailing_arguments));}

std::shared_ptr<cpp_ast::CppExpression> gen_method_namespace_alias_cpp(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return make_callee_call_cpp(static_prefix + cpp_naming::map_method(method_name), trailing_arguments);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_after_object_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String static_receiver_name = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*original_object_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*original_object_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return name; } return mlc::String(""); }();
return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (static_receiver_name == mlc::String("File")) { return gen_method_file_cpp(method_name, trailing_arguments); } if (static_receiver_name == mlc::String("Profile")) { return gen_method_profile_cpp(method_name, trailing_arguments); } if (static_receiver_name == mlc::String("Map")) { return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (method_name == mlc::String("new")) { return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList({})); } return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context); }(); } if (static_receiver_name == mlc::String("Shared")) { return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (method_name == mlc::String("new")) { return argument_count == 1 ? gen_method_shared_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context); } return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context); }(); } return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context); }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_result_option_combinator_call_cpp(bool is_option, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
mlc::String namespace_prefix = is_option ? mlc::String("mlc::opt::") : mlc::String("mlc::result::");
mlc::String combinator_name = result_option_method_cpp_name(is_option, method_name);
return make_callee_call_cpp(namespace_prefix + combinator_name, prepend_expression_to_argument_list(receiver_expression, trailing_arguments));
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_dispatch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String object_type_name, mlc::String method_name, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_argument_expressions, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*object)._); auto [namespace_object_name, _w0, _w1] = _v_semanticexpressionident; return context.namespace_alias_prefixes.has(namespace_object_name) ? gen_method_namespace_alias_cpp(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_argument_expressions) : gen_method_expr_after_object_cpp(receiver_expression, object_type_name, method_name, trailing_argument_expressions, arguments.size(), context, object, arguments, gen_stmts, evaluate_expression); } return gen_method_expr_after_object_cpp(receiver_expression, object_type_name, method_name, trailing_argument_expressions, arguments.size(), context, object, arguments, gen_stmts, evaluate_expression); }();}

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
std::shared_ptr<registry::Type> object_type = semantic_ir::sexpr_type(object);
mlc::String object_type_name = object_type_name_for_dispatch(object_type);
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_cpp(positional_argument_expressions, receiver_and_method_parameter_mutability_flags, context, gen_stmts, evaluate_expression);
std::shared_ptr<cpp_ast::CppExpression> receiver_expression = materialization_outcome.materialized_argument_expressions[0];
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_argument_expressions = {};
int tail_index = 1;
while (tail_index < materialization_outcome.materialized_argument_expressions.size()){
{
trailing_argument_expressions.push_back(materialization_outcome.materialized_argument_expressions[tail_index]);
tail_index = tail_index + 1;
}
}
std::shared_ptr<cpp_ast::CppExpression> result_or_option_code = [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<registry::TGeneric>((*object_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*object_type)); auto [generic_name, _w0] = _v_tgeneric; return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (generic_name == mlc::String("Result")) { return gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions); } if (generic_name == mlc::String("Option")) { return gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions); } return gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); }(); } if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [receiver_named] = _v_tnamed; return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (receiver_named == mlc::String("Result")) { return gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions); } if (receiver_named == mlc::String("Option")) { return gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions); } return gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); }(); } return gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); }();
return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, result_or_option_code);
}

} // namespace method_gen

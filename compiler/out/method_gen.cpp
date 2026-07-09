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
#include "type_gen.hpp"
#include "semantic_type_structure.hpp"
#include "weak_method_gen.hpp"

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
using namespace type_gen;
using namespace semantic_type_structure;
using namespace weak_method_gen;

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_profile_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_shared_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_arc_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_mutex_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String type_method_owner_key(mlc::String type_name, mlc::String method_name) noexcept;

mlc::String resolve_method_owner_mangled(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String type_name, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept;

mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept;

mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept;

mlc::String result_option_method_cpp_name(bool is_option, mlc::String method_name) noexcept;

mlc::String static_receiver_name_from_object_expression(std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression) noexcept;

mlc::String gen_method_expr_fallback_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_fallback_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept;

mlc::String gen_method_map_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept;

mlc::String gen_method_shared_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_arc_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_mutex_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> trailing_arguments) noexcept;

mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> prepend_expression_to_argument_list(std::shared_ptr<cpp_ast::CppExpression> leading_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_expressions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_file_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_profile_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_arc_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_lock_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, std::shared_ptr<semantic_ir::SemanticExpression> callback_argument, std::shared_ptr<registry::Type> mutex_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_owner_call_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_builtin_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_namespace_alias_cpp(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_map_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_arc_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

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

mlc::String gen_method_arc_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String argument_code = eval_expr_fn(argument, context, gen_stmts);
mlc::String element_type = expression_support::semantic_type_to_cpp_type_name(semantic_ir::sexpr_type(argument), context);
return mlc::String("mlc::concurrency::Arc<") + element_type + mlc::String(">(") + argument_code + mlc::String(")");
}

mlc::String gen_method_mutex_new(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String argument_code = eval_expr_fn(argument, context, gen_stmts);
mlc::String element_type = expression_support::semantic_type_to_cpp_type_name(semantic_ir::sexpr_type(argument), context);
return mlc::String("mlc::concurrency::Mutex<") + element_type + mlc::String(">(") + argument_code + mlc::String(")");
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

mlc::String result_option_method_cpp_name(bool is_option, mlc::String method_name) noexcept{return method_name == mlc::String("ok") ? is_option ? method_name : mlc::String("ok_into_optional") : method_name;}

mlc::String static_receiver_name_from_object_expression(std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [name, _w0, _w1] = semanticexpressionident; return name; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return mlc::String(""); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return mlc::String(""); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return mlc::String(""); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return mlc::String(""); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return mlc::String(""); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return mlc::String(""); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return mlc::String(""); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return mlc::String(""); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return mlc::String(""); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return mlc::String(""); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return mlc::String(""); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return mlc::String(""); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return mlc::String(""); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return mlc::String(""); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return mlc::String(""); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return mlc::String(""); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return mlc::String(""); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return mlc::String(""); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return mlc::String(""); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return mlc::String(""); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return mlc::String(""); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return mlc::String(""); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return mlc::String(""); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionarray; return mlc::String(""); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return mlc::String(""); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return mlc::String(""); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return mlc::String(""); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return mlc::String(""); }
}, (*original_object_expression)._);}

mlc::String gen_method_expr_fallback_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept{return is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, context) : method_name == mlc::String("to_string") && argument_count == 0 ? expr::runtime_to_string_call(receiver_fragment) : gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_fallback_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept{return is_user_defined_method_for_type(method_name, object_type_name, context) ? gen_method_owner_call_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, context) : method_name == mlc::String("to_string") && argument_count == 0 ? make_callee_call_cpp(mlc::String("mlc::to_string"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{receiver_expression}) : gen_method_builtin_cpp(receiver_expression, method_name, trailing_arguments);}

mlc::String gen_method_map_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept{return method_name == mlc::String("new") ? expr::empty_map_initializer() : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);}

mlc::String gen_method_shared_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_shared_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn) : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);}

mlc::String gen_method_arc_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_arc_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn) : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);}

mlc::String gen_method_mutex_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_mutex_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn) : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);}

mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String ident_name = static_receiver_name_from_object_expression(original_object_expression);
mlc::String static_receiver_name = ident_name != mlc::String("") ? ident_name : receiver_fragment;
return static_receiver_name == mlc::String("File") ? gen_method_file_using_trailing_argument_fragments(method_name, trailing_arguments_joined) : static_receiver_name == mlc::String("Profile") ? gen_method_profile_using_trailing_argument_fragments(method_name, trailing_arguments_joined) : static_receiver_name == mlc::String("Map") ? gen_method_map_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context) : static_receiver_name == mlc::String("Shared") ? gen_method_shared_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn) : static_receiver_name == mlc::String("Arc") ? gen_method_arc_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn) : static_receiver_name == mlc::String("Mutex") ? gen_method_mutex_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn) : gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
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
mlc::String smart_pointer_sugar = weak_method_gen::is_shared_weak_sugar_method(method_name) && semantic_type_structure::type_is_shared_pointer(object_type) && arguments.size() == 0 ? weak_method_gen::gen_method_shared_weak_using_fragments(receiver_fragment, object_type, context) : weak_method_gen::is_weak_upgrade_sugar_method(method_name) && weak_method_gen::type_is_weak_pointer(object_type) && arguments.size() == 0 ? weak_method_gen::gen_method_weak_upgrade_using_fragments(receiver_fragment) : mlc::String("");
mlc::String ro_code = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*object_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*object_type)); auto [generic_name, _w0] = _v_tgeneric; return generic_name == mlc::String("Result") ? gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined) : generic_name == mlc::String("Option") ? gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined) : mlc::String(""); } if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [receiver_named] = _v_tnamed; return receiver_named == mlc::String("Result") ? gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined) : receiver_named == mlc::String("Option") ? gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined) : mlc::String(""); } return mlc::String(""); }();
mlc::String invoke_expression = smart_pointer_sugar != mlc::String("") ? smart_pointer_sugar : ro_code != mlc::String("") ? ro_code : [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*object)._); auto [namespace_object_name, _w0, _w1] = _v_semanticexpressionident; return context.namespace_alias_prefixes.has(namespace_object_name) ? gen_method_namespace_alias_using_fragments(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_arguments_joined) : gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); } return gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.size(), context, object, arguments, gen_stmts, eval_expr_fn); }();
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

std::shared_ptr<cpp_ast::CppExpression> gen_method_arc_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
std::shared_ptr<cpp_ast::CppExpression> argument_expression = evaluate_expression(argument, context, gen_stmts);
mlc::String element_type = expression_support::semantic_type_to_cpp_type_name(semantic_ir::sexpr_type(argument), context);
return make_callee_call_cpp(mlc::String("mlc::concurrency::Arc<") + element_type + mlc::String(">"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{argument_expression});
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_new_cpp(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
std::shared_ptr<cpp_ast::CppExpression> argument_expression = evaluate_expression(argument, context, gen_stmts);
mlc::String element_type = expression_support::semantic_type_to_cpp_type_name(semantic_ir::sexpr_type(argument), context);
return make_callee_call_cpp(mlc::String("mlc::concurrency::Mutex<") + element_type + mlc::String(">"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{argument_expression});
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_lock_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, std::shared_ptr<semantic_ir::SemanticExpression> callback_argument, std::shared_ptr<registry::Type> mutex_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
std::shared_ptr<registry::Type> inner_type = semantic_type_structure::mutex_inner_type_from_mutex_type(mutex_type);
mlc::String inner_type_cpp = type_gen::sem_type_to_cpp(context, inner_type);
return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*callback_argument)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*callback_argument)._); auto [parameter_names, body, _w0, _w1] = _v_semanticexpressionlambda; return parameter_names.size() == 1 ? [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String parameter_name = cpp_naming::cpp_safe(parameter_names[0]);
  std::shared_ptr<cpp_ast::CppExpression> body_expression = evaluate_expression(body, context, gen_stmts);
  std::shared_ptr<cpp_ast::CppExpression> lock_callback = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMutableLambda(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String(""), true})}, mlc::Array<std::shared_ptr<cpp_ast::CppParam>>{std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{parameter_name, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(inner_type_cpp + mlc::String("&")))})}, body_expression));
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, mlc::String("lock"), false)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{lock_callback}));
 }() : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, mlc::String("lock"), false)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{evaluate_expression(callback_argument, context, gen_stmts)})); } return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, mlc::String("lock"), false)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{evaluate_expression(callback_argument, context, gen_stmts)})); }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_owner_call_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, context::CodegenContext context) noexcept{
mlc::String mangled_name = resolve_method_owner_mangled(method_name, type_name, context);
return make_callee_call_cpp(context::CodegenContext_resolve(context, mangled_name), prepend_expression_to_argument_list(receiver_expression, trailing_arguments));
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_builtin_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, cpp_naming::map_method(method_name), false)), trailing_arguments));}

std::shared_ptr<cpp_ast::CppExpression> gen_method_namespace_alias_cpp(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{return make_callee_call_cpp(static_prefix + cpp_naming::map_method(method_name), trailing_arguments);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_map_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept{return method_name == mlc::String("new") ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList({})) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_shared_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_arc_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_arc_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return method_name == mlc::String("new") && argument_count == 1 ? gen_method_mutex_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);}

std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_after_object_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String static_receiver_name = static_receiver_name_from_object_expression(original_object_expression);
return static_receiver_name == mlc::String("File") ? gen_method_file_cpp(method_name, trailing_arguments) : static_receiver_name == mlc::String("Profile") ? gen_method_profile_cpp(method_name, trailing_arguments) : static_receiver_name == mlc::String("Map") ? gen_method_map_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context) : static_receiver_name == mlc::String("Shared") ? gen_method_shared_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression) : static_receiver_name == mlc::String("Arc") ? gen_method_arc_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression) : static_receiver_name == mlc::String("Mutex") ? gen_method_mutex_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression) : gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
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
return method_name == mlc::String("lock") && semantic_type_structure::type_is_mutex(object_type) && arguments.size() == 1 ? mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, gen_method_mutex_lock_cpp(receiver_expression, arguments[0], object_type, context, gen_stmts, evaluate_expression)) : weak_method_gen::is_shared_weak_sugar_method(method_name) && semantic_type_structure::type_is_shared_pointer(object_type) && arguments.size() == 0 ? mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, weak_method_gen::gen_method_shared_weak_cpp(receiver_expression, object_type, context)) : weak_method_gen::is_weak_upgrade_sugar_method(method_name) && weak_method_gen::type_is_weak_pointer(object_type) && arguments.size() == 0 ? mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, weak_method_gen::gen_method_weak_upgrade_cpp(receiver_expression)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  std::shared_ptr<cpp_ast::CppExpression> result_or_option_code = [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<registry::TGeneric>((*object_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*object_type)); auto [generic_name, _w0] = _v_tgeneric; return generic_name == mlc::String("Result") ? gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions) : generic_name == mlc::String("Option") ? gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions) : gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); } if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [receiver_named] = _v_tnamed; return receiver_named == mlc::String("Result") ? gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions) : receiver_named == mlc::String("Option") ? gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions) : gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); } return gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression); }();
  return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, result_or_option_code);
 }();
}

} // namespace method_gen

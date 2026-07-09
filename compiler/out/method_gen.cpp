#define main mlc_user_main
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

mlc::String gen_method_file_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), mlc::String("", 0));
  } else   {
    return expr::function_call_parentheses(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments_joined);
  }
}
mlc::String gen_method_profile_using_trailing_argument_fragments(mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(expression_support::cpp_function_name_for_profile_method(method_name), mlc::String("", 0));
  } else   {
    return expr::function_call_parentheses(expression_support::cpp_function_name_for_profile_method(method_name), trailing_arguments_joined);
  }
}
mlc::String type_method_owner_key(mlc::String type_name, mlc::String method_name) noexcept{
  return ((type_name + mlc::String("_", 1)) + method_name);
}
mlc::String resolve_method_owner_mangled(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{
  if (((type_name != mlc::String("", 0)) && context.method_owners.has(type_method_owner_key(type_name, method_name))))   {
    return context.method_owners.get(type_method_owner_key(type_name, method_name));
  } else   {
    return context.method_owners.get(method_name);
  }
}
mlc::String gen_method_owner_call_using_fragments(mlc::String receiver_fragment, mlc::String type_name, mlc::String method_name, mlc::String trailing_arguments_joined, context::CodegenContext context) noexcept{
  auto mangled_name = resolve_method_owner_mangled(method_name, type_name, context);
  auto function_name = context::CodegenContext_resolve(context, mangled_name);
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(function_name, receiver_fragment);
  } else   {
    return expr::function_call_parentheses(function_name, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
  }
}
mlc::String gen_method_builtin_using_fragments(mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
  auto call_base = expr::dot_member_access(receiver_fragment, cpp_naming::map_method(method_name));
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(call_base, mlc::String("", 0));
  } else   {
    return expr::function_call_parentheses(call_base, trailing_arguments_joined);
  }
}
mlc::String gen_method_namespace_alias_using_fragments(mlc::String static_prefix, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
  auto call_base = ((((mlc::String("", 0) + mlc::to_string(static_prefix)) + mlc::String("", 0)) + mlc::to_string(cpp_naming::map_method(method_name))) + mlc::String("", 0));
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(call_base, mlc::String("", 0));
  } else   {
    return expr::function_call_parentheses(call_base, trailing_arguments_joined);
  }
}
mlc::String object_type_name_for_dispatch(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return mlc::String("i32", 3);
}
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return mlc::String("string", 6);
}
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return mlc::String("bool", 4);
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner] = tShared; return [&]() -> mlc::String {
auto __match_subject = inner;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
}
bool is_user_defined_method_for_type(mlc::String method_name, mlc::String type_name, context::CodegenContext context) noexcept{
  if (((type_name != mlc::String("", 0)) && context.method_owners.has(type_method_owner_key(type_name, method_name))))   {
    return true;
  } else   {
    return ((context.method_owners.has(method_name) && (type_name != mlc::String("", 0))) && (context.method_owners.get(method_name) == type_method_owner_key(type_name, method_name)));
  }
}
mlc::String trailing_argument_fragments_join(mlc::Array<mlc::String> fragments) noexcept{
  return fragments.join(mlc::String(", ", 2));
}
mlc::String result_option_method_cpp_name(bool is_option, mlc::String method_name) noexcept{
  if ((method_name == mlc::String("ok", 2)))   {
    if (is_option)     {
      return method_name;
    } else     {
      return mlc::String("ok_into_optional", 16);
    }
  } else   {
    return method_name;
  }
}
mlc::String static_receiver_name_from_object_expression(std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [name, __1, __2] = semanticExpressionIdent; return name; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1] = semanticExpressionExtern; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, __2, __3] = semanticExpressionBlock; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return mlc::String("", 0); }
}, (*original_object_expression));
}
mlc::String gen_method_expr_fallback_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept{
  if (is_user_defined_method_for_type(method_name, object_type_name, context))   {
    return gen_method_owner_call_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, context);
  } else if (((method_name == mlc::String("to_string", 9)) && (argument_count == 0)))   {
    return expr::runtime_to_string_call(receiver_fragment);
  } else   {
    return gen_method_builtin_using_fragments(receiver_fragment, method_name, trailing_arguments_joined);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_fallback_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept{
  if (is_user_defined_method_for_type(method_name, object_type_name, context))   {
    return gen_method_owner_call_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, context);
  } else if (((method_name == mlc::String("to_string", 9)) && (argument_count == 0)))   {
    return make_callee_call_cpp(mlc::String("mlc::to_string", 14), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{receiver_expression});
  } else   {
    return gen_method_builtin_cpp(receiver_expression, method_name, trailing_arguments);
  }
}
mlc::String gen_method_map_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context) noexcept{
  if ((method_name == mlc::String("new", 3)))   {
    return expr::empty_map_initializer();
  } else   {
    return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  }
}
mlc::String gen_method_shared_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_shared_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn);
  } else   {
    return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  }
}
mlc::String gen_method_arc_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_arc_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn);
  } else   {
    return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  }
}
mlc::String gen_method_mutex_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_mutex_new(original_trailing_arguments[0], context, gen_stmts, eval_expr_fn);
  } else   {
    return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  }
}
mlc::String gen_method_expr_after_object_using_fragments(mlc::String receiver_fragment, mlc::String object_type_name, mlc::String method_name, mlc::String trailing_arguments_joined, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto ident_name = static_receiver_name_from_object_expression(original_object_expression);
  auto static_receiver_name = ((ident_name != mlc::String("", 0)) ? (ident_name) : (receiver_fragment));
  if ((static_receiver_name == mlc::String("File", 4)))   {
    return gen_method_file_using_trailing_argument_fragments(method_name, trailing_arguments_joined);
  } else if ((static_receiver_name == mlc::String("Profile", 7)))   {
    return gen_method_profile_using_trailing_argument_fragments(method_name, trailing_arguments_joined);
  } else if ((static_receiver_name == mlc::String("Map", 3)))   {
    return gen_method_map_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  } else if ((static_receiver_name == mlc::String("Shared", 6)))   {
    return gen_method_shared_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn);
  } else if ((static_receiver_name == mlc::String("Arc", 3)))   {
    return gen_method_arc_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn);
  } else if ((static_receiver_name == mlc::String("Mutex", 5)))   {
    return gen_method_mutex_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context, original_trailing_arguments, gen_stmts, eval_expr_fn);
  } else   {
    return gen_method_expr_fallback_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, argument_count, context);
  }
}
mlc::String gen_result_option_combinator_call_using_fragments(bool is_option, mlc::String receiver_fragment, mlc::String method_name, mlc::String trailing_arguments_joined) noexcept{
  auto base = (is_option ? (mlc::String("mlc::opt::", 10)) : (mlc::String("mlc::result::", 13)));
  auto cpp_name = result_option_method_cpp_name(is_option, method_name);
  auto callee = ((((mlc::String("", 0) + mlc::to_string(base)) + mlc::String("", 0)) + mlc::to_string(cpp_name)) + mlc::String("", 0));
  if ((trailing_arguments_joined.length() == 0))   {
    return expr::function_call_parentheses(callee, receiver_fragment);
  } else   {
    return expr::function_call_parentheses(callee, expr::comma_separated_pair(receiver_fragment, trailing_arguments_joined));
  }
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> build_positional_receiver_and_arguments(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> trailing_arguments) noexcept{
  auto positional = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{};
  positional.push_back(object);
  auto argument_walk_index = 0;
  while ((argument_walk_index < trailing_arguments.length()))   {
    positional.push_back(trailing_arguments[argument_walk_index]);
    (argument_walk_index = (argument_walk_index + 1));
  }
  return positional;
}
mlc::String gen_method_expr(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto object_type = semantic_ir::sexpr_type(object);
  auto object_type_name = object_type_name_for_dispatch(object_type);
  auto positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
  auto materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_maybe_holding_mut_references(positional_argument_expressions, receiver_and_method_parameter_mutability_flags, context, gen_stmts, eval_expr_fn);
  auto receiver_fragment = materialization_outcome.materialized_argument_fragments[0];
  auto trailing_argument_fragments = mlc::Array<mlc::String>{};
  auto tail_fragment_index = 1;
  while ((tail_fragment_index < materialization_outcome.materialized_argument_fragments.length()))   {
    trailing_argument_fragments.push_back(materialization_outcome.materialized_argument_fragments[tail_fragment_index]);
    (tail_fragment_index = (tail_fragment_index + 1));
  }
  auto trailing_arguments_joined = trailing_argument_fragments_join(trailing_argument_fragments);
  auto smart_pointer_sugar = (((weak_method_gen::is_shared_weak_sugar_method(method_name) && semantic_type_structure::type_is_shared_pointer(object_type)) && (arguments.length() == 0)) ? (weak_method_gen::gen_method_shared_weak_using_fragments(receiver_fragment, object_type, context)) : ((((weak_method_gen::is_weak_upgrade_sugar_method(method_name) && weak_method_gen::type_is_weak_pointer(object_type)) && (arguments.length() == 0)) ? (weak_method_gen::gen_method_weak_upgrade_using_fragments(receiver_fragment)) : (mlc::String("", 0)))));
  auto ro_code = [&]() -> mlc::String {
auto __match_subject = object_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [generic_name, __1] = tGeneric; return ((generic_name == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined)) : (((generic_name == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined)) : (mlc::String("", 0)))));
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [receiver_named] = tNamed; return ((receiver_named == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_using_fragments(false, receiver_fragment, method_name, trailing_arguments_joined)) : (((receiver_named == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_using_fragments(true, receiver_fragment, method_name, trailing_arguments_joined)) : (mlc::String("", 0)))));
}
return mlc::String("", 0);
std::abort();
}();
  auto invoke_expression = ((smart_pointer_sugar != mlc::String("", 0)) ? (smart_pointer_sugar) : ([&]() -> mlc::String {
  if ((ro_code != mlc::String("", 0)))   {
    return ro_code;
  } else   {
    return [&]() -> mlc::String {
auto __match_subject = object;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [namespace_object_name, __1, __2] = semanticExpressionIdent; return (context.namespace_alias_prefixes.has(namespace_object_name) ? (gen_method_namespace_alias_using_fragments(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_arguments_joined)) : (gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.length(), context, object, arguments, gen_stmts, eval_expr_fn)));
}
return gen_method_expr_after_object_using_fragments(receiver_fragment, object_type_name, method_name, trailing_arguments_joined, arguments.length(), context, object, arguments, gen_stmts, eval_expr_fn);
std::abort();
}();
  }
}()));
  return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}
std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(callee_name), arguments});
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> prepend_expression_to_argument_list(std::shared_ptr<cpp_ast::CppExpression> leading_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_expressions) noexcept{
  auto arguments = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{leading_expression};
  auto index = 0;
  while ((index < trailing_expressions.length()))   {
    arguments.push_back(trailing_expressions[index]);
    (index = (index + 1));
  }
  return arguments;
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_file_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
  return make_callee_call_cpp(expression_support::cpp_function_name_for_file_method(method_name), trailing_arguments);
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_profile_cpp(mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
  return make_callee_call_cpp(expression_support::cpp_function_name_for_profile_method(method_name), trailing_arguments);
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_owner_call_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, context::CodegenContext context) noexcept{
  auto mangled_name = resolve_method_owner_mangled(method_name, type_name, context);
  return make_callee_call_cpp(context::CodegenContext_resolve(context, mangled_name), prepend_expression_to_argument_list(receiver_expression, trailing_arguments));
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_builtin_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{receiver_expression, cpp_naming::map_method(method_name), false}), trailing_arguments});
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_namespace_alias_cpp(mlc::String static_prefix, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
  return make_callee_call_cpp(((((mlc::String("", 0) + mlc::to_string(static_prefix)) + mlc::String("", 0)) + mlc::to_string(cpp_naming::map_method(method_name))) + mlc::String("", 0)), trailing_arguments);
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_map_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context) noexcept{
  if ((method_name == mlc::String("new", 3)))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList{{}});
  } else   {
    return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_shared_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression);
  } else   {
    return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_arc_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_arc_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression);
  } else   {
    return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_mutex_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  if (((method_name == mlc::String("new", 3)) && (argument_count == 1)))   {
    return gen_method_mutex_new_cpp(original_trailing_arguments[0], context, gen_stmts, evaluate_expression);
  } else   {
    return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_after_object_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String object_type_name, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments, int argument_count, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticExpression> original_object_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> original_trailing_arguments, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  auto static_receiver_name = static_receiver_name_from_object_expression(original_object_expression);
  if ((static_receiver_name == mlc::String("File", 4)))   {
    return gen_method_file_cpp(method_name, trailing_arguments);
  } else if ((static_receiver_name == mlc::String("Profile", 7)))   {
    return gen_method_profile_cpp(method_name, trailing_arguments);
  } else if ((static_receiver_name == mlc::String("Map", 3)))   {
    return gen_method_map_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  } else if ((static_receiver_name == mlc::String("Shared", 6)))   {
    return gen_method_shared_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression);
  } else if ((static_receiver_name == mlc::String("Arc", 3)))   {
    return gen_method_arc_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression);
  } else if ((static_receiver_name == mlc::String("Mutex", 5)))   {
    return gen_method_mutex_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context, original_trailing_arguments, gen_stmts, evaluate_expression);
  } else   {
    return gen_method_expr_fallback_cpp(receiver_expression, object_type_name, method_name, trailing_arguments, argument_count, context);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_result_option_combinator_call_cpp(bool is_option, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::String method_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_arguments) noexcept{
  auto namespace_prefix = (is_option ? (mlc::String("mlc::opt::", 10)) : (mlc::String("mlc::result::", 13)));
  auto combinator_name = result_option_method_cpp_name(is_option, method_name);
  return make_callee_call_cpp(((((mlc::String("", 0) + mlc::to_string(namespace_prefix)) + mlc::String("", 0)) + mlc::to_string(combinator_name)) + mlc::String("", 0)), prepend_expression_to_argument_list(receiver_expression, trailing_arguments));
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_dispatch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String object_type_name, mlc::String method_name, std::shared_ptr<cpp_ast::CppExpression> receiver_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> trailing_argument_expressions, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = object;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [namespace_object_name, __1, __2] = semanticExpressionIdent; return (context.namespace_alias_prefixes.has(namespace_object_name) ? (gen_method_namespace_alias_cpp(context.namespace_alias_prefixes.get(namespace_object_name), method_name, trailing_argument_expressions)) : (gen_method_expr_after_object_cpp(receiver_expression, object_type_name, method_name, trailing_argument_expressions, arguments.length(), context, object, arguments, gen_stmts, evaluate_expression)));
}
return gen_method_expr_after_object_cpp(receiver_expression, object_type_name, method_name, trailing_argument_expressions, arguments.length(), context, object, arguments, gen_stmts, evaluate_expression);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_method_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  auto object_type = semantic_ir::sexpr_type(object);
  auto object_type_name = object_type_name_for_dispatch(object_type);
  auto positional_argument_expressions = build_positional_receiver_and_arguments(object, arguments);
  auto materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_cpp(positional_argument_expressions, receiver_and_method_parameter_mutability_flags, context, gen_stmts, evaluate_expression);
  auto receiver_expression = materialization_outcome.materialized_argument_expressions[0];
  auto trailing_argument_expressions = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
  auto tail_index = 1;
  while ((tail_index < materialization_outcome.materialized_argument_expressions.length()))   {
    trailing_argument_expressions.push_back(materialization_outcome.materialized_argument_expressions[tail_index]);
    (tail_index = (tail_index + 1));
  }
  if ((((method_name == mlc::String("lock", 4)) && semantic_type_structure::type_is_mutex(object_type)) && (arguments.length() == 1)))   {
    return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, gen_method_mutex_lock_cpp(receiver_expression, arguments[0], object_type, context, gen_stmts, evaluate_expression));
  } else if (((weak_method_gen::is_shared_weak_sugar_method(method_name) && semantic_type_structure::type_is_shared_pointer(object_type)) && (arguments.length() == 0)))   {
    return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, weak_method_gen::gen_method_shared_weak_cpp(receiver_expression, object_type, context));
  } else if (((weak_method_gen::is_weak_upgrade_sugar_method(method_name) && weak_method_gen::type_is_weak_pointer(object_type)) && (arguments.length() == 0)))   {
    return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, weak_method_gen::gen_method_weak_upgrade_cpp(receiver_expression));
  } else   {
    auto result_or_option_code = [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = object_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [generic_name, __1] = tGeneric; return ((generic_name == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions)) : (((generic_name == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions)) : (gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression)))));
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [receiver_named] = tNamed; return ((receiver_named == mlc::String("Result", 6)) ? (gen_result_option_combinator_call_cpp(false, receiver_expression, method_name, trailing_argument_expressions)) : (((receiver_named == mlc::String("Option", 6)) ? (gen_result_option_combinator_call_cpp(true, receiver_expression, method_name, trailing_argument_expressions)) : (gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression)))));
}
return gen_method_dispatch_cpp(object, object_type_name, method_name, receiver_expression, trailing_argument_expressions, arguments, context, gen_stmts, evaluate_expression);
std::abort();
}();
    return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, result_or_option_code);
  }
}

} // namespace method_gen

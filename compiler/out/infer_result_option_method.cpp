#define main mlc_user_main
#include "infer_result_option_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "result_option_method_types.hpp"
#include "hof_method_spec.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_result_option_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace result_option_method_types;
using namespace hof_method_spec;
using namespace infer_lambda_context;
using namespace method_receiver_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, __1, __2] = exprLambda; return ((parameter_names.length() != expected) ? (ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((((mlc::String("method ", 7) + mlc::to_string(method_name)) + mlc::String(" expects a lambda with ", 23)) + mlc::to_string(mlc::to_string(expected))) + mlc::String(" parameter(s)", 13)), ast::expr_span(expression), diagnostic_codes::diagnostic_code_e055())})) : (errors));
}
return errors;
std::abort();
}();
}
mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept{
  return argument_types_so_far.concat(mlc::Array<std::shared_ptr<registry::Type>>{type_to_append});
}
bool is_result_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, __1] = tGeneric; return (name == mlc::String("Result", 6));
}
return false;
std::abort();
}();
}
std::shared_ptr<registry::Type> result_inner_error_from_return_type(std::shared_ptr<registry::Type> lambda_return_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = lambda_return_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return [&]() -> std::shared_ptr<registry::Type> {
  if (((name == mlc::String("Result", 6)) && (type_arguments.length() >= 2)))   {
    return type_arguments[1];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}();
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return hof_method_spec::is_result_option_hof_method(receiver_type, method_name);
}
infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  auto receiver_type = object_parsed.inferred_type;
  auto receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(receiver_type, method_name, method_span);
  auto expected_arity = result_option_method_types::result_option_hof_expected_argument_count(receiver_type, method_name);
  auto arity_errors = ((expected_arity >= 0) ? (((method_arguments.length() != expected_arity) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((((mlc::String("expected ", 9) + mlc::to_string(mlc::to_string(expected_arity))) + mlc::String(" arguments, got ", 16)) + mlc::to_string(mlc::to_string(method_arguments.length()))) + mlc::String("", 0)), method_span, diagnostic_codes::diagnostic_code_e014())}) : (mlc::Array<ast::Diagnostic>{}))) : (mlc::Array<ast::Diagnostic>{}));
  auto merged = object_parsed;
  auto argument_types = mlc::Array<std::shared_ptr<registry::Type>>{};
  if (result_option_method_types::is_result_generic(receiver_type))   {
    auto result_ok_type_value = result_option_method_types::result_ok_type(receiver_type);
    auto result_error_type_value = result_option_method_types::result_err_type(receiver_type);
    if (((method_name == mlc::String("unwrap_or", 9)) && (method_arguments.length() > 0)))     {
      auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
      (merged = infer_result::InferResult_absorb(merged, argument_parsed));
      (argument_types = append_argument_type(argument_types, argument_parsed.inferred_type));
      if ((((!semantic_type_structure::type_is_unknown(result_ok_type_value)) && (!semantic_type_structure::type_is_unknown(argument_parsed.inferred_type))) && (!semantic_type_structure::types_structurally_equal(result_ok_type_value, argument_parsed.inferred_type))))       {
        (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("method unwrap_or value must match Ok type (expected ", 52) + mlc::to_string(semantic_type_structure::type_description(result_ok_type_value))) + mlc::String(")", 1)), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e056())})});
      }
    } else if (((method_name == mlc::String("unwrap_or_else", 14)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)});
      auto lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
      if ((((!semantic_type_structure::type_is_unknown(result_ok_type_value)) && (!semantic_type_structure::type_is_unknown(lambda_return_type))) && (!semantic_type_structure::types_structurally_equal(result_ok_type_value, lambda_return_type))))       {
        (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("method unwrap_or_else must return the Ok type (expected ", 56) + mlc::to_string(semantic_type_structure::type_description(result_ok_type_value))) + mlc::String(")", 1)), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e057())})});
      }
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("map", 3)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_ok_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("map_err", 7)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("and_then", 8)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_ok_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      auto extra = lambda_arity_errors;
      auto lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
      if (is_result_type(lambda_return_type))       {
        auto callback_result_error_type = result_inner_error_from_return_type(lambda_return_type);
        if ((((!semantic_type_structure::type_is_unknown(result_error_type_value)) && (!semantic_type_structure::type_is_unknown(callback_result_error_type))) && (!semantic_type_structure::types_structurally_equal(result_error_type_value, callback_result_error_type))))         {
          (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("method and_then callback must return Result with the same error type (expected ", 79) + mlc::to_string(semantic_type_structure::type_description(result_error_type_value))) + mlc::String(")", 1)), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e058())}));
        }
      } else       {
        (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method and_then must return a Result type", 41), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e059())}));
      }
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if ((method_name == mlc::String("ok", 2)))     {
      auto _noop = 0;
    } else if (((method_name == mlc::String("or_else", 7)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    }
  } else if (result_option_method_types::is_option_generic(receiver_type))   {
    auto option_inner_type_value = result_option_method_types::option_inner_type(receiver_type);
    if (((method_name == mlc::String("unwrap_or", 9)) && (method_arguments.length() > 0)))     {
      auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
      (merged = infer_result::InferResult_absorb(merged, argument_parsed));
      (argument_types = append_argument_type(argument_types, argument_parsed.inferred_type));
      if ((((!semantic_type_structure::type_is_unknown(option_inner_type_value)) && (!semantic_type_structure::type_is_unknown(argument_parsed.inferred_type))) && (!semantic_type_structure::types_structurally_equal(option_inner_type_value, argument_parsed.inferred_type))))       {
        (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("method unwrap_or value must match Some type (expected ", 54) + mlc::to_string(semantic_type_structure::type_description(option_inner_type_value))) + mlc::String(")", 1)), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e060())})});
      }
    } else if (((method_name == mlc::String("or_else", 7)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 0, mlc::String("or_else", 7));
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 0, mlc::String("or_else", 7)));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      auto extra = lambda_arity_errors;
      auto lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
      if (result_option_method_types::is_option_generic(lambda_return_type))       {
        auto callback_option_inner_type = result_option_method_types::option_inner_type(lambda_return_type);
        if ((((!semantic_type_structure::type_is_unknown(option_inner_type_value)) && (!semantic_type_structure::type_is_unknown(callback_option_inner_type))) && (!semantic_type_structure::types_structurally_equal(option_inner_type_value, callback_option_inner_type))))         {
          (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("method or_else must return Option with the same inner type (expected ", 69) + mlc::to_string(semantic_type_structure::type_description(option_inner_type_value))) + mlc::String(")", 1)), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e061())}));
        }
      } else       {
        (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method or_else must return Option", 33), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e062())}));
      }
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("ok_or", 5)) && (method_arguments.length() > 0)))     {
      auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
      (merged = infer_result::InferResult_absorb(merged, argument_parsed));
      (argument_types = append_argument_type(argument_types, argument_parsed.inferred_type));
    } else if (((method_name == mlc::String("map", 3)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("and_then", 8)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      auto extra = lambda_arity_errors;
      auto return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
      if ((!result_option_method_types::is_option_generic(return_type)))       {
        (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method and_then must return Option", 34), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e063())}));
      }
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    } else if (((method_name == mlc::String("filter", 6)) && (method_arguments.length() > 0)))     {
      auto callback_argument = method_arguments[0];
      auto lambda_arity_errors = append_lambda_arity(mlc::Array<ast::Diagnostic>{}, callback_argument, 1, method_name);
      auto lambda_parsed = [&]() -> infer_result::InferResult {
auto __match_subject = callback_argument;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(callback_argument, inference_context);
std::abort();
}();
      (lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name));
      (merged = infer_result::InferResult_absorb(merged, lambda_parsed));
      auto extra = lambda_arity_errors;
      auto lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
      if (((!semantic_type_structure::type_is_unknown(lambda_return_type)) && (!semantic_type_structure::type_is_bool(lambda_return_type))))       {
        (extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method filter expects a predicate returning bool", 48), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e064())}));
      }
      (merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)});
      (argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type));
    }
  }
  auto hof_result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
  return infer_result::InferResult{hof_result_type, ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

} // namespace infer_result_option_method

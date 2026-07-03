#define main mlc_user_main
#include "named_args.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace named_args {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

bool expression_is_named_argument(std::shared_ptr<ast::Expr> argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, __1, __2] = exprNamedArg; return true;
}
return false;
std::abort();
}();
}
bool call_arguments_contain_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{
  return arguments.any(expression_is_named_argument);
}
int find_parameter_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept{
  return parameter_names.find_index([=](mlc::String parameter_name) mutable { return (parameter_name == sought_parameter_name); });
}
std::shared_ptr<ast::Expr> inner_expression_without_named_label(std::shared_ptr<ast::Expr> argument) noexcept{
  return [&]() -> std::shared_ptr<ast::Expr> {
auto __match_subject = argument;
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, inner_expression, __2] = exprNamedArg; return inner_expression;
}
return argument;
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Expr>> strip_named_argument_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{
  return arguments.map(inner_expression_without_named_label);
}
mlc::Array<std::shared_ptr<ast::Expr>> build_placeholder_slot_expressions(mlc::Array<mlc::String> parameter_names) noexcept{
  return parameter_names.fold([&]() {
auto empty_slot_expressions = mlc::Array<std::shared_ptr<ast::Expr>>{};
return empty_slot_expressions;
}(), [=](mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions_so_far, mlc::String _parameter_name) mutable { return slot_expressions_so_far.concat(mlc::Array<std::shared_ptr<ast::Expr>>{ast::expr_placeholder()}); });
}
mlc::Array<int> build_zero_filled_slot_flags(mlc::Array<mlc::String> parameter_names) noexcept{
  return parameter_names.fold([&]() {
auto empty_slot_filled_flags = mlc::Array<int>{};
return empty_slot_filled_flags;
}(), [=](mlc::Array<int> slot_filled_flags_so_far, mlc::String _parameter_name) mutable { return slot_filled_flags_so_far.concat(mlc::Array<int>{0}); });
}
int first_unfilled_positional_slot(mlc::Array<int> slot_filled_flags, int parameter_count, int start_at) noexcept{
  auto slot_index = start_at;
  while (((slot_index < parameter_count) && (slot_filled_flags[slot_index] == 1)))   {
    (slot_index = (slot_index + 1));
  }
  return slot_index;
}
Named_argument_reorder_state process_one_call_argument_for_named_reorder(Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument, mlc::Array<mlc::String> parameter_names, int parameter_count, bool collect_parameter_errors) noexcept{
  return [&]() -> Named_argument_reorder_state {
auto __match_subject = argument;
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [name, inner_expression, argument_span] = exprNamedArg; return [&]() {
auto slot = find_parameter_slot(parameter_names, name);
auto errors_after_named_argument = ((collect_parameter_errors && (slot == (-1))) ? (state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("unknown parameter name: ", 24) + name), argument_span, diagnostic_codes::diagnostic_code_e034())})) : ((((collect_parameter_errors && (slot >= 0)) && (state.slot_filled_flags[slot] == 1)) ? (state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("duplicate named arg: ", 21) + name), argument_span, diagnostic_codes::diagnostic_code_e035())})) : (state.errors))));
return [&]() -> Named_argument_reorder_state {
  if (((slot >= 0) && (state.slot_filled_flags[slot] == 0)))   {
    return [&]() {
auto slot_expressions = state.slot_expressions;
slot_expressions.set(slot, inner_expression);
auto slot_filled_flags = state.slot_filled_flags;
slot_filled_flags.set(slot, 1);
return Named_argument_reorder_state{slot_expressions, slot_filled_flags, errors_after_named_argument, state.positional_slot};
}();
  } else   {
    return Named_argument_reorder_state{state.slot_expressions, state.slot_filled_flags, errors_after_named_argument, state.positional_slot};
  }
}();
}();
}
return [&]() {
auto positional_slot = first_unfilled_positional_slot(state.slot_filled_flags, parameter_count, state.positional_slot);
return [&]() -> Named_argument_reorder_state {
  if ((positional_slot < parameter_count))   {
    return [&]() {
auto slot_expressions = state.slot_expressions;
slot_expressions.set(positional_slot, argument);
auto slot_filled_flags = state.slot_filled_flags;
slot_filled_flags.set(positional_slot, 1);
return Named_argument_reorder_state{slot_expressions, slot_filled_flags, state.errors, (positional_slot + 1)};
}();
  } else   {
    return state;
  }
}();
}();
std::abort();
}();
}
Compact_slot_expressions_fold_state compact_slot_expressions_fold_step(Compact_slot_expressions_fold_state state, std::shared_ptr<ast::Expr> slot_expression, mlc::Array<int> slot_filled_flags) noexcept{
  return Compact_slot_expressions_fold_state{((slot_filled_flags[state.next_slot_index] == 1) ? (state.compact_expressions.concat(mlc::Array<std::shared_ptr<ast::Expr>>{slot_expression})) : (state.compact_expressions)), (state.next_slot_index + 1)};
}
mlc::Array<std::shared_ptr<ast::Expr>> compact_filled_slot_expressions(mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions, mlc::Array<int> slot_filled_flags) noexcept{
  return slot_expressions.fold(Compact_slot_expressions_fold_state{[&]() {
auto empty_compact_expressions = mlc::Array<std::shared_ptr<ast::Expr>>{};
return empty_compact_expressions;
}(), 0}, [=](Compact_slot_expressions_fold_state fold_state, std::shared_ptr<ast::Expr> slot_expression) mutable { return compact_slot_expressions_fold_step(fold_state, slot_expression, slot_filled_flags); }).compact_expressions;
}
ResolvedNamedArguments reorder_named_arguments_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, bool collect_parameter_errors) noexcept{
  auto parameter_count = parameter_names.length();
  auto initial_state = Named_argument_reorder_state{build_placeholder_slot_expressions(parameter_names), build_zero_filled_slot_flags(parameter_names), [&]() {
auto empty_errors = mlc::Array<ast::Diagnostic>{};
return empty_errors;
}(), 0};
  auto final_state = arguments.fold(initial_state, [=](Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument) mutable { return process_one_call_argument_for_named_reorder(state, argument, parameter_names, parameter_count, collect_parameter_errors); });
  return ResolvedNamedArguments{compact_filled_slot_expressions(final_state.slot_expressions, final_state.slot_filled_flags), final_state.errors};
}
ResolvedNamedArguments resolve_named_call_arguments_for_infer(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept{
  if ((!call_arguments_contain_named_labels(arguments)))   {
    return ResolvedNamedArguments{arguments, {}};
  } else   {
    auto parameter_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
    if ((parameter_names.length() == 0))     {
      return ResolvedNamedArguments{strip_named_argument_labels(arguments), {}};
    } else     {
      return reorder_named_arguments_to_positional(arguments, parameter_names, true);
    }
  }
}
mlc::Array<std::shared_ptr<ast::Expr>> resolve_named_call_arguments_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept{
  if ((!call_arguments_contain_named_labels(arguments)))   {
    return arguments;
  } else   {
    auto parameter_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
    if ((parameter_names.length() == 0))     {
      return strip_named_argument_labels(arguments);
    } else     {
      return reorder_named_arguments_to_positional(arguments, parameter_names, false).expressions;
    }
  }
}

} // namespace named_args

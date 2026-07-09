#include "named_args.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace named_args {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct Named_argument_reorder_state {mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions;mlc::Array<int> slot_filled_flags;mlc::Array<ast::Diagnostic> errors;int positional_slot;};

struct Compact_slot_expressions_fold_state {mlc::Array<std::shared_ptr<ast::Expr>> compact_expressions;int next_slot_index;};

bool expression_is_named_argument(std::shared_ptr<ast::Expr> argument) noexcept;

bool call_arguments_contain_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

int find_parameter_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;

std::shared_ptr<ast::Expr> inner_expression_without_named_label(std::shared_ptr<ast::Expr> argument) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> strip_named_argument_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> build_placeholder_slot_expressions(mlc::Array<mlc::String> parameter_names) noexcept;

mlc::Array<int> build_zero_filled_slot_flags(mlc::Array<mlc::String> parameter_names) noexcept;

int first_unfilled_positional_slot(mlc::Array<int> slot_filled_flags, int parameter_count, int start_at) noexcept;

named_args::Named_argument_reorder_state process_one_call_argument_for_named_reorder(named_args::Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument, mlc::Array<mlc::String> parameter_names, int parameter_count, bool collect_parameter_errors) noexcept;

named_args::Compact_slot_expressions_fold_state compact_slot_expressions_fold_step(named_args::Compact_slot_expressions_fold_state state, std::shared_ptr<ast::Expr> slot_expression, mlc::Array<int> slot_filled_flags) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> compact_filled_slot_expressions(mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions, mlc::Array<int> slot_filled_flags) noexcept;

named_args::ResolvedNamedArguments reorder_named_arguments_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, bool collect_parameter_errors) noexcept;

named_args::ResolvedNamedArguments resolve_named_call_arguments_for_infer(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> resolve_named_call_arguments_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

bool expression_is_named_argument(std::shared_ptr<ast::Expr> argument) noexcept{return [&]() { if (std::holds_alternative<ast::ExprNamedArg>((*argument)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument)._); auto [_w0, _w1, _w2] = _v_exprnamedarg; return true; } return false; }();}

bool call_arguments_contain_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{return arguments.any(expression_is_named_argument);}

int find_parameter_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept{return parameter_names.find_index([sought_parameter_name](mlc::String parameter_name) mutable { return parameter_name == sought_parameter_name; });}

std::shared_ptr<ast::Expr> inner_expression_without_named_label(std::shared_ptr<ast::Expr> argument) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprNamedArg>((*argument)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return inner_expression; } return argument; }();}

mlc::Array<std::shared_ptr<ast::Expr>> strip_named_argument_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{return arguments.map(inner_expression_without_named_label);}

mlc::Array<std::shared_ptr<ast::Expr>> build_placeholder_slot_expressions(mlc::Array<mlc::String> parameter_names) noexcept{return parameter_names.fold([&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { 
  mlc::Array<std::shared_ptr<ast::Expr>> empty_slot_expressions = {};
  return empty_slot_expressions;
 }(), [](mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions_so_far, mlc::String _parameter_name) mutable { return slot_expressions_so_far.concat(mlc::Array<std::shared_ptr<ast::Expr>>{ast::expr_placeholder()}); });}

mlc::Array<int> build_zero_filled_slot_flags(mlc::Array<mlc::String> parameter_names) noexcept{return parameter_names.fold([&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_slot_filled_flags = {};
  return empty_slot_filled_flags;
 }(), [](mlc::Array<int> slot_filled_flags_so_far, mlc::String _parameter_name) mutable { return slot_filled_flags_so_far.concat(mlc::Array<int>{0}); });}

int first_unfilled_positional_slot(mlc::Array<int> slot_filled_flags, int parameter_count, int start_at) noexcept{
int slot_index = start_at;
while (slot_index < parameter_count && slot_filled_flags[slot_index] == 1){
{
slot_index = slot_index + 1;
}
}
return slot_index;
}

named_args::Named_argument_reorder_state process_one_call_argument_for_named_reorder(named_args::Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument, mlc::Array<mlc::String> parameter_names, int parameter_count, bool collect_parameter_errors) noexcept{return [&]() -> named_args::Named_argument_reorder_state { if (std::holds_alternative<ast::ExprNamedArg>((*argument)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument)._); auto [name, inner_expression, argument_span] = _v_exprnamedarg; return [&]() -> named_args::Named_argument_reorder_state { 
  int slot = find_parameter_slot(parameter_names, name);
  mlc::Array<ast::Diagnostic> errors_after_named_argument = collect_parameter_errors && slot == -1 ? state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("unknown parameter name: ") + name, argument_span, diagnostic_codes::diagnostic_code_e034())}) : collect_parameter_errors && slot >= 0 && state.slot_filled_flags[slot] == 1 ? state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("duplicate named arg: ") + name, argument_span, diagnostic_codes::diagnostic_code_e035())}) : state.errors;
  return slot >= 0 && state.slot_filled_flags[slot] == 0 ? [&]() -> named_args::Named_argument_reorder_state { 
  mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions = state.slot_expressions;
  slot_expressions.set(slot, inner_expression);
  mlc::Array<int> slot_filled_flags = state.slot_filled_flags;
  slot_filled_flags.set(slot, 1);
  return named_args::Named_argument_reorder_state{slot_expressions, slot_filled_flags, errors_after_named_argument, state.positional_slot};
 }() : named_args::Named_argument_reorder_state{state.slot_expressions, state.slot_filled_flags, errors_after_named_argument, state.positional_slot};
 }(); } return [&]() -> named_args::Named_argument_reorder_state { 
  int positional_slot = first_unfilled_positional_slot(state.slot_filled_flags, parameter_count, state.positional_slot);
  return positional_slot < parameter_count ? [&]() -> named_args::Named_argument_reorder_state { 
  mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions = state.slot_expressions;
  slot_expressions.set(positional_slot, argument);
  mlc::Array<int> slot_filled_flags = state.slot_filled_flags;
  slot_filled_flags.set(positional_slot, 1);
  return named_args::Named_argument_reorder_state{slot_expressions, slot_filled_flags, state.errors, positional_slot + 1};
 }() : state;
 }(); }();}

named_args::Compact_slot_expressions_fold_state compact_slot_expressions_fold_step(named_args::Compact_slot_expressions_fold_state state, std::shared_ptr<ast::Expr> slot_expression, mlc::Array<int> slot_filled_flags) noexcept{return named_args::Compact_slot_expressions_fold_state{slot_filled_flags[state.next_slot_index] == 1 ? state.compact_expressions.concat(mlc::Array<std::shared_ptr<ast::Expr>>{slot_expression}) : state.compact_expressions, state.next_slot_index + 1};}

mlc::Array<std::shared_ptr<ast::Expr>> compact_filled_slot_expressions(mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions, mlc::Array<int> slot_filled_flags) noexcept{return slot_expressions.fold(named_args::Compact_slot_expressions_fold_state{[&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { 
  mlc::Array<std::shared_ptr<ast::Expr>> empty_compact_expressions = {};
  return empty_compact_expressions;
 }(), 0}, [slot_filled_flags](named_args::Compact_slot_expressions_fold_state fold_state, std::shared_ptr<ast::Expr> slot_expression) mutable { return compact_slot_expressions_fold_step(fold_state, slot_expression, slot_filled_flags); }).compact_expressions;}

named_args::ResolvedNamedArguments reorder_named_arguments_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, bool collect_parameter_errors) noexcept{
int parameter_count = parameter_names.size();
named_args::Named_argument_reorder_state initial_state = named_args::Named_argument_reorder_state{build_placeholder_slot_expressions(parameter_names), build_zero_filled_slot_flags(parameter_names), [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_errors = {};
  return empty_errors;
 }(), 0};
named_args::Named_argument_reorder_state final_state = arguments.fold(initial_state, [parameter_names, parameter_count, collect_parameter_errors](named_args::Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument) mutable { return process_one_call_argument_for_named_reorder(state, argument, parameter_names, parameter_count, collect_parameter_errors); });
return named_args::ResolvedNamedArguments{compact_filled_slot_expressions(final_state.slot_expressions, final_state.slot_filled_flags), final_state.errors};
}

named_args::ResolvedNamedArguments resolve_named_call_arguments_for_infer(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept{return !call_arguments_contain_named_labels(arguments) ? named_args::ResolvedNamedArguments{arguments, {}} : [&]() -> named_args::ResolvedNamedArguments { 
  mlc::Array<mlc::String> parameter_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
  return parameter_names.size() == 0 ? named_args::ResolvedNamedArguments{strip_named_argument_labels(arguments), {}} : reorder_named_arguments_to_positional(arguments, parameter_names, true);
 }();}

mlc::Array<std::shared_ptr<ast::Expr>> resolve_named_call_arguments_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept{return !call_arguments_contain_named_labels(arguments) ? arguments : [&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { 
  mlc::Array<mlc::String> parameter_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
  return parameter_names.size() == 0 ? strip_named_argument_labels(arguments) : reorder_named_arguments_to_positional(arguments, parameter_names, false).expressions;
 }();}

} // namespace named_args

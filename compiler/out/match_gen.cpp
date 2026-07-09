#define main mlc_user_main
#include "match_gen.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "result_option_method_types.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "print.hpp"
#include "literals.hpp"

namespace match_gen {

using namespace ast;
using namespace semantic_ir;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace match_analysis;
using namespace expr;
using namespace registry;
using namespace semantic_type_structure;
using namespace result_option_method_types;
using namespace type_gen;
using namespace literals;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace print;
using namespace literals;
using namespace ast_tokens;

bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept{
  return [&]() -> bool {
auto __match_subject = match_result_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
RecordFieldBindAccum record_field_bind_step(RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern, mlc::String lower_name) noexcept{
  return [&]() -> RecordFieldBindAccum {
auto __match_subject = field_pattern;
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [field_name, __1] = patternIdent; return RecordFieldBindAccum{(accumulated.field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name)), context::CodegenContext_add_value(accumulated.arm_context, field_name)};
}
return accumulated;
std::abort();
}();
}
RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept{
  return field_patterns.fold(RecordFieldBindAccum{mlc::String("", 0), start_context}, [=](RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern) mutable { return record_field_bind_step(accumulated, field_pattern, lower_name); });
}
context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, context::CodegenContext base_context) noexcept{
  auto ctor_type_information = context::lookup_ctor_type_info_for_context(base_context, constructor_name);
  auto arm_context = base_context;
  auto pattern_index = 0;
  while ((pattern_index < sub_patterns.length()))   {
    [&]() {
auto __match_subject = sub_patterns[pattern_index];
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [pattern_name, __1] = patternIdent; [&]() {
if (ctor_type_information->shared_pos.contains(pattern_index)) {
  (arm_context = context::CodegenContext_add_shared(arm_context, pattern_name));
}
if (ctor_type_information->shared_arr_pos.contains(pattern_index)) {
  (arm_context = context::CodegenContext_add_shared_array(arm_context, pattern_name));
}
if (((!ctor_type_information->shared_pos.contains(pattern_index)) && (!ctor_type_information->shared_arr_pos.contains(pattern_index)))) {
  (arm_context = context::CodegenContext_add_value(arm_context, pattern_name));
}
return std::make_tuple();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
    (pattern_index = (pattern_index + 1));
  }
  return arm_context;
}
mlc::String gen_arm_wild_like_body(std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm_body, context, gen_stmts));
  } else   {
    return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm_body, context, gen_stmts));
  }
}
mlc::String gen_arm_ident_body(mlc::String binding_name, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_binding_identifier_void(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts));
  } else   {
    return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts));
  }
}
mlc::String gen_arm_ctor_generic_body(mlc::String qualified_name, mlc::String lower_name, mlc::String binding, mlc::String body_code, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_constructed_value_generic_void(qualified_name, lower_name, binding, body_code);
  } else   {
    return expr::match_arm_constructed_value_generic(qualified_name, lower_name, binding, body_code);
  }
}
mlc::String gen_arm_ctor_record_body(mlc::String parameter, mlc::String binding, mlc::String body_code, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_constructed_value_void(parameter, binding, body_code);
  } else   {
    return expr::match_arm_constructed_value(parameter, binding, body_code);
  }
}
mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, constructor_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
  auto binding = [&]() -> mlc::String {
  if ((sub_patterns.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
  }
}();
  auto is_generic = decl_index::list_contains(context.generic_variants, constructor_name);
  auto arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
  auto body_code = eval_expr_fn(arm_body, arm_context, gen_stmts);
  if (is_generic)   {
    return gen_arm_ctor_generic_body(qualified_name, lower_name, binding, body_code, visit_void);
  } else   {
    auto parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String("", 0), lower_name);
    return gen_arm_ctor_record_body(parameter, binding, body_code, visit_void);
  }
}
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, record_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
  auto is_generic = decl_index::list_contains(context.generic_variants, record_name);
  auto accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
  auto body_code = eval_expr_fn(arm_body, accumulated.arm_context, gen_stmts);
  if (is_generic)   {
    return gen_arm_ctor_generic_body(qualified_name, lower_name, accumulated.field_bindings, body_code, visit_void);
  } else   {
    auto parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String("", 0), lower_name);
    return gen_arm_ctor_record_body(parameter, accumulated.field_bindings, body_code, visit_void);
  }
}
std::shared_ptr<semantic_ir::SemanticMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept{
  return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> {
auto __match_subject = arm->pattern;
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.map([=](std::shared_ptr<ast::Pattern> alternative) mutable { return match_arm_with_pattern(arm, alternative); });
}
return mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{arm};
std::abort();
}();
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{
  return arms.flat_map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return singleton_or_split(arm); });
}
bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept{
  return expanded_arms.any([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return arm->has_guard; });
}
bool pattern_is_wildcard(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return true;
}
return false;
std::abort();
}();
}
bool expanded_any_wildcard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept{
  return expanded_arms.any([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_is_wildcard(arm->pattern); });
}
std::shared_ptr<registry::Type> match_effective_result_type(std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context) noexcept{
  if (result_option_method_types::is_result_generic(match_result_type))   {
    return match_result_type;
  } else if (result_option_method_types::is_result_generic(context.enclosing_function_return_type))   {
    return context.enclosing_function_return_type;
  } else   {
    return match_result_type;
  }
}
bool is_plain_named_non_result_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return (name != mlc::String("Result", 6));
}
return false;
std::abort();
}();
}
bool should_use_explicit_visit_return_type(std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{
  if (result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)))   {
    return true;
  }
  auto effective_result_type = match_effective_result_type(match_result_type, context);
  auto return_cpp = match_return_cpp_type(context, effective_result_type, subject);
  if ((return_cpp.length() == 0))   {
    return false;
  }
  if (is_plain_named_non_result_type(effective_result_type))   {
    return true;
  }
  auto ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(effective_result_type));
  auto subject_cpp = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(subject));
  if (((ok_cpp == mlc::String("auto", 4)) || (subject_cpp == mlc::String("auto", 4))))   {
    return return_cpp.contains(mlc::String("Result", 6));
  } else   {
    return (ok_cpp != subject_cpp);
  }
}
mlc::String guarded_literal_condition(std::shared_ptr<ast::Pattern> pattern, mlc::String variant_access_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternBool>((*__match_subject))) {
const ast::PatternBool& patternBool = std::get<ast::PatternBool>((*__match_subject));
auto [value, __1] = patternBool; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_boolean_literal(value))) + mlc::String(")", 1));
}
if (std::holds_alternative<ast::PatternInt>((*__match_subject))) {
const ast::PatternInt& patternInt = std::get<ast::PatternInt>((*__match_subject));
auto [value, __1] = patternInt; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_integer_literal(value))) + mlc::String(")", 1));
}
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [value, __1] = patternStr; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_string_literal(value))) + mlc::String(")", 1));
}
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [value, __1] = patternStringLit; return ((((mlc::String("(", 1) + mlc::to_string(variant_access_expression)) + mlc::String(" == ", 4)) + mlc::to_string(literals::gen_string_literal(value))) + mlc::String(")", 1));
}
return mlc::String("false", 5);
std::abort();
}();
}
mlc::String return_or_discard_line(bool use_void, mlc::String body_code) noexcept{
  if (use_void)   {
    return ((mlc::String("", 0) + mlc::to_string(body_code)) + mlc::String(";\nreturn;\n", 10));
  } else   {
    return expr::return_line(body_code);
  }
}
mlc::String guarded_body_fragment(bool has_guard, mlc::String guard_code, bool use_void, mlc::String body_code) noexcept{
  if (has_guard)   {
    return (expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("\n", 1));
  } else   {
    return return_or_discard_line(use_void, body_code);
  }
}
mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto body_code = eval_expr_fn(arm->body, context, gen_stmts);
  if (arm->has_guard)   {
    auto guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
    return (expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("\n", 1));
  } else   {
    return return_or_discard_line(use_void, body_code);
  }
}
mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto condition = guarded_literal_condition(arm->pattern, variant_access_expression);
  auto body_code = eval_expr_fn(arm->body, context, gen_stmts);
  if (arm->has_guard)   {
    auto guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
    auto combined_condition = ((((mlc::String("(", 1) + mlc::to_string(condition)) + mlc::String(" && (", 5)) + mlc::to_string(guard_code)) + mlc::String("))", 2));
    return (expr::if_brace_block(combined_condition, return_or_discard_line(use_void, body_code)) + mlc::String("\n", 1));
  } else   {
    return (expr::if_brace_block(condition, return_or_discard_line(use_void, body_code)) + mlc::String("\n", 1));
  }
}
mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto binding_cpp_safe = cpp_naming::cpp_safe(binding_name);
  auto extended_context = context::CodegenContext_add_value(context, binding_name);
  auto binding_statement = ((((mlc::String("auto ", 5) + mlc::to_string(binding_cpp_safe)) + mlc::String(" = ", 3)) + mlc::to_string(subject_holder_name)) + mlc::String(";\n", 2));
  auto body_code = eval_expr_fn(arm->body, extended_context, gen_stmts);
  if (arm->has_guard)   {
    auto guard_code = eval_expr_fn(arm->when_condition, extended_context, gen_stmts);
    return (((mlc::String("{\n", 2) + binding_statement) + expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code))) + mlc::String("}\n", 2));
  } else   {
    return (((mlc::String("{\n", 2) + binding_statement) + return_or_discard_line(use_void, body_code)) + mlc::String("}\n", 2));
  }
}
mlc::String generic_variant_type_argument(context::CodegenContext context, mlc::String constructor_name) noexcept{
  if (decl_index::list_contains(context.generic_variants, constructor_name))   {
    return mlc::String("<auto>", 6);
  } else   {
    return mlc::String("", 0);
  }
}
int type_parameter_name_index(mlc::Array<mlc::String> parameter_names, mlc::String parameter_name) noexcept{
  auto index = 0;
  while ((index < parameter_names.length()))   {
    if ((parameter_names[index] == parameter_name))     {
      return index;
    }
    (index = (index + 1));
  }
  return (-1);
}
mlc::String cpp_angle_bracket_type_arguments(mlc::Array<mlc::String> type_argument_parts) noexcept{
  if ((type_argument_parts.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return ((mlc::String("<", 1) + mlc::to_string(type_argument_parts.join(mlc::String(", ", 2)))) + mlc::String(">", 1));
  }
}
mlc::String instantiated_variant_type_argument_from_maps(context::CodegenContext context, mlc::String owner_name, mlc::String constructor_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{
  auto all_parameter_names = context.sum_type_parameter_names.get(owner_name);
  auto used_parameter_names = context.variant_used_type_parameter_names.get(constructor_name);
  auto cpp_argument_parts = mlc::Array<mlc::String>{};
  auto used_index = 0;
  while ((used_index < used_parameter_names.length()))   {
    auto parameter_index = type_parameter_name_index(all_parameter_names, used_parameter_names[used_index]);
    if (((parameter_index < 0) || (parameter_index >= type_arguments.length())))     {
      return mlc::String("", 0);
    }
    if (semantic_type_structure::type_is_unknown(type_arguments[parameter_index]))     {
      return mlc::String("", 0);
    }
    cpp_argument_parts.push_back(type_gen::sem_type_to_cpp(context, type_arguments[parameter_index]));
    (used_index = (used_index + 1));
  }
  return cpp_angle_bracket_type_arguments(cpp_argument_parts);
}
mlc::String generic_subject_type_name(std::shared_ptr<registry::Type> subject_type) noexcept{
  return semantic_type_structure::generic_type_name_from_type(subject_type);
}
mlc::Array<std::shared_ptr<registry::Type>> generic_subject_type_arguments(std::shared_ptr<registry::Type> subject_type) noexcept{
  return semantic_type_structure::generic_type_arguments_from_type(subject_type);
}
mlc::String instantiated_variant_type_argument_from_generic_subject(context::CodegenContext context, mlc::String constructor_name, mlc::String sum_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{
  if ((context.variant_types.has(constructor_name) && (context.variant_types.get(constructor_name) != sum_name)))   {
    return mlc::String("", 0);
  } else if ((context.variant_used_type_parameter_names.has(constructor_name) && (context.variant_used_type_parameter_names.get(constructor_name).length() == 0)))   {
    return mlc::String("", 0);
  } else if ((context.variant_used_type_parameter_names.has(constructor_name) && context.sum_type_parameter_names.has(sum_name)))   {
    return instantiated_variant_type_argument_from_maps(context, sum_name, constructor_name, type_arguments);
  } else if ((((type_arguments.length() == 1) && (!semantic_type_structure::type_is_unknown(type_arguments[0]))) && (!(context.variant_used_type_parameter_names.has(constructor_name) && (context.variant_used_type_parameter_names.get(constructor_name).length() == 0)))))   {
    return cpp_angle_bracket_type_arguments(mlc::Array<mlc::String>{type_gen::sem_type_to_cpp(context, type_arguments[0])});
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String instantiated_variant_type_argument_from_subject(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{
  auto sum_name = generic_subject_type_name(subject_type);
  if ((sum_name == mlc::String("", 0)))   {
    return mlc::String("", 0);
  } else   {
    return instantiated_variant_type_argument_from_generic_subject(context, constructor_name, sum_name, generic_subject_type_arguments(subject_type));
  }
}
mlc::String result_ctor_cpp_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{
  if ((constructor_name == mlc::String("Ok", 2)))   {
    return ((mlc::String("<", 1) + mlc::to_string(type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(subject_type)))) + mlc::String(">", 1));
  } else if ((constructor_name == mlc::String("Err", 3)))   {
    return ((mlc::String("<", 1) + mlc::to_string(type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(subject_type)))) + mlc::String(">", 1));
  } else   {
    return mlc::String("<auto>", 6);
  }
}
mlc::String generic_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  if (result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)))   {
    return result_ctor_cpp_type_argument(context, constructor_name, semantic_ir::sexpr_type(subject));
  } else   {
    return non_result_ctor_type_argument(context, constructor_name, semantic_ir::sexpr_type(subject));
  }
}
mlc::String non_result_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{
  auto instantiated = instantiated_variant_type_argument_from_subject(context, constructor_name, subject_type);
  if ((instantiated.length() > 0))   {
    return instantiated;
  } else   {
    return generic_variant_type_argument(context, constructor_name);
  }
}
mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, constructor_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
  auto binding_prefix = [&]() -> mlc::String {
  if ((sub_patterns.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
  }
}();
  auto type_argument = generic_ctor_type_argument(context, constructor_name, subject);
  auto arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
  auto holds_check = ((((((mlc::String("std::holds_alternative<", 23) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2)) + mlc::to_string(variant_access_expression)) + mlc::String(")", 1));
  auto open_block = ((((((mlc::String("if (", 4) + mlc::to_string(holds_check)) + mlc::String(") {\n", 4)) + ((((((((((mlc::String("const ", 6) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String("& ", 2)) + mlc::to_string(lower_name)) + mlc::String(" = std::get<", 12)) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2))) + variant_access_expression) + mlc::String(");\n", 3)) + binding_prefix);
  auto body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
  auto guard_code = (arm->has_guard ? (eval_expr_fn(arm->when_condition, arm_context, gen_stmts)) : (mlc::String("", 0)));
  auto inner = guarded_body_fragment(arm->has_guard, guard_code, use_void, body_code);
  return ((open_block + inner) + mlc::String("}\n", 2));
}
mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, record_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
  auto type_argument = (decl_index::list_contains(context.generic_variants, record_name) ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
  auto accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
  auto holds_check = ((((((mlc::String("std::holds_alternative<", 23) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2)) + mlc::to_string(variant_access_expression)) + mlc::String(")", 1));
  auto open_block = ((((((mlc::String("if (", 4) + mlc::to_string(holds_check)) + mlc::String(") {\n", 4)) + ((((((((((mlc::String("const ", 6) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String("& ", 2)) + mlc::to_string(lower_name)) + mlc::String(" = std::get<", 12)) + mlc::to_string(qualified_name)) + mlc::String("", 0)) + mlc::to_string(type_argument)) + mlc::String(">(", 2))) + variant_access_expression) + mlc::String(");\n", 3)) + accumulated.field_bindings);
  auto body_code = eval_expr_fn(arm->body, accumulated.arm_context, gen_stmts);
  auto guard_code = (arm->has_guard ? (eval_expr_fn(arm->when_condition, accumulated.arm_context, gen_stmts)) : (mlc::String("", 0)));
  auto inner = guarded_body_fragment(arm->has_guard, guard_code, use_void, body_code);
  return ((open_block + inner) + mlc::String("}\n", 2));
}
mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  return std::visit(overloaded{[&](const ast::PatternCtor& patternCtor) { auto [name, sub_patterns, __2] = patternCtor; return gen_guarded_constructor_arm_fragment(arm, name, sub_patterns, variant_access_expression, subject, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternRecord& patternRecord) { auto [name, field_patterns, __2] = patternRecord; return gen_guarded_record_arm_fragment(arm, name, field_patterns, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return gen_guarded_wild_or_unit_arm_fragment(arm, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return gen_guarded_wild_or_unit_arm_fragment(arm, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternIdent& patternIdent) { auto [binding_name, __1] = patternIdent; return gen_guarded_identifier_binding_arm_fragment(arm, binding_name, subject_holder_name, use_void, context, gen_stmts, eval_expr_fn); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return mlc::String("", 0); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return mlc::String("", 0); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return mlc::String("", 0); }
}, (*arm->pattern));
}
std::shared_ptr<cpp_ast::CppExpression> string_match_literal_condition_cpp(std::shared_ptr<ast::Pattern> pattern, mlc::String subject_holder) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [value, __1] = patternStr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("==", 2), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{subject_holder}), literals::gen_string_literal_cpp(value)});
}
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [value, __1] = patternStringLit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("==", 2), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{subject_holder}), literals::gen_string_literal_cpp(value)});
}
return emit_helpers::make_boolean_cpp_expression(false);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
  return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_return_cpp_statement(eval_expr_cpp_fn(arm->body, context, gen_stmts))});
}
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_string(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_fragment_cpp_statement(expr::return_line(eval_expr_fn(arm->body, context, gen_stmts)))});
}
std::shared_ptr<cpp_ast::CppStatement> string_match_arm_else_branch(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int start_index, mlc::String subject_holder, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppStatement>(std::shared_ptr<semantic_ir::SemanticMatchArm>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> build_return_block) noexcept{
  if ((start_index >= arms.length()))   {
    return emit_helpers::make_expression_cpp_statement(emit_helpers::make_identifier_cpp_expression(mlc::String("std::abort()", 12)));
  } else   {
    auto arm = arms[start_index];
    auto return_block = build_return_block(arm, context, gen_stmts);
    return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = arm->pattern;
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return return_block;
}
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [__0, __1] = patternStr; return emit_helpers::make_if_cpp_statement(string_match_literal_condition_cpp(arm->pattern, subject_holder), return_block, string_match_arm_else_branch(arms, (start_index + 1), subject_holder, context, gen_stmts, build_return_block));
}
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [__0, __1] = patternStringLit; return emit_helpers::make_if_cpp_statement(string_match_literal_condition_cpp(arm->pattern, subject_holder), return_block, string_match_arm_else_branch(arms, (start_index + 1), subject_holder, context, gen_stmts, build_return_block));
}
return return_block;
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
  auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  statements.push_back(emit_helpers::make_auto_cpp_statement(mlc::String("__match_subject", 15), eval_expr_cpp_fn(subject, context, gen_stmts)));
  statements.push_back(string_match_arm_else_branch(expanded_arms, 0, mlc::String("__match_subject", 15), context, gen_stmts, [=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext arm_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return string_match_return_block_cpp(arm, arm_context, gen_stmts_fn, eval_expr_cpp_fn); }));
  return statements;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  statements.push_back(emit_helpers::make_fragment_cpp_statement(((mlc::String("auto __match_subject = ", 23) + mlc::to_string(eval_expr_fn(subject, context, gen_stmts))) + mlc::String(";\n", 2))));
  statements.push_back(string_match_arm_else_branch(expanded_arms, 0, mlc::String("__match_subject", 15), context, gen_stmts, [=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext arm_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return string_match_return_block_string(arm, arm_context, gen_stmts_fn, eval_expr_fn); }));
  return statements;
}
mlc::String string_match_statements_to_source(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  auto body_source = mlc::String("", 0);
  auto statement_index = 0;
  while ((statement_index < statements.length()))   {
    (body_source = ((body_source + print::print_statement(statements[statement_index])) + mlc::String("\n", 1)));
    (statement_index = (statement_index + 1));
  }
  return body_source;
}
mlc::String gen_string_match_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  return string_match_statements_to_source(string_match_statements_string(subject, expanded_arms, context, gen_stmts, eval_expr_fn));
}
mlc::String gen_match_string_literal_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
  auto inner_body = gen_string_match_body(subject, expanded_arms, context, gen_stmts, eval_expr_fn);
  return ((((mlc::String("[&]() -> ", 9) + mlc::to_string(return_cpp)) + mlc::String(" {\n", 3)) + mlc::to_string(inner_body)) + mlc::String("}()", 3));
}
std::shared_ptr<cpp_ast::CppExpression> gen_match_string_literal_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
  auto return_cpp = match_return_cpp_type(context, match_result_type, subject);
  auto inner_body = string_match_statements_to_source(string_match_statements_cpp(subject, expanded_arms, context, gen_stmts, eval_expr_cpp_fn));
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn{return_cpp, inner_body});
}
mlc::String full_result_cpp_template(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
  if ((!result_option_method_types::is_result_generic(semantic_type)))   {
    return mlc::String("", 0);
  }
  auto ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(semantic_type));
  auto err_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(semantic_type));
  if (((ok_cpp != mlc::String("auto", 4)) && (err_cpp != mlc::String("auto", 4))))   {
    return expr::cpp_template_two_type_arguments(mlc::String("ast::Result", 11), ok_cpp, err_cpp);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String expression_result_cpp_type_for_codegen(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
  auto direct = type_gen::sem_type_to_cpp(context, semantic_type);
  if (direct.contains(mlc::String("<", 1)))   {
    return direct;
  }
  if ((direct != mlc::String("ast::Result", 11)))   {
    return direct;
  }
  auto from_enclosing = full_result_cpp_template(context, context.enclosing_function_return_type);
  if ((from_enclosing != mlc::String("", 0)))   {
    return from_enclosing;
  }
  auto from_type = full_result_cpp_template(context, semantic_type);
  if ((from_type != mlc::String("", 0)))   {
    return from_type;
  }
  return direct;
}
mlc::String result_ok_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
  auto from_match = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(match_result_type));
  if ((from_match != mlc::String("auto", 4)))   {
    return from_match;
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String result_ok_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
  if ((!result_option_method_types::is_result_generic(match_result_type)))   {
    return mlc::String("", 0);
  } else   {
    return result_ok_type_cpp_from_generic(context, match_result_type);
  }
}
mlc::String result_err_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
  auto from_match = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(match_result_type));
  if ((from_match != mlc::String("auto", 4)))   {
    return from_match;
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String result_err_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
  if ((!result_option_method_types::is_result_generic(match_result_type)))   {
    return mlc::String("", 0);
  } else   {
    return result_err_type_cpp_from_generic(context, match_result_type);
  }
}
mlc::String match_ok_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::String direct) noexcept{
  auto from_result = result_ok_type_cpp_if_generic(context, match_result_type);
  if ((from_result != mlc::String("", 0)))   {
    return from_result;
  } else if (((((direct != mlc::String("ast::Result", 11)) && (direct != mlc::String("void", 4))) && (direct != mlc::String("auto", 4))) && (direct.length() > 0)))   {
    return direct;
  } else if (result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)))   {
    return type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(semantic_ir::sexpr_type(subject)));
  } else   {
    return mlc::String("auto", 4);
  }
}
mlc::String match_err_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  auto from_result = result_err_type_cpp_if_generic(context, match_result_type);
  if ((from_result != mlc::String("", 0)))   {
    return from_result;
  } else if (result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)))   {
    return type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(semantic_ir::sexpr_type(subject)));
  } else   {
    return mlc::String("auto", 4);
  }
}
bool cpp_type_string_has_auto_placeholder(mlc::String type_string) noexcept{
  return (((((type_string == mlc::String("auto", 4)) || type_string.contains(mlc::String("<auto>", 6))) || type_string.contains(mlc::String("<auto,", 6))) || type_string.contains(mlc::String(", auto>", 7))) || type_string.contains(mlc::String(", auto,", 7)));
}
bool is_bare_result_cpp_name(mlc::String type_string) noexcept{
  return ((type_string == mlc::String("ast::Result", 11)) || (type_string == mlc::String("Result", 6)));
}
mlc::String match_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  auto direct = type_gen::sem_type_to_cpp(context, match_result_type);
  auto direct_is_incomplete = cpp_type_string_has_auto_placeholder(direct);
  auto enclosing_cpp = type_gen::sem_type_to_cpp(context, context.enclosing_function_return_type);
  auto enclosing_usable = (((((enclosing_cpp != mlc::String("auto", 4)) && (enclosing_cpp != mlc::String("void", 4))) && (enclosing_cpp.length() > 0)) && (!cpp_type_string_has_auto_placeholder(enclosing_cpp))) && (!result_option_method_types::is_result_generic(context.enclosing_function_return_type)));
  if ((direct.contains(mlc::String("<", 1)) && (!direct_is_incomplete)))   {
    return direct;
  } else if (is_bare_result_cpp_name(direct))   {
    auto from_match = full_result_cpp_template(context, match_result_type);
    if ((from_match != mlc::String("", 0)))     {
      return from_match;
    } else     {
      auto from_enclosing = full_result_cpp_template(context, context.enclosing_function_return_type);
      if ((from_enclosing != mlc::String("", 0)))       {
        return from_enclosing;
      } else       {
        return direct;
      }
    }
  } else if (((((direct != mlc::String("auto", 4)) && (direct != mlc::String("", 0))) && (!direct_is_incomplete)) && (!result_option_method_types::is_result_generic(match_result_type))))   {
    return direct;
  } else if ((enclosing_usable && ((((direct == mlc::String("auto", 4)) || direct_is_incomplete) || (direct == mlc::String("ast::Result", 11))) || result_option_method_types::is_result_generic(match_result_type))))   {
    return enclosing_cpp;
  } else   {
    auto ok_cpp = match_ok_cpp_type(context, match_result_type, subject, direct);
    auto err_cpp = match_err_cpp_type(context, match_result_type, subject);
    if (((ok_cpp != mlc::String("auto", 4)) && (err_cpp != mlc::String("auto", 4))))     {
      return expr::cpp_template_two_type_arguments(mlc::String("ast::Result", 11), ok_cpp, err_cpp);
    } else if (direct_is_incomplete)     {
      return mlc::String("auto", 4);
    } else     {
      return direct;
    }
  }
}
mlc::String result_template_from_match_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
  if ((!result_option_method_types::is_result_generic(match_result_type)))   {
    return mlc::String("", 0);
  } else   {
    auto ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(match_result_type));
    auto err_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(match_result_type));
    if (((ok_cpp != mlc::String("auto", 4)) && (err_cpp != mlc::String("auto", 4))))     {
      return expr::cpp_template_two_type_arguments(mlc::String("ast::Result", 11), ok_cpp, err_cpp);
    } else     {
      return mlc::String("", 0);
    }
  }
}
mlc::String match_expression_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  auto enclosing_cpp = type_gen::sem_type_to_cpp(context, context.enclosing_function_return_type);
  if (((((enclosing_cpp != mlc::String("auto", 4)) && (enclosing_cpp != mlc::String("void", 4))) && (enclosing_cpp.length() > 0)) && (!result_option_method_types::is_result_generic(context.enclosing_function_return_type))))   {
    return enclosing_cpp;
  } else   {
    auto direct = type_gen::sem_type_to_cpp(context, match_result_type);
    if (direct.contains(mlc::String("<", 1)))     {
      return direct;
    } else if (((direct == mlc::String("ast::Result", 11)) && (full_result_cpp_template(context, context.enclosing_function_return_type) != mlc::String("", 0))))     {
      return full_result_cpp_template(context, context.enclosing_function_return_type);
    } else if ((((((enclosing_cpp != mlc::String("auto", 4)) && (enclosing_cpp != mlc::String("void", 4))) && (enclosing_cpp.length() > 0)) && (!result_option_method_types::is_result_generic(context.enclosing_function_return_type))) && ((direct == mlc::String("ast::Result", 11)) || result_option_method_types::is_result_generic(match_result_type))))     {
      return enclosing_cpp;
    } else if ((result_template_from_match_type(context, match_result_type) != mlc::String("", 0)))     {
      return result_template_from_match_type(context, match_result_type);
    } else     {
      auto err_cpp = match_err_cpp_type(context, match_result_type, subject);
      if ((((((direct != mlc::String("ast::Result", 11)) && (direct != mlc::String("void", 4))) && (direct != mlc::String("auto", 4))) && (direct.length() > 0)) && (err_cpp != mlc::String("auto", 4))))       {
        return expr::cpp_template_two_type_arguments(mlc::String("ast::Result", 11), direct, err_cpp);
      } else       {
        return direct;
      }
    }
  }
}
mlc::String gen_match_guarded_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto subject_code = eval_expr_fn(subject, context, gen_stmts);
  auto needs_dereference = ((expanded_arms.length() > 0) ? (match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context)) : (true));
  auto variant_access = expr::visit_subject_for_match(mlc::String("__match_subject", 15), needs_dereference);
  auto use_void = match_visit_uses_void_lambdas(match_result_type);
  auto fragment_lines = expanded_arms.map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_guarded_arm_fragment(arm, variant_access, mlc::String("__match_subject", 15), subject, use_void, context, gen_stmts, eval_expr_fn); });
  return ((((mlc::String("auto __match_subject = ", 23) + mlc::to_string(subject_code)) + mlc::String(";\n", 2)) + fragment_lines.join(mlc::String("", 0))) + mlc::String("std::abort();\n", 14));
}
mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto inner_body = gen_match_guarded_body(subject, expanded_arms, match_result_type, context, gen_stmts, eval_expr_fn);
  if (match_visit_uses_void_lambdas(match_result_type))   {
    return ((mlc::String("[&]() {\n", 8) + mlc::to_string(inner_body)) + mlc::String("}()", 3));
  } else   {
    auto return_cpp = match_return_cpp_type(context, match_result_type, subject);
    return ((((mlc::String("[&]() -> ", 9) + mlc::to_string(return_cpp)) + mlc::String(" {\n", 3)) + mlc::to_string(inner_body)) + mlc::String("}()", 3));
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
  auto inner_body = gen_match_guarded_body(subject, expanded_arms, match_result_type, context, gen_stmts, [=](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext eval_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return print::print_expr(eval_expr_cpp_fn(expression, eval_context, gen_stmts_fn)); });
  if (match_visit_uses_void_lambdas(match_result_type))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock{inner_body});
  } else   {
    auto return_cpp = match_return_cpp_type(context, match_result_type, subject);
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn{return_cpp, inner_body});
  }
}
mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  return std::visit(overloaded{[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternIdent& patternIdent) { auto [name, __1] = patternIdent; return gen_arm_ident_body(name, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternCtor& patternCtor) { auto [name, sub_patterns, __2] = patternCtor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternRecord& patternRecord) { auto [name, field_patterns, __2] = patternRecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); }
}, (*arm->pattern));
}
std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, constructor_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
  auto binding = [&]() -> mlc::String {
  if ((sub_patterns.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
  }
}();
  auto type_argument = generic_ctor_type_argument(context, constructor_name, subject);
  auto variant_used_type_parameter_names = [&]() -> auto {
  if (context.variant_used_type_parameter_names.has(constructor_name))   {
    return context.variant_used_type_parameter_names.get(constructor_name);
  } else   {
    return [&]() {
auto empty_names = mlc::Array<mlc::String>{};
return empty_names;
}();
  }
}();
  auto effective_type_argument = (((type_argument == mlc::String("<auto>", 6)) && (variant_used_type_parameter_names.length() == 0)) ? (mlc::String("", 0)) : (((((type_argument.length() > 0) && (variant_used_type_parameter_names.length() == 0)) && (sub_patterns.length() == 0)) ? (mlc::String("", 0)) : (type_argument))));
  auto use_generic_lambda = ((decl_index::list_contains(context.generic_variants, constructor_name) && (variant_used_type_parameter_names.length() > 0)) && ((effective_type_argument == mlc::String("<auto>", 6)) || (effective_type_argument == mlc::String("", 0))));
  auto arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
  auto body_expression = eval_expr_cpp_fn(arm_body, arm_context, gen_stmts);
  if (use_generic_lambda)   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric{qualified_name, lower_name, binding, visit_void, return_cpp, body_expression});
  } else   {
    auto parameter = expr::match_lambda_const_reference_parameter(qualified_name, effective_type_argument, lower_name);
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed{parameter, binding, visit_void, return_cpp, body_expression});
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
  auto qualified_name = context::CodegenContext_resolve(context, record_name);
  auto lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
  auto is_generic = decl_index::list_contains(context.generic_variants, record_name);
  auto accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
  auto body_expression = eval_expr_cpp_fn(arm_body, accumulated.arm_context, gen_stmts);
  if (is_generic)   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric{qualified_name, lower_name, accumulated.field_bindings, visit_void, return_cpp, body_expression});
  } else   {
    auto parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String("", 0), lower_name);
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed{parameter, accumulated.field_bindings, visit_void, return_cpp, body_expression});
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
  return std::visit(overloaded{[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternIdent& patternIdent) { auto [name, __1] = patternIdent; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmBinding{cpp_naming::cpp_safe(name), visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context::CodegenContext_add_value(context, name), gen_stmts)}); },
[&](const ast::PatternCtor& patternCtor) { auto [name, sub_patterns, __2] = patternCtor; return gen_arm_ctor_cpp(name, sub_patterns, arm->body, subject, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); },
[&](const ast::PatternRecord& patternRecord) { auto [name, field_patterns, __2] = patternRecord; return gen_arm_record_pattern_cpp(name, field_patterns, arm->body, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild{visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts)}); }
}, (*arm->pattern));
}
mlc::String gen_match_std_visit_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
  auto arm_lines = expanded_arms.map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_arm(arm, context, gen_stmts, eval_expr_fn, visit_void); });
  auto subject_code = eval_expr_fn(subject, context, gen_stmts);
  auto needs_dereference = ((expanded_arms.length() > 0) ? (match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context)) : (true));
  return expr::std_visit_match_expression(arm_lines.join(mlc::String(",\n", 2)), expr::visit_subject_for_match(subject_code, needs_dereference));
}
std::shared_ptr<cpp_ast::CppExpression> gen_match_std_visit_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
  auto visit_void = match_visit_uses_void_lambdas(match_result_type);
  auto effective_result_type = match_effective_result_type(match_result_type, context);
  auto return_cpp = ((visit_void || (!should_use_explicit_visit_return_type(match_result_type, subject, context))) ? (mlc::String("", 0)) : (match_return_cpp_type(context, effective_result_type, subject)));
  auto arm_expressions = expanded_arms.map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_arm_cpp(arm, subject, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); });
  auto needs_dereference = ((expanded_arms.length() > 0) ? (match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context)) : (true));
  auto subject_code = print::print_expr(eval_expr_cpp_fn(subject, context, gen_stmts));
  auto visit_subject = expr::visit_subject_for_match(subject_code, needs_dereference);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{expr::std_visit_match_expression(arm_expressions.map([=](std::shared_ptr<cpp_ast::CppExpression> arm) mutable { return print::print_expr(arm); }).join(mlc::String(",\n", 2)), visit_subject)});
}
mlc::String gen_match(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto expanded = expand_or_arms(arms);
  auto use_void_visit = match_visit_uses_void_lambdas(match_result_type);
  if (expanded_any_guard(expanded))   {
    return gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn);
  } else if (match_analysis::should_use_string_match_if_chain(subject, expanded))   {
    return gen_match_string_literal_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn);
  } else if (expanded_any_wildcard(expanded))   {
    return gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn);
  } else if (use_void_visit)   {
    return gen_match_std_visit_string(subject, expanded, context, gen_stmts, eval_expr_fn, true);
  } else   {
    return gen_match_std_visit_string(subject, expanded, context, gen_stmts, eval_expr_fn, false);
  }
}

} // namespace match_gen

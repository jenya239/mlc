#ifndef MATCH_GEN_HPP
#define MATCH_GEN_HPP

#include "mlc.hpp"
#include <variant>
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

struct RecordFieldBindAccum {
  mlc::String field_bindings;
  context::CodegenContext arm_context;
};
bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept;
RecordFieldBindAccum record_field_bind_step(RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern, mlc::String lower_name) noexcept;
RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept;
context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, context::CodegenContext base_context) noexcept;
template<typename __F3>
mlc::String gen_arm_wild_like_body(std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn, bool visit_void) noexcept;
template<typename __F4>
mlc::String gen_arm_ident_body(mlc::String binding_name, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn, bool visit_void) noexcept;
mlc::String gen_arm_ctor_generic_body(mlc::String qualified_name, mlc::String lower_name, mlc::String binding, mlc::String body_code, bool visit_void) noexcept;
mlc::String gen_arm_ctor_record_body(mlc::String parameter, mlc::String binding, mlc::String body_code, bool visit_void) noexcept;
template<typename __F5>
mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn, bool visit_void) noexcept;
template<typename __F5>
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn, bool visit_void) noexcept;
std::shared_ptr<semantic_ir::SemanticMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;
bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept;
bool pattern_is_wildcard(std::shared_ptr<ast::Pattern> pattern) noexcept;
bool expanded_any_wildcard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept;
std::shared_ptr<registry::Type> match_effective_result_type(std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context) noexcept;
bool is_plain_named_non_result_type(std::shared_ptr<registry::Type> type_value) noexcept;
bool should_use_explicit_visit_return_type(std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept;
mlc::String guarded_literal_condition(std::shared_ptr<ast::Pattern> pattern, mlc::String variant_access_expression) noexcept;
mlc::String return_or_discard_line(bool use_void, mlc::String body_code) noexcept;
mlc::String guarded_body_fragment(bool has_guard, mlc::String guard_code, bool use_void, mlc::String body_code) noexcept;
template<typename __F4>
mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept;
template<typename __F5>
mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept;
template<typename __F6>
mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F6 eval_expr_fn) noexcept;
mlc::String generic_variant_type_argument(context::CodegenContext context, mlc::String constructor_name) noexcept;
int type_parameter_name_index(mlc::Array<mlc::String> parameter_names, mlc::String parameter_name) noexcept;
mlc::String cpp_angle_bracket_type_arguments(mlc::Array<mlc::String> type_argument_parts) noexcept;
mlc::String instantiated_variant_type_argument_from_maps(context::CodegenContext context, mlc::String owner_name, mlc::String constructor_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;
mlc::String generic_subject_type_name(std::shared_ptr<registry::Type> subject_type) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> generic_subject_type_arguments(std::shared_ptr<registry::Type> subject_type) noexcept;
mlc::String instantiated_variant_type_argument_from_generic_subject(context::CodegenContext context, mlc::String constructor_name, mlc::String sum_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;
mlc::String instantiated_variant_type_argument_from_subject(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept;
mlc::String result_ctor_cpp_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept;
mlc::String generic_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;
mlc::String non_result_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept;
template<typename __F8>
mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F8 eval_expr_fn) noexcept;
template<typename __F7>
mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F7 eval_expr_fn) noexcept;
mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppExpression> string_match_literal_condition_cpp(std::shared_ptr<ast::Pattern> pattern, mlc::String subject_holder) noexcept;
template<typename __F3>
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_cpp_fn) noexcept;
template<typename __F3>
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_string(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppStatement> string_match_arm_else_branch(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int start_index, mlc::String subject_holder, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppStatement>(std::shared_ptr<semantic_ir::SemanticMatchArm>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> build_return_block) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String string_match_statements_to_source(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;
mlc::String gen_string_match_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_match_string_literal_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_match_string_literal_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;
mlc::String full_result_cpp_template(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;
mlc::String expression_result_cpp_type_for_codegen(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;
mlc::String result_ok_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept;
mlc::String result_ok_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept;
mlc::String result_err_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept;
mlc::String result_err_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept;
mlc::String match_ok_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::String direct) noexcept;
mlc::String match_err_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;
bool cpp_type_string_has_auto_placeholder(mlc::String type_string) noexcept;
bool is_bare_result_cpp_name(mlc::String type_string) noexcept;
mlc::String match_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;
mlc::String result_template_from_match_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept;
mlc::String match_expression_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;
mlc::String gen_match_guarded_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_cpp_fn) noexcept;
mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;
template<typename __F6>
std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F6 eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;
mlc::String gen_match_std_visit_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_match_std_visit_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;
mlc::String gen_match(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
template<typename __F3>
mlc::String gen_arm_wild_like_body(std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm_body, context, gen_stmts));
  } else   {
    return expr::match_arm_wild_or_unit_return(eval_expr_fn(arm_body, context, gen_stmts));
  }
}
template<typename __F4>
mlc::String gen_arm_ident_body(mlc::String binding_name, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn, bool visit_void) noexcept{
  if (visit_void)   {
    return expr::match_arm_binding_identifier_void(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts));
  } else   {
    return expr::match_arm_binding_identifier(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts));
  }
}
template<typename __F5>
mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn, bool visit_void) noexcept{
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
template<typename __F5>
mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn, bool visit_void) noexcept{
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
template<typename __F4>
mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept{
  auto body_code = eval_expr_fn(arm->body, context, gen_stmts);
  if (arm->has_guard)   {
    auto guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
    return (expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("\n", 1));
  } else   {
    return return_or_discard_line(use_void, body_code);
  }
}
template<typename __F5>
mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept{
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
template<typename __F6>
mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F6 eval_expr_fn) noexcept{
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
template<typename __F8>
mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F8 eval_expr_fn) noexcept{
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
template<typename __F7>
mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F7 eval_expr_fn) noexcept{
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
template<typename __F3>
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_cpp_fn) noexcept{
  return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_return_cpp_statement(eval_expr_cpp_fn(arm->body, context, gen_stmts))});
}
template<typename __F3>
std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_string(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn) noexcept{
  return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_fragment_cpp_statement(expr::return_line(eval_expr_fn(arm->body, context, gen_stmts)))});
}
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_cpp_fn) noexcept{
  auto inner_body = gen_match_guarded_body(subject, expanded_arms, match_result_type, context, gen_stmts, [=](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext eval_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return print::print_expr(eval_expr_cpp_fn(expression, eval_context, gen_stmts_fn)); });
  if (match_visit_uses_void_lambdas(match_result_type))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock{inner_body});
  } else   {
    auto return_cpp = match_return_cpp_type(context, match_result_type, subject);
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn{return_cpp, inner_body});
  }
}
template<typename __F6>
std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F6 eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
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
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
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

} // namespace match_gen

#endif // MATCH_GEN_HPP

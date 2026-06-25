#include "match_gen.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"
#include "registry.hpp"
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
using namespace type_gen;
using namespace literals;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace print;
using namespace literals;
using namespace ast_tokens;

struct RecordFieldBindAccum {mlc::String field_bindings;context::CodegenContext arm_context;};

bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept;

match_gen::RecordFieldBindAccum record_field_bind_step(match_gen::RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern, mlc::String lower_name) noexcept;

match_gen::RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept;

context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, context::CodegenContext base_context) noexcept;

mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

std::shared_ptr<semantic_ir::SemanticMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept;

mlc::String guarded_literal_condition(std::shared_ptr<ast::Pattern> pattern, mlc::String variant_access_expression) noexcept;

mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> string_match_literal_condition_cpp(std::shared_ptr<ast::Pattern> pattern, mlc::String subject_holder) noexcept;

std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_string(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppStatement> string_match_arm_else_branch(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int start_index, mlc::String subject_holder, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppStatement>(std::shared_ptr<semantic_ir::SemanticMatchArm>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> build_return_block) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String string_match_statements_to_source(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

mlc::String gen_string_match_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_match_string_literal_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_match_string_literal_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

mlc::String gen_match_guarded_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept;

mlc::String gen_match(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnit>((*match_result_type))) {  return true; } return false; }();}

match_gen::RecordFieldBindAccum record_field_bind_step(match_gen::RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern, mlc::String lower_name) noexcept{return [&]() -> match_gen::RecordFieldBindAccum { if (std::holds_alternative<ast::PatternIdent>((*field_pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*field_pattern)); auto [field_name, _w0] = _v_patternident; return match_gen::RecordFieldBindAccum{accumulated.field_bindings + expr::record_pattern_field_binding(cpp_naming::cpp_safe(field_name), lower_name), context::CodegenContext_add_value(accumulated.arm_context, field_name)}; } return accumulated; }();}

match_gen::RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept{return field_patterns.fold(match_gen::RecordFieldBindAccum{mlc::String(""), start_context}, [lower_name](match_gen::RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern) mutable { return record_field_bind_step(accumulated, field_pattern, lower_name); });}

context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, context::CodegenContext base_context) noexcept{
std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_information = context::lookup_ctor_type_info_for_context(base_context, constructor_name);
context::CodegenContext arm_context = std::move(base_context);
int pattern_index = 0;
while (pattern_index < sub_patterns.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatternIdent>((*sub_patterns[pattern_index]))) { auto _v_patternident = std::get<ast::PatternIdent>((*sub_patterns[pattern_index])); auto [pattern_name, _w0] = _v_patternident; return [&]() { 
  if (ctor_type_information->shared_pos.contains(pattern_index)){
{
arm_context = context::CodegenContext_add_shared(arm_context, pattern_name);
}
}
  if (ctor_type_information->shared_arr_pos.contains(pattern_index)){
{
arm_context = context::CodegenContext_add_shared_array(arm_context, pattern_name);
}
}
  if (!ctor_type_information->shared_pos.contains(pattern_index) && !ctor_type_information->shared_arr_pos.contains(pattern_index)){
arm_context = context::CodegenContext_add_value(arm_context, pattern_name);
}
 }(); } return; }();
pattern_index = pattern_index + 1;
}
}
return arm_context;
}

mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
bool is_generic = decl_index::list_contains(context.generic_variants, constructor_name);
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
mlc::String body_code = eval_expr_fn(arm_body, arm_context, gen_stmts);
return is_generic ? visit_void ? expr::match_arm_constructed_value_generic_void(qualified_name, lower_name, binding, body_code) : expr::match_arm_constructed_value_generic(qualified_name, lower_name, binding, body_code) : [&]() -> mlc::String { 
  mlc::String param = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return visit_void ? expr::match_arm_constructed_value_void(param, binding, body_code) : expr::match_arm_constructed_value(param, binding, body_code);
 }();
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
mlc::String body_code = eval_expr_fn(arm_body, accumulated.arm_context, gen_stmts);
return is_generic ? visit_void ? expr::match_arm_constructed_value_generic_void(qualified_name, lower_name, accumulated.field_bindings, body_code) : expr::match_arm_constructed_value_generic(qualified_name, lower_name, accumulated.field_bindings, body_code) : [&]() -> mlc::String { 
  mlc::String param = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return visit_void ? expr::match_arm_constructed_value_void(param, accumulated.field_bindings, body_code) : expr::match_arm_constructed_value(param, accumulated.field_bindings, body_code);
 }();
}

std::shared_ptr<semantic_ir::SemanticMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept{return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> { if (std::holds_alternative<ast::PatternOr>((*arm->pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*arm->pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.map([arm](std::shared_ptr<ast::Pattern> alternative) mutable { return match_arm_with_pattern(arm, alternative); }); } return mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{arm}; }();}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return mlc::collections::flat_map(arms, [](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return singleton_or_split(arm); });}

bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept{return expanded_arms.any([](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return arm->has_guard; });}

mlc::String guarded_literal_condition(std::shared_ptr<ast::Pattern> pattern, mlc::String variant_access_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::PatternBool>((*pattern))) { auto _v_patternbool = std::get<ast::PatternBool>((*pattern)); auto [value, _w0] = _v_patternbool; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_boolean_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternInt>((*pattern))) { auto _v_patternint = std::get<ast::PatternInt>((*pattern)); auto [value, _w0] = _v_patternint; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_integer_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [value, _w0] = _v_patternstr; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_string_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [value, _w0] = _v_patternstringlit; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_string_literal(value) + mlc::String(")"); } return mlc::String("false"); }();}

mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
}

mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String condition = guarded_literal_condition(arm->pattern, variant_access_expression);
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  mlc::String combined_condition = mlc::String("(") + condition + mlc::String(" && (") + guard_code + mlc::String("))");
  return expr::if_brace_block(combined_condition, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::if_brace_block(condition, expr::return_line(body_code)) + mlc::String("\n");
}

mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String binding_cpp_safe = cpp_naming::cpp_safe(binding_name);
context::CodegenContext extended_context = context::CodegenContext_add_value(context, binding_name);
mlc::String binding_statement = mlc::String("auto ") + binding_cpp_safe + mlc::String(" = ") + subject_holder_name + mlc::String(";\n");
mlc::String body_code = eval_expr_fn(arm->body, extended_context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, extended_context, gen_stmts);
  return mlc::String("{\n") + binding_statement + expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("}\n");
 }() : mlc::String("{\n") + binding_statement + expr::return_line(body_code) + mlc::String("}\n");
}

mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding_prefix = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
mlc::String type_argument = decl_index::list_contains(context.generic_variants, constructor_name) ? mlc::String("<auto>") : mlc::String("");
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + binding_prefix;
mlc::String body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
mlc::String inner = arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, arm_context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
mlc::String type_argument = decl_index::list_contains(context.generic_variants, record_name) ? mlc::String("<auto>") : mlc::String("");
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + accumulated.field_bindings;
mlc::String body_code = eval_expr_fn(arm->body, accumulated.arm_context, gen_stmts);
mlc::String inner = arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, accumulated.arm_context, gen_stmts);
  return expr::if_brace_block(guard_code, expr::return_line(body_code)) + mlc::String("\n");
 }() : expr::return_line(body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return std::visit(overloaded{
  [&](const PatternCtor& patternctor) -> mlc::String { auto [name, sub_patterns, _w0] = patternctor; return gen_guarded_constructor_arm_fragment(arm, name, sub_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [name, field_patterns, _w0] = patternrecord; return gen_guarded_record_arm_fragment(arm, name, field_patterns, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return gen_guarded_wild_or_unit_arm_fragment(arm, context, gen_stmts, eval_expr_fn); },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, context, gen_stmts, eval_expr_fn); },
  [&](const PatternIdent& patternident) -> mlc::String { auto [binding_name, _w0] = patternident; return gen_guarded_identifier_binding_arm_fragment(arm, binding_name, subject_holder_name, context, gen_stmts, eval_expr_fn); },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return mlc::String(""); },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return mlc::String(""); },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return mlc::String(""); }
}, (*arm->pattern));}

std::shared_ptr<cpp_ast::CppExpression> string_match_literal_condition_cpp(std::shared_ptr<ast::Pattern> pattern, mlc::String subject_holder) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [value, _w0] = _v_patternstr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("=="), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(subject_holder)), literals::gen_string_literal_cpp(value))); } if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [value, _w0] = _v_patternstringlit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("=="), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(subject_holder)), literals::gen_string_literal_cpp(value))); } return emit_helpers::make_boolean_cpp_expression(false); }();}

std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(eval_expr_cpp_fn(arm->body, context, gen_stmts))});}

std::shared_ptr<cpp_ast::CppStatement> string_match_return_block_string(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_fragment_cpp_statement(expr::return_line(eval_expr_fn(arm->body, context, gen_stmts)))});}

std::shared_ptr<cpp_ast::CppStatement> string_match_arm_else_branch(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int start_index, mlc::String subject_holder, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppStatement>(std::shared_ptr<semantic_ir::SemanticMatchArm>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> build_return_block) noexcept{return start_index >= arms.size() ? emit_helpers::make_expression_cpp_statement(emit_helpers::make_identifier_cpp_expression(mlc::String("std::abort()"))) : [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  std::shared_ptr<semantic_ir::SemanticMatchArm> arm = arms[start_index];
  std::shared_ptr<cpp_ast::CppStatement> return_block = build_return_block(arm, context, gen_stmts);
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<ast::PatternWild>((*arm->pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*arm->pattern)); auto [_w0] = _v_patternwild; return return_block; } if (std::holds_alternative<ast::PatternStr>((*arm->pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*arm->pattern)); auto [_w0, _w1] = _v_patternstr; return emit_helpers::make_if_cpp_statement(string_match_literal_condition_cpp(arm->pattern, subject_holder), return_block, string_match_arm_else_branch(arms, start_index + 1, subject_holder, context, gen_stmts, build_return_block)); } if (std::holds_alternative<ast::PatternStringLit>((*arm->pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*arm->pattern)); auto [_w0, _w1] = _v_patternstringlit; return emit_helpers::make_if_cpp_statement(string_match_literal_condition_cpp(arm->pattern, subject_holder), return_block, string_match_arm_else_branch(arms, start_index + 1, subject_holder, context, gen_stmts, build_return_block)); } return return_block; }();
 }();}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(emit_helpers::make_auto_cpp_statement(mlc::String("__match_subject"), eval_expr_cpp_fn(subject, context, gen_stmts)));
statements.push_back(string_match_arm_else_branch(expanded_arms, 0, mlc::String("__match_subject"), context, gen_stmts, [eval_expr_cpp_fn](std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext arm_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return string_match_return_block_cpp(arm, arm_context, gen_stmts_fn, eval_expr_cpp_fn); }));
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> string_match_statements_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(emit_helpers::make_fragment_cpp_statement(mlc::String("auto __match_subject = ") + eval_expr_fn(subject, context, gen_stmts) + mlc::String(";\n")));
statements.push_back(string_match_arm_else_branch(expanded_arms, 0, mlc::String("__match_subject"), context, gen_stmts, [eval_expr_fn](std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext arm_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return string_match_return_block_string(arm, arm_context, gen_stmts_fn, eval_expr_fn); }));
return statements;
}

mlc::String string_match_statements_to_source(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
mlc::String body_source = mlc::String("");
int statement_index = 0;
while (statement_index < statements.size()){
{
body_source = body_source + print::print_statement(statements[statement_index]) + mlc::String("\n");
statement_index = statement_index + 1;
}
}
return body_source;
}

mlc::String gen_string_match_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return string_match_statements_to_source(string_match_statements_string(subject, expanded_arms, context, gen_stmts, eval_expr_fn));}

mlc::String gen_match_string_literal_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
mlc::String inner_body = gen_string_match_body(subject, expanded_arms, context, gen_stmts, eval_expr_fn);
return mlc::String("[&]() -> ") + return_cpp + mlc::String(" {\n") + inner_body + mlc::String("}()");
}

std::shared_ptr<cpp_ast::CppExpression> gen_match_string_literal_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
mlc::String inner_body = string_match_statements_to_source(string_match_statements_cpp(subject, expanded_arms, context, gen_stmts, eval_expr_cpp_fn));
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn(return_cpp, inner_body));
}

mlc::String gen_match_guarded_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
mlc::String variant_access = expr::visit_subject_for_match(mlc::String("__match_subject"), needs_dereference);
mlc::Array<mlc::String> fragment_lines = expanded_arms.map([variant_access, context, gen_stmts, eval_expr_fn](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_guarded_arm_fragment(arm, variant_access, mlc::String("__match_subject"), context, gen_stmts, eval_expr_fn); });
return mlc::String("auto __match_subject = ") + subject_code + mlc::String(";\n") + fragment_lines.join(mlc::String("")) + mlc::String("std::abort();\n");
}

mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
mlc::String inner_body = gen_match_guarded_body(subject, expanded_arms, context, gen_stmts, eval_expr_fn);
return mlc::String("[&]() -> ") + return_cpp + mlc::String(" {\n") + inner_body + mlc::String("}()");
}

std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, match_result_type);
mlc::String inner_body = gen_match_guarded_body(subject, expanded_arms, context, gen_stmts, [eval_expr_cpp_fn](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext eval_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return print::print_expr(eval_expr_cpp_fn(expression, eval_context, gen_stmts_fn)); });
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn(return_cpp, inner_body));
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternIdent& patternident) -> mlc::String { auto [name, _w0] = patternident; return visit_void ? expr::match_arm_binding_identifier_void(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, context::CodegenContext_add_value(context, name), gen_stmts)) : expr::match_arm_binding_identifier(cpp_naming::cpp_safe(name), eval_expr_fn(arm->body, context::CodegenContext_add_value(context, name), gen_stmts)); },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [name, sub_patterns, _w0] = patternctor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [name, field_patterns, _w0] = patternrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm->body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm->body, context, gen_stmts)); }
}, (*arm->pattern));}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
bool is_generic = decl_index::list_contains(context.generic_variants, constructor_name);
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
std::shared_ptr<cpp_ast::CppExpression> body_expression = eval_expr_cpp_fn(arm_body, arm_context, gen_stmts);
return is_generic ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric(qualified_name, lower_name, binding, visit_void, body_expression)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed(parameter, binding, visit_void, body_expression));
 }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
std::shared_ptr<cpp_ast::CppExpression> body_expression = eval_expr_cpp_fn(arm_body, accumulated.arm_context, gen_stmts);
return is_generic ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric(qualified_name, lower_name, accumulated.field_bindings, visit_void, body_expression)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed(parameter, accumulated.field_bindings, visit_void, body_expression));
 }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0] = patternwild; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0] = patternunit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternbool; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternint; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternstr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternstringlit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, _w0] = patternident; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmBinding(cpp_naming::cpp_safe(name), visit_void, eval_expr_cpp_fn(arm->body, context::CodegenContext_add_value(context, name), gen_stmts))); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, sub_patterns, _w0] = patternctor; return gen_arm_ctor_cpp(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_cpp_fn, visit_void); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, field_patterns, _w0] = patternrecord; return gen_arm_record_pattern_cpp(name, field_patterns, arm->body, context, gen_stmts, eval_expr_cpp_fn, visit_void); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patterntuple; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1, _w2] = patternarray; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternor; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, eval_expr_cpp_fn(arm->body, context, gen_stmts))); }
}, (*arm->pattern));}

mlc::String gen_match(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded = expand_or_arms(arms);
return expanded_any_guard(expanded) ? gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : match_analysis::should_use_string_match_if_chain(subject, expanded) ? gen_match_string_literal_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : [&]() -> mlc::String { 
  bool visit_void = match_visit_uses_void_lambdas(match_result_type);
  mlc::Array<mlc::String> arm_lines = expanded.map([context, gen_stmts, eval_expr_fn, visit_void](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_arm(arm, context, gen_stmts, eval_expr_fn, visit_void); });
  mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
  bool needs_deref = expanded.size() > 0 ? match_analysis::first_arm_needs_deref(expanded[0], subject, context) : true;
  return expr::std_visit_match_expression(arm_lines.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_deref));
 }();
}

} // namespace match_gen

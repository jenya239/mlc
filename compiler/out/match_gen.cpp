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

struct RecordFieldBindAccum {mlc::String field_bindings;context::CodegenContext arm_context;};

bool match_visit_uses_void_lambdas(std::shared_ptr<registry::Type> match_result_type) noexcept;

match_gen::RecordFieldBindAccum record_field_bind_step(match_gen::RecordFieldBindAccum accumulated, std::shared_ptr<ast::Pattern> field_pattern, mlc::String lower_name) noexcept;

match_gen::RecordFieldBindAccum record_pattern_field_bindings_and_context(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String lower_name, context::CodegenContext start_context) noexcept;

context::CodegenContext codegen_context_with_ctor_field_bindings(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, context::CodegenContext base_context) noexcept;

mlc::String gen_arm_wild_like_body(std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

mlc::String gen_arm_ident_body(mlc::String binding_name, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

mlc::String gen_arm_ctor_generic_body(mlc::String qualified_name, mlc::String lower_name, mlc::String binding, mlc::String body_code, bool visit_void) noexcept;

mlc::String gen_arm_ctor_record_body(mlc::String parameter, mlc::String binding, mlc::String body_code, bool visit_void) noexcept;

mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

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

mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

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

mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

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

std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept;

mlc::String gen_match_std_visit_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_match_std_visit_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept;

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
[&]() -> void { if (std::holds_alternative<ast::PatternIdent>((*sub_patterns[pattern_index]))) { auto _v_patternident = std::get<ast::PatternIdent>((*sub_patterns[pattern_index])); auto [pattern_name, _w0] = _v_patternident; if (ctor_type_information->shared_pos.contains(pattern_index)){
{
arm_context = context::CodegenContext_add_shared(arm_context, pattern_name);
}
} if (ctor_type_information->shared_arr_pos.contains(pattern_index)){
{
arm_context = context::CodegenContext_add_shared_array(arm_context, pattern_name);
}
} if (!ctor_type_information->shared_pos.contains(pattern_index) && !ctor_type_information->shared_arr_pos.contains(pattern_index)){
arm_context = context::CodegenContext_add_value(arm_context, pattern_name);
}; return; }  }();
pattern_index = pattern_index + 1;
}
}
return arm_context;
}

mlc::String gen_arm_wild_like_body(std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{return visit_void ? expr::match_arm_wild_or_unit_statement(eval_expr_fn(arm_body, context, gen_stmts)) : expr::match_arm_wild_or_unit_return(eval_expr_fn(arm_body, context, gen_stmts));}

mlc::String gen_arm_ident_body(mlc::String binding_name, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{return visit_void ? expr::match_arm_binding_identifier_void(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts)) : expr::match_arm_binding_identifier(cpp_naming::cpp_safe(binding_name), eval_expr_fn(arm_body, context::CodegenContext_add_value(context, binding_name), gen_stmts));}

mlc::String gen_arm_ctor_generic_body(mlc::String qualified_name, mlc::String lower_name, mlc::String binding, mlc::String body_code, bool visit_void) noexcept{return visit_void ? expr::match_arm_constructed_value_generic_void(qualified_name, lower_name, binding, body_code) : expr::match_arm_constructed_value_generic(qualified_name, lower_name, binding, body_code);}

mlc::String gen_arm_ctor_record_body(mlc::String parameter, mlc::String binding, mlc::String body_code, bool visit_void) noexcept{return visit_void ? expr::match_arm_constructed_value_void(parameter, binding, body_code) : expr::match_arm_constructed_value(parameter, binding, body_code);}

mlc::String gen_arm_ctor(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
bool is_generic = decl_index::list_contains(context.generic_variants, constructor_name);
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
mlc::String body_code = eval_expr_fn(arm_body, arm_context, gen_stmts);
return is_generic ? gen_arm_ctor_generic_body(qualified_name, lower_name, binding, body_code, visit_void) : [&]() -> mlc::String { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return gen_arm_ctor_record_body(parameter, binding, body_code, visit_void);
 }();
}

mlc::String gen_arm_record_pattern(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
mlc::String body_code = eval_expr_fn(arm_body, accumulated.arm_context, gen_stmts);
return is_generic ? gen_arm_ctor_generic_body(qualified_name, lower_name, accumulated.field_bindings, body_code, visit_void) : [&]() -> mlc::String { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return gen_arm_ctor_record_body(parameter, accumulated.field_bindings, body_code, visit_void);
 }();
}

std::shared_ptr<semantic_ir::SemanticMatchArm> match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept{return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> singleton_or_split(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> { if (std::holds_alternative<ast::PatternOr>((*arm->pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*arm->pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.map([arm](std::shared_ptr<ast::Pattern> alternative) mutable { return match_arm_with_pattern(arm, alternative); }); } return mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{arm}; }();}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return mlc::collections::flat_map(arms, [](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return singleton_or_split(arm); });}

bool expanded_any_guard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept{return expanded_arms.any([](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return arm->has_guard; });}

bool pattern_is_wildcard(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return true; } return false; }();}

bool expanded_any_wildcard(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms) noexcept{return expanded_arms.any([](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_is_wildcard(arm->pattern); });}

std::shared_ptr<registry::Type> match_effective_result_type(std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context) noexcept{return result_option_method_types::is_result_generic(match_result_type) ? match_result_type : result_option_method_types::is_result_generic(context.enclosing_function_return_type) ? context.enclosing_function_return_type : match_result_type;}

bool is_plain_named_non_result_type(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return name != mlc::String("Result"); } return false; }();}

bool should_use_explicit_visit_return_type(std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{
if (result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject))){
{
return true;
}
}
std::shared_ptr<registry::Type> effective_result_type = match_effective_result_type(match_result_type, context);
mlc::String return_cpp = match_return_cpp_type(context, effective_result_type, subject);
if (return_cpp.length() == 0){
{
return false;
}
}
if (is_plain_named_non_result_type(effective_result_type)){
{
return true;
}
}
mlc::String ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(effective_result_type));
mlc::String subject_cpp = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(subject));
return ok_cpp == mlc::String("auto") || subject_cpp == mlc::String("auto") ? return_cpp.contains(mlc::String("Result")) : ok_cpp != subject_cpp;
}

mlc::String guarded_literal_condition(std::shared_ptr<ast::Pattern> pattern, mlc::String variant_access_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::PatternBool>((*pattern))) { auto _v_patternbool = std::get<ast::PatternBool>((*pattern)); auto [value, _w0] = _v_patternbool; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_boolean_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternInt>((*pattern))) { auto _v_patternint = std::get<ast::PatternInt>((*pattern)); auto [value, _w0] = _v_patternint; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_integer_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [value, _w0] = _v_patternstr; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_string_literal(value) + mlc::String(")"); } if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [value, _w0] = _v_patternstringlit; return mlc::String("(") + variant_access_expression + mlc::String(" == ") + literals::gen_string_literal(value) + mlc::String(")"); } return mlc::String("false"); }();}

mlc::String return_or_discard_line(bool use_void, mlc::String body_code) noexcept{return use_void ? body_code + mlc::String(";\nreturn;\n") : expr::return_line(body_code);}

mlc::String guarded_body_fragment(bool has_guard, mlc::String guard_code, bool use_void, mlc::String body_code) noexcept{return has_guard ? expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("\n") : return_or_discard_line(use_void, body_code);}

mlc::String gen_guarded_wild_or_unit_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  return expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("\n");
 }() : return_or_discard_line(use_void, body_code);
}

mlc::String gen_guarded_literal_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String condition = guarded_literal_condition(arm->pattern, variant_access_expression);
mlc::String body_code = eval_expr_fn(arm->body, context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, context, gen_stmts);
  mlc::String combined_condition = mlc::String("(") + condition + mlc::String(" && (") + guard_code + mlc::String("))");
  return expr::if_brace_block(combined_condition, return_or_discard_line(use_void, body_code)) + mlc::String("\n");
 }() : expr::if_brace_block(condition, return_or_discard_line(use_void, body_code)) + mlc::String("\n");
}

mlc::String gen_guarded_identifier_binding_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String binding_name, mlc::String subject_holder_name, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String binding_cpp_safe = cpp_naming::cpp_safe(binding_name);
context::CodegenContext extended_context = context::CodegenContext_add_value(context, binding_name);
mlc::String binding_statement = mlc::String("auto ") + binding_cpp_safe + mlc::String(" = ") + subject_holder_name + mlc::String(";\n");
mlc::String body_code = eval_expr_fn(arm->body, extended_context, gen_stmts);
return arm->has_guard ? [&]() -> mlc::String { 
  mlc::String guard_code = eval_expr_fn(arm->when_condition, extended_context, gen_stmts);
  return mlc::String("{\n") + binding_statement + expr::if_brace_block(guard_code, return_or_discard_line(use_void, body_code)) + mlc::String("}\n");
 }() : mlc::String("{\n") + binding_statement + return_or_discard_line(use_void, body_code) + mlc::String("}\n");
}

mlc::String generic_variant_type_argument(context::CodegenContext context, mlc::String constructor_name) noexcept{return decl_index::list_contains(context.generic_variants, constructor_name) ? mlc::String("<auto>") : mlc::String("");}

int type_parameter_name_index(mlc::Array<mlc::String> parameter_names, mlc::String parameter_name) noexcept{
int index = 0;
while (index < parameter_names.size()){
{
if (parameter_names[index] == parameter_name){
{
return index;
}
}
index = index + 1;
}
}
return -1;
}

mlc::String cpp_angle_bracket_type_arguments(mlc::Array<mlc::String> type_argument_parts) noexcept{return type_argument_parts.size() == 0 ? mlc::String("") : mlc::String("<") + type_argument_parts.join(mlc::String(", ")) + mlc::String(">");}

mlc::String instantiated_variant_type_argument_from_maps(context::CodegenContext context, mlc::String owner_name, mlc::String constructor_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{
mlc::Array<mlc::String> all_parameter_names = context.sum_type_parameter_names.get(owner_name);
mlc::Array<mlc::String> used_parameter_names = context.variant_used_type_parameter_names.get(constructor_name);
mlc::Array<mlc::String> cpp_argument_parts = {};
int used_index = 0;
while (used_index < used_parameter_names.size()){
{
int parameter_index = type_parameter_name_index(all_parameter_names, used_parameter_names[used_index]);
if (parameter_index < 0 || parameter_index >= type_arguments.size()){
{
return mlc::String("");
}
}
if (semantic_type_structure::type_is_unknown(type_arguments[parameter_index])){
{
return mlc::String("");
}
}
cpp_argument_parts.push_back(type_gen::sem_type_to_cpp(context, type_arguments[parameter_index]));
used_index = used_index + 1;
}
}
return cpp_angle_bracket_type_arguments(cpp_argument_parts);
}

mlc::String generic_subject_type_name(std::shared_ptr<registry::Type> subject_type) noexcept{return semantic_type_structure::generic_type_name_from_type(subject_type);}

mlc::Array<std::shared_ptr<registry::Type>> generic_subject_type_arguments(std::shared_ptr<registry::Type> subject_type) noexcept{return semantic_type_structure::generic_type_arguments_from_type(subject_type);}

mlc::String instantiated_variant_type_argument_from_generic_subject(context::CodegenContext context, mlc::String constructor_name, mlc::String sum_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{return context.variant_types.has(constructor_name) && context.variant_types.get(constructor_name) != sum_name ? mlc::String("") : context.variant_used_type_parameter_names.has(constructor_name) && context.variant_used_type_parameter_names.get(constructor_name).size() == 0 ? mlc::String("") : context.variant_used_type_parameter_names.has(constructor_name) && context.sum_type_parameter_names.has(sum_name) ? instantiated_variant_type_argument_from_maps(context, sum_name, constructor_name, type_arguments) : type_arguments.size() == 1 && !semantic_type_structure::type_is_unknown(type_arguments[0]) && !(context.variant_used_type_parameter_names.has(constructor_name) && context.variant_used_type_parameter_names.get(constructor_name).size() == 0) ? cpp_angle_bracket_type_arguments(mlc::Array<mlc::String>{type_gen::sem_type_to_cpp(context, type_arguments[0])}) : mlc::String("");}

mlc::String instantiated_variant_type_argument_from_subject(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{
mlc::String sum_name = generic_subject_type_name(subject_type);
return sum_name == mlc::String("") ? mlc::String("") : instantiated_variant_type_argument_from_generic_subject(context, constructor_name, sum_name, generic_subject_type_arguments(subject_type));
}

mlc::String result_ctor_cpp_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{return constructor_name == mlc::String("Ok") ? mlc::String("<") + type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(subject_type)) + mlc::String(">") : constructor_name == mlc::String("Err") ? mlc::String("<") + type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(subject_type)) + mlc::String(">") : mlc::String("<auto>");}

mlc::String generic_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{return result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)) ? result_ctor_cpp_type_argument(context, constructor_name, semantic_ir::sexpr_type(subject)) : non_result_ctor_type_argument(context, constructor_name, semantic_ir::sexpr_type(subject));}

mlc::String non_result_ctor_type_argument(context::CodegenContext context, mlc::String constructor_name, std::shared_ptr<registry::Type> subject_type) noexcept{
mlc::String instantiated = instantiated_variant_type_argument_from_subject(context, constructor_name, subject_type);
return instantiated.length() > 0 ? instantiated : generic_variant_type_argument(context, constructor_name);
}

mlc::String gen_guarded_constructor_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String variant_access_expression, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding_prefix = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
mlc::String type_argument = generic_ctor_type_argument(context, constructor_name, subject);
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + binding_prefix;
mlc::String body_code = eval_expr_fn(arm->body, arm_context, gen_stmts);
mlc::String guard_code = arm->has_guard ? eval_expr_fn(arm->when_condition, arm_context, gen_stmts) : mlc::String("");
mlc::String inner = guarded_body_fragment(arm->has_guard, guard_code, use_void, body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_record_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String variant_access_expression, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
mlc::String type_argument = decl_index::list_contains(context.generic_variants, record_name) ? mlc::String("<auto>") : mlc::String("");
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
mlc::String holds_check = mlc::String("std::holds_alternative<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(")");
mlc::String open_block = mlc::String("if (") + holds_check + mlc::String(") {\n") + mlc::String("const ") + qualified_name + type_argument + mlc::String("& ") + lower_name + mlc::String(" = std::get<") + qualified_name + type_argument + mlc::String(">(") + variant_access_expression + mlc::String(");\n") + accumulated.field_bindings;
mlc::String body_code = eval_expr_fn(arm->body, accumulated.arm_context, gen_stmts);
mlc::String guard_code = arm->has_guard ? eval_expr_fn(arm->when_condition, accumulated.arm_context, gen_stmts) : mlc::String("");
mlc::String inner = guarded_body_fragment(arm->has_guard, guard_code, use_void, body_code);
return open_block + inner + mlc::String("}\n");
}

mlc::String gen_guarded_arm_fragment(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mlc::String variant_access_expression, mlc::String subject_holder_name, std::shared_ptr<semantic_ir::SemanticExpression> subject, bool use_void, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return std::visit(overloaded{
  [&](const PatternCtor& patternctor) -> mlc::String { auto [name, sub_patterns, _w0] = patternctor; return gen_guarded_constructor_arm_fragment(arm, name, sub_patterns, variant_access_expression, subject, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [name, field_patterns, _w0] = patternrecord; return gen_guarded_record_arm_fragment(arm, name, field_patterns, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return gen_guarded_wild_or_unit_arm_fragment(arm, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return gen_guarded_wild_or_unit_arm_fragment(arm, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return gen_guarded_literal_arm_fragment(arm, variant_access_expression, use_void, context, gen_stmts, eval_expr_fn); },
  [&](const PatternIdent& patternident) -> mlc::String { auto [binding_name, _w0] = patternident; return gen_guarded_identifier_binding_arm_fragment(arm, binding_name, subject_holder_name, use_void, context, gen_stmts, eval_expr_fn); },
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
mlc::String return_cpp = match_return_cpp_type(context, match_result_type, subject);
mlc::String inner_body = string_match_statements_to_source(string_match_statements_cpp(subject, expanded_arms, context, gen_stmts, eval_expr_cpp_fn));
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn(return_cpp, inner_body));
}

mlc::String full_result_cpp_template(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
if (!result_option_method_types::is_result_generic(semantic_type)){
{
return mlc::String("");
}
}
mlc::String ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(semantic_type));
mlc::String err_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(semantic_type));
return ok_cpp != mlc::String("auto") && err_cpp != mlc::String("auto") ? expr::cpp_template_two_type_arguments(mlc::String("ast::Result"), ok_cpp, err_cpp) : mlc::String("");
}

mlc::String expression_result_cpp_type_for_codegen(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
mlc::String direct = type_gen::sem_type_to_cpp(context, semantic_type);
if (direct.contains(mlc::String("<"))){
{
return direct;
}
}
if (direct != mlc::String("ast::Result")){
{
return direct;
}
}
mlc::String from_enclosing = full_result_cpp_template(context, context.enclosing_function_return_type);
if (from_enclosing != mlc::String("")){
{
return from_enclosing;
}
}
mlc::String from_type = full_result_cpp_template(context, semantic_type);
if (from_type != mlc::String("")){
{
return from_type;
}
}
return direct;
}

mlc::String result_ok_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
mlc::String from_match = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(match_result_type));
return from_match != mlc::String("auto") ? from_match : mlc::String("");
}

mlc::String result_ok_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{return !result_option_method_types::is_result_generic(match_result_type) ? mlc::String("") : result_ok_type_cpp_from_generic(context, match_result_type);}

mlc::String result_err_type_cpp_from_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
mlc::String from_match = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(match_result_type));
return from_match != mlc::String("auto") ? from_match : mlc::String("");
}

mlc::String result_err_type_cpp_if_generic(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{return !result_option_method_types::is_result_generic(match_result_type) ? mlc::String("") : result_err_type_cpp_from_generic(context, match_result_type);}

mlc::String match_ok_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::String direct) noexcept{
mlc::String from_result = result_ok_type_cpp_if_generic(context, match_result_type);
return from_result != mlc::String("") ? from_result : direct != mlc::String("ast::Result") && direct != mlc::String("void") && direct != mlc::String("auto") && direct.length() > 0 ? direct : result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)) ? type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(semantic_ir::sexpr_type(subject))) : mlc::String("auto");
}

mlc::String match_err_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
mlc::String from_result = result_err_type_cpp_if_generic(context, match_result_type);
return from_result != mlc::String("") ? from_result : result_option_method_types::is_result_generic(semantic_ir::sexpr_type(subject)) ? type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(semantic_ir::sexpr_type(subject))) : mlc::String("auto");
}

bool cpp_type_string_has_auto_placeholder(mlc::String type_string) noexcept{return type_string == mlc::String("auto") || type_string.contains(mlc::String("<auto>")) || type_string.contains(mlc::String("<auto,")) || type_string.contains(mlc::String(", auto>")) || type_string.contains(mlc::String(", auto,"));}

bool is_bare_result_cpp_name(mlc::String type_string) noexcept{return type_string == mlc::String("ast::Result") || type_string == mlc::String("Result");}

mlc::String match_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
mlc::String direct = type_gen::sem_type_to_cpp(context, match_result_type);
bool direct_is_incomplete = cpp_type_string_has_auto_placeholder(direct);
mlc::String enclosing_cpp = type_gen::sem_type_to_cpp(context, context.enclosing_function_return_type);
bool enclosing_usable = enclosing_cpp != mlc::String("auto") && enclosing_cpp != mlc::String("void") && enclosing_cpp.length() > 0 && !cpp_type_string_has_auto_placeholder(enclosing_cpp) && !result_option_method_types::is_result_generic(context.enclosing_function_return_type);
return direct.contains(mlc::String("<")) && !direct_is_incomplete ? direct : is_bare_result_cpp_name(direct) ? [&]() -> mlc::String { 
  mlc::String from_match = full_result_cpp_template(context, match_result_type);
  return from_match != mlc::String("") ? from_match : [&]() -> mlc::String { 
  mlc::String from_enclosing = full_result_cpp_template(context, context.enclosing_function_return_type);
  return from_enclosing != mlc::String("") ? from_enclosing : direct;
 }();
 }() : direct != mlc::String("auto") && direct != mlc::String("") && !direct_is_incomplete && !result_option_method_types::is_result_generic(match_result_type) ? direct : enclosing_usable && (direct == mlc::String("auto") || direct_is_incomplete || direct == mlc::String("ast::Result") || result_option_method_types::is_result_generic(match_result_type)) ? enclosing_cpp : [&]() -> mlc::String { 
  mlc::String ok_cpp = match_ok_cpp_type(context, match_result_type, subject, direct);
  mlc::String err_cpp = match_err_cpp_type(context, match_result_type, subject);
  return ok_cpp != mlc::String("auto") && err_cpp != mlc::String("auto") ? expr::cpp_template_two_type_arguments(mlc::String("ast::Result"), ok_cpp, err_cpp) : direct_is_incomplete ? mlc::String("auto") : direct;
 }();
}

mlc::String result_template_from_match_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type) noexcept{
return !result_option_method_types::is_result_generic(match_result_type) ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String ok_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(match_result_type));
  mlc::String err_cpp = type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(match_result_type));
  return ok_cpp != mlc::String("auto") && err_cpp != mlc::String("auto") ? expr::cpp_template_two_type_arguments(mlc::String("ast::Result"), ok_cpp, err_cpp) : mlc::String("");
 }();
}

mlc::String match_expression_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_result_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
mlc::String enclosing_cpp = type_gen::sem_type_to_cpp(context, context.enclosing_function_return_type);
return enclosing_cpp != mlc::String("auto") && enclosing_cpp != mlc::String("void") && enclosing_cpp.length() > 0 && !result_option_method_types::is_result_generic(context.enclosing_function_return_type) ? enclosing_cpp : [&]() -> mlc::String { 
  mlc::String direct = type_gen::sem_type_to_cpp(context, match_result_type);
  return direct.contains(mlc::String("<")) ? direct : direct == mlc::String("ast::Result") && full_result_cpp_template(context, context.enclosing_function_return_type) != mlc::String("") ? full_result_cpp_template(context, context.enclosing_function_return_type) : enclosing_cpp != mlc::String("auto") && enclosing_cpp != mlc::String("void") && enclosing_cpp.length() > 0 && !result_option_method_types::is_result_generic(context.enclosing_function_return_type) && (direct == mlc::String("ast::Result") || result_option_method_types::is_result_generic(match_result_type)) ? enclosing_cpp : result_template_from_match_type(context, match_result_type) != mlc::String("") ? result_template_from_match_type(context, match_result_type) : [&]() -> mlc::String { 
  mlc::String err_cpp = match_err_cpp_type(context, match_result_type, subject);
  return direct != mlc::String("ast::Result") && direct != mlc::String("void") && direct != mlc::String("auto") && direct.length() > 0 && err_cpp != mlc::String("auto") ? expr::cpp_template_two_type_arguments(mlc::String("ast::Result"), direct, err_cpp) : direct;
 }();
 }();
}

mlc::String gen_match_guarded_body(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
mlc::String variant_access = expr::visit_subject_for_match(mlc::String("__match_subject"), needs_dereference);
bool use_void = match_visit_uses_void_lambdas(match_result_type);
mlc::Array<mlc::String> fragment_lines = expanded_arms.map([variant_access, subject, use_void, context, gen_stmts, eval_expr_fn](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_guarded_arm_fragment(arm, variant_access, mlc::String("__match_subject"), subject, use_void, context, gen_stmts, eval_expr_fn); });
return mlc::String("auto __match_subject = ") + subject_code + mlc::String(";\n") + fragment_lines.join(mlc::String("")) + mlc::String("std::abort();\n");
}

mlc::String gen_match_guarded_expression(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String inner_body = gen_match_guarded_body(subject, expanded_arms, match_result_type, context, gen_stmts, eval_expr_fn);
return match_visit_uses_void_lambdas(match_result_type) ? mlc::String("[&]() {\n") + inner_body + mlc::String("}()") : [&]() -> mlc::String { 
  mlc::String return_cpp = match_return_cpp_type(context, match_result_type, subject);
  return mlc::String("[&]() -> ") + return_cpp + mlc::String(" {\n") + inner_body + mlc::String("}()");
 }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_match_guarded_expression_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
mlc::String inner_body = gen_match_guarded_body(subject, expanded_arms, match_result_type, context, gen_stmts, [eval_expr_cpp_fn](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext eval_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts_fn) mutable { return print::print_expr(eval_expr_cpp_fn(expression, eval_context, gen_stmts_fn)); });
return match_visit_uses_void_lambdas(match_result_type) ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock(inner_body)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String return_cpp = match_return_cpp_type(context, match_result_type, subject);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn(return_cpp, inner_body));
 }();
}

mlc::String gen_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternIdent& patternident) -> mlc::String { auto [name, _w0] = patternident; return gen_arm_ident_body(name, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [name, sub_patterns, _w0] = patternctor; return gen_arm_ctor(name, sub_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [name, field_patterns, _w0] = patternrecord; return gen_arm_record_pattern(name, field_patterns, arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return gen_arm_wild_like_body(arm->body, context, gen_stmts, eval_expr_fn, visit_void); }
}, (*arm->pattern));}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_ctor_cpp(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, constructor_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(constructor_name));
mlc::String binding = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), lower_name);
mlc::String type_argument = generic_ctor_type_argument(context, constructor_name, subject);
mlc::Array<mlc::String> variant_used_type_parameter_names = context.variant_used_type_parameter_names.has(constructor_name) ? context.variant_used_type_parameter_names.get(constructor_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_names = {};
  return empty_names;
 }();
mlc::String effective_type_argument = type_argument == mlc::String("<auto>") && variant_used_type_parameter_names.size() == 0 ? mlc::String("") : type_argument.length() > 0 && variant_used_type_parameter_names.size() == 0 && sub_patterns.size() == 0 ? mlc::String("") : type_argument;
bool use_generic_lambda = decl_index::list_contains(context.generic_variants, constructor_name) && variant_used_type_parameter_names.size() > 0 && (effective_type_argument == mlc::String("<auto>") || effective_type_argument == mlc::String(""));
context::CodegenContext arm_context = codegen_context_with_ctor_field_bindings(constructor_name, sub_patterns, context);
std::shared_ptr<cpp_ast::CppExpression> body_expression = eval_expr_cpp_fn(arm_body, arm_context, gen_stmts);
return use_generic_lambda ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric(qualified_name, lower_name, binding, visit_void, return_cpp, body_expression)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, effective_type_argument, lower_name);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed(parameter, binding, visit_void, return_cpp, body_expression));
 }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_record_pattern_cpp(mlc::String record_name, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, std::shared_ptr<semantic_ir::SemanticExpression> arm_body, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(context, record_name);
mlc::String lower_name = cpp_naming::cpp_safe(cpp_naming::lower_first(record_name));
bool is_generic = decl_index::list_contains(context.generic_variants, record_name);
match_gen::RecordFieldBindAccum accumulated = record_pattern_field_bindings_and_context(field_patterns, lower_name, context);
std::shared_ptr<cpp_ast::CppExpression> body_expression = eval_expr_cpp_fn(arm_body, accumulated.arm_context, gen_stmts);
return is_generic ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructedGeneric(qualified_name, lower_name, accumulated.field_bindings, visit_void, return_cpp, body_expression)) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String parameter = expr::match_lambda_const_reference_parameter(qualified_name, mlc::String(""), lower_name);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmConstructed(parameter, accumulated.field_bindings, visit_void, return_cpp, body_expression));
 }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_arm_cpp(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn, bool visit_void, mlc::String return_cpp) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0] = patternwild; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0] = patternunit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternbool; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternint; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternstr; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternstringlit; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, _w0] = patternident; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmBinding(cpp_naming::cpp_safe(name), visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context::CodegenContext_add_value(context, name), gen_stmts))); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, sub_patterns, _w0] = patternctor; return gen_arm_ctor_cpp(name, sub_patterns, arm->body, subject, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, field_patterns, _w0] = patternrecord; return gen_arm_record_pattern_cpp(name, field_patterns, arm->body, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patterntuple; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1, _w2] = patternarray; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppExpression> { auto [_w0, _w1] = patternor; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppVisitArmWild(visit_void, return_cpp, eval_expr_cpp_fn(arm->body, context, gen_stmts))); }
}, (*arm->pattern));}

mlc::String gen_match_std_visit_string(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn, bool visit_void) noexcept{
mlc::Array<mlc::String> arm_lines = expanded_arms.map([context, gen_stmts, eval_expr_fn, visit_void](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_arm(arm, context, gen_stmts, eval_expr_fn, visit_void); });
mlc::String subject_code = eval_expr_fn(subject, context, gen_stmts);
bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
return expr::std_visit_match_expression(arm_lines.join(mlc::String(",\n")), expr::visit_subject_for_match(subject_code, needs_dereference));
}

std::shared_ptr<cpp_ast::CppExpression> gen_match_std_visit_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_cpp_fn) noexcept{
bool visit_void = match_visit_uses_void_lambdas(match_result_type);
std::shared_ptr<registry::Type> effective_result_type = match_effective_result_type(match_result_type, context);
mlc::String return_cpp = visit_void || !should_use_explicit_visit_return_type(match_result_type, subject, context) ? mlc::String("") : match_return_cpp_type(context, effective_result_type, subject);
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arm_expressions = expanded_arms.map([subject, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return gen_arm_cpp(arm, subject, context, gen_stmts, eval_expr_cpp_fn, visit_void, return_cpp); });
bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
mlc::String subject_code = print::print_expr(eval_expr_cpp_fn(subject, context, gen_stmts));
mlc::String visit_subject = expr::visit_subject_for_match(subject_code, needs_dereference);
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(expr::std_visit_match_expression(arm_expressions.map([](std::shared_ptr<cpp_ast::CppExpression> arm) mutable { return print::print_expr(arm); }).join(mlc::String(",\n")), visit_subject)));
}

mlc::String gen_match(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_result_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded = expand_or_arms(arms);
bool use_void_visit = match_visit_uses_void_lambdas(match_result_type);
return expanded_any_guard(expanded) ? gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : match_analysis::should_use_string_match_if_chain(subject, expanded) ? gen_match_string_literal_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : expanded_any_wildcard(expanded) ? gen_match_guarded_expression(subject, expanded, match_result_type, context, gen_stmts, eval_expr_fn) : use_void_visit ? gen_match_std_visit_string(subject, expanded, context, gen_stmts, eval_expr_fn, true) : gen_match_std_visit_string(subject, expanded, context, gen_stmts, eval_expr_fn, false);
}

} // namespace match_gen

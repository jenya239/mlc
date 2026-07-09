#include "infer_match.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "pattern_env.hpp"
#include "substitution.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"

namespace infer_match {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace type_diagnostics;
using namespace pattern_env;
using namespace substitution;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct String_match_duplicate_literal_fold_state {mlc::HashMap<mlc::String, bool> seen_literals;mlc::Array<ast::Diagnostic> errors;};

struct String_match_wildcard_fold_state {mlc::Array<ast::Diagnostic> errors;int wildcard_count;int arm_index;int last_arm_index;};

bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool pattern_is_wildcard_only(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> pattern_string_literals_flat(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> literals_so_far) noexcept;

bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

bool should_apply_string_match_rules(std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

mlc::Array<ast::Diagnostic> string_match_subject_type_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type) noexcept;

mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) noexcept;

mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

infer_match::String_match_duplicate_literal_fold_state string_match_duplicate_literal_fold_step(infer_match::String_match_duplicate_literal_fold_state state, mlc::String literal_value, std::shared_ptr<ast::Pattern> pattern_for_diagnostic) noexcept;

mlc::Array<ast::Diagnostic> string_match_duplicate_literal_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

infer_match::String_match_wildcard_fold_state string_match_wildcard_fold_step(infer_match::String_match_wildcard_fold_state state, std::shared_ptr<ast::MatchArm> match_arm) noexcept;

mlc::Array<ast::Diagnostic> string_match_wildcard_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

mlc::Array<ast::Diagnostic> string_match_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::String subject_algebraic_type_name(std::shared_ptr<registry::Type> subject_type) noexcept;

bool pattern_is_catch_all(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> append_covered_constructors(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> covered_so_far) noexcept;

mlc::String join_variant_names(mlc::Array<mlc::String> variant_names) noexcept;

bool match_arm_covers_for_exhaustiveness(std::shared_ptr<ast::MatchArm> match_arm) noexcept;

mlc::Array<ast::Diagnostic> match_exhaustiveness_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [_w0, _w1] = _v_patternstringlit; return true; } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [_w0, _w1] = _v_patternstr; return true; } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.any([](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_contains_string_literal(alternative); }); } return false; }();}

bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [_w0, _w1] = _v_patternstringlit; return true; } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [_w0, _w1] = _v_patternstr; return true; } if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return true; } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.all([](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_is_string_match_pattern(alternative); }); } return false; }();}

bool pattern_is_wildcard_only(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return true; } return false; }();}

mlc::Array<mlc::String> pattern_string_literals_flat(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> literals_so_far) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [value, _w0] = _v_patternstringlit; return literals_so_far.concat(mlc::Array<mlc::String>{value}); } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [value, _w0] = _v_patternstr; return literals_so_far.concat(mlc::Array<mlc::String>{value}); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.fold(literals_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> alternative_pattern) mutable { return pattern_string_literals_flat(alternative_pattern, accumulator); }); } return literals_so_far; }();}

bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{return match_arms.any([](std::shared_ptr<ast::MatchArm> match_arm) mutable { return pattern_contains_string_literal(match_arm->pattern); });}

bool should_apply_string_match_rules(std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{return match_arms_use_string_literals(match_arms) || semantic_type_structure::type_is_string(subject_type);}

mlc::Array<ast::Diagnostic> string_match_subject_type_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type) noexcept{return semantic_type_structure::type_is_unknown(subject_type) || semantic_type_structure::type_is_string(subject_type) ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_list = {};
  return empty_list;
 }() : mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string literal match requires subject type string"), ast::expr_span(subject), diagnostic_codes::diagnostic_code_e077())};}

mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) noexcept{return pattern_is_string_match_pattern(match_arm->pattern) ? diagnostics_so_far : diagnostics_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string match arm must use string literal or wildcard pattern"), ast::pattern_span(match_arm->pattern), diagnostic_codes::diagnostic_code_e079())});}

mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{return match_arms.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [](mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) mutable { return string_match_invalid_arm_pattern_fold_step(diagnostics_so_far, match_arm); });}

infer_match::String_match_duplicate_literal_fold_state string_match_duplicate_literal_fold_step(infer_match::String_match_duplicate_literal_fold_state state, mlc::String literal_value, std::shared_ptr<ast::Pattern> pattern_for_diagnostic) noexcept{return state.seen_literals.has(literal_value) ? infer_match::String_match_duplicate_literal_fold_state{state.seen_literals, state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("duplicate string literal in match"), ast::pattern_span(pattern_for_diagnostic), diagnostic_codes::diagnostic_code_e078())})} : [&]() -> infer_match::String_match_duplicate_literal_fold_state { 
  mlc::HashMap<mlc::String, bool> seen_literals = state.seen_literals;
  seen_literals.set(literal_value, true);
  return infer_match::String_match_duplicate_literal_fold_state{seen_literals, state.errors};
 }();}

mlc::Array<ast::Diagnostic> string_match_duplicate_literal_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
mlc::Array<mlc::String> empty_literals = {};
return match_arms.fold(infer_match::String_match_duplicate_literal_fold_state{mlc::HashMap<mlc::String, bool>(), [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }()}, [empty_literals](infer_match::String_match_duplicate_literal_fold_state fold_state, std::shared_ptr<ast::MatchArm> match_arm) mutable { return pattern_string_literals_flat(match_arm->pattern, empty_literals).fold(fold_state, [match_arm](infer_match::String_match_duplicate_literal_fold_state state_after_literals, mlc::String literal_value) mutable { return string_match_duplicate_literal_fold_step(state_after_literals, literal_value, match_arm->pattern); }); }).errors;
}

infer_match::String_match_wildcard_fold_state string_match_wildcard_fold_step(infer_match::String_match_wildcard_fold_state state, std::shared_ptr<ast::MatchArm> match_arm) noexcept{
bool is_wildcard_arm = pattern_is_wildcard_only(match_arm->pattern);
int wildcard_count_after_arm = is_wildcard_arm ? state.wildcard_count + 1 : state.wildcard_count;
mlc::Array<ast::Diagnostic> errors_after_arm = is_wildcard_arm && state.arm_index != state.last_arm_index ? state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("wildcard match arm must be last for string match"), ast::pattern_span(match_arm->pattern), diagnostic_codes::diagnostic_code_e080())}) : state.errors;
return infer_match::String_match_wildcard_fold_state{errors_after_arm, wildcard_count_after_arm, state.arm_index + 1, state.last_arm_index};
}

mlc::Array<ast::Diagnostic> string_match_wildcard_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
int last_arm_index = -1;
int arm_count_index = 0;
while (arm_count_index < match_arms.size()){
{
last_arm_index = arm_count_index;
arm_count_index = arm_count_index + 1;
}
}
infer_match::String_match_wildcard_fold_state fold_result = match_arms.fold(infer_match::String_match_wildcard_fold_state{[&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), 0, 0, last_arm_index}, [](infer_match::String_match_wildcard_fold_state fold_state, std::shared_ptr<ast::MatchArm> match_arm) mutable { return string_match_wildcard_fold_step(fold_state, match_arm); });
return fold_result.wildcard_count > 1 ? fold_result.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string match allows at most one wildcard arm"), ast::pattern_span(match_arms[0]->pattern), diagnostic_codes::diagnostic_code_e080())}) : fold_result.errors;
}

mlc::Array<ast::Diagnostic> string_match_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{return ast::diagnostics_append(ast::diagnostics_append([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), string_match_invalid_arm_pattern_diagnostic(match_arms)), ast::diagnostics_append(string_match_duplicate_literal_diagnostic(match_arms), string_match_wildcard_diagnostic(match_arms)));}

mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [_w0, span] = _v_patterntuple; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard is not supported with tuple patterns"), span, diagnostic_codes::diagnostic_code_e032())}; } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [_w0, _w1, span] = _v_patternarray; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard is not supported with array patterns"), span, diagnostic_codes::diagnostic_code_e032())}; } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_list = {};
  return empty_list;
 }(); }();}

mlc::String subject_algebraic_type_name(std::shared_ptr<registry::Type> subject_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*subject_type))) { auto _v_tnamed = std::get<registry::TNamed>((*subject_type)); auto [type_name] = _v_tnamed; return type_name; } if (std::holds_alternative<registry::TGeneric>((*subject_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*subject_type)); auto [type_name, _w0] = _v_tgeneric; return type_name; } return mlc::String(""); }();}

bool pattern_is_catch_all(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return true; } if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [_w0, _w1] = _v_patternident; return true; } return false; }();}

mlc::Array<mlc::String> append_covered_constructors(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> covered_so_far) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [constructor_name, _w0, _w1] = _v_patternctor; return covered_so_far.concat(mlc::Array<mlc::String>{constructor_name}); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [record_name, _w0, _w1] = _v_patternrecord; return record_name != mlc::String("") ? covered_so_far.concat(mlc::Array<mlc::String>{record_name}) : covered_so_far; } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.fold(covered_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> alternative) mutable { return append_covered_constructors(alternative, accumulator); }); } return covered_so_far; }();}

mlc::String join_variant_names(mlc::Array<mlc::String> variant_names) noexcept{return variant_names.fold(mlc::String(""), [](mlc::String joined_so_far, mlc::String variant_name) mutable { return joined_so_far == mlc::String("") ? variant_name : joined_so_far + mlc::String(", ") + variant_name; });}

bool match_arm_covers_for_exhaustiveness(std::shared_ptr<ast::MatchArm> match_arm) noexcept{return !match_arm->has_guard;}

mlc::Array<ast::Diagnostic> match_exhaustiveness_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> empty_diagnostics = {};
mlc::String algebraic_type_name = subject_algebraic_type_name(subject_type);
return algebraic_type_name == mlc::String("") ? empty_diagnostics : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> required_variants = registry::TypeRegistry_algebraic_decl_variant_names_for(registry, algebraic_type_name);
  return required_variants.size() == 0 ? empty_diagnostics : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> covered_constructors = {};
  bool has_catch_all = false;
  int arm_index = 0;
  while (arm_index < match_arms.size()){
{
std::shared_ptr<ast::MatchArm> match_arm = match_arms[arm_index];
if (match_arm_covers_for_exhaustiveness(match_arm)){
{
if (pattern_is_catch_all(match_arm->pattern)){
has_catch_all = true;
} else {
covered_constructors = append_covered_constructors(match_arm->pattern, covered_constructors);
}
}
}
arm_index = arm_index + 1;
}
}
  return has_catch_all ? empty_diagnostics : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> missing_variants = required_variants.filter([covered_constructors](mlc::String variant_name) mutable { return !covered_constructors.any([variant_name](mlc::String covered_name) mutable { return covered_name == variant_name; }); });
  return missing_variants.size() == 0 ? empty_diagnostics : mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("non-exhaustive match: missing patterns for ") + join_variant_names(missing_variants), ast::expr_span(subject), diagnostic_codes::diagnostic_code_e084())};
 }();
 }();
 }();
}

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult subject_parsed = infer_expr_fn(subject, inference_context);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = substitution::substitution_from_generic(subject_parsed.inferred_type, inference_context.registry);
mlc::Array<ast::Diagnostic> collected_errors = subject_parsed.errors;
if (should_apply_string_match_rules(subject_parsed.inferred_type, match_arms)){
{
collected_errors = ast::diagnostics_append(collected_errors, string_match_subject_type_diagnostic(subject, subject_parsed.inferred_type));
collected_errors = ast::diagnostics_append(collected_errors, string_match_pattern_diagnostic(match_arms));
}
}
collected_errors = ast::diagnostics_append(collected_errors, match_exhaustiveness_diagnostic(subject, subject_parsed.inferred_type, match_arms, inference_context.registry));
std::shared_ptr<registry::Type> arm_type = std::make_shared<registry::Type>((registry::TUnknown{}));
int arm_index = 0;
while (arm_index < match_arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, match_arms[arm_index]->pattern, inference_context.registry, substitution, subject_parsed.inferred_type);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
if (match_arms[arm_index]->has_guard){
{
collected_errors = ast::diagnostics_append(collected_errors, pattern_guard_unsupported_tuple_or_array_diagnostic(match_arms[arm_index]->pattern));
infer_result::InferResult guard_inference = infer_expr_fn(match_arms[arm_index]->when_condition, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, guard_inference.errors);
if (!semantic_type_structure::type_is_unknown(guard_inference.inferred_type) && !semantic_type_structure::type_is_bool(guard_inference.inferred_type)){
collected_errors = ast::diagnostics_append(collected_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard must have type bool"), ast::expr_span(match_arms[arm_index]->when_condition), diagnostic_codes::diagnostic_code_e032())});
}
}
}
infer_result::InferResult arm_parsed = infer_expr_fn(match_arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_parsed.errors);
arm_type = arm_index == 0 ? arm_parsed.inferred_type : arm_type;
mlc::Array<ast::Diagnostic> arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_parsed.inferred_type, match_arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = arm_index + 1;
}
}
return infer_result::InferResult{arm_type, collected_errors};
}

} // namespace infer_match

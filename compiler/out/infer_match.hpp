#ifndef INFER_MATCH_HPP
#define INFER_MATCH_HPP

#include "mlc.hpp"
#include <variant>
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

struct String_match_duplicate_literal_fold_state {
  mlc::HashMap<mlc::String, bool> seen_literals;
  mlc::Array<ast::Diagnostic> errors;
};
struct String_match_wildcard_fold_state {
  mlc::Array<ast::Diagnostic> errors;
  int wildcard_count;
  int arm_index;
  int last_arm_index;
};
bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept;
bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept;
bool pattern_is_wildcard_only(std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::Array<mlc::String> pattern_string_literals_flat(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> literals_so_far) noexcept;
bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
bool should_apply_string_match_rules(std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
mlc::Array<ast::Diagnostic> string_match_subject_type_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type) noexcept;
mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) noexcept;
mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
String_match_duplicate_literal_fold_state string_match_duplicate_literal_fold_step(String_match_duplicate_literal_fold_state state, mlc::String literal_value, std::shared_ptr<ast::Pattern> pattern_for_diagnostic) noexcept;
mlc::Array<ast::Diagnostic> string_match_duplicate_literal_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
String_match_wildcard_fold_state string_match_wildcard_fold_step(String_match_wildcard_fold_state state, std::shared_ptr<ast::MatchArm> match_arm) noexcept;
mlc::Array<ast::Diagnostic> string_match_wildcard_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
mlc::Array<ast::Diagnostic> string_match_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::String subject_algebraic_type_name(std::shared_ptr<registry::Type> subject_type) noexcept;
bool pattern_is_catch_all(std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::Array<mlc::String> append_covered_constructors(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> covered_so_far) noexcept;
mlc::String join_variant_names(mlc::Array<mlc::String> variant_names) noexcept;
bool match_arm_covers_for_exhaustiveness(std::shared_ptr<ast::MatchArm> match_arm) noexcept;
mlc::Array<ast::Diagnostic> match_exhaustiveness_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, registry::TypeRegistry registry) noexcept;
template<typename __F3>
infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, check_context::CheckContext inference_context, __F3 infer_expr_fn) noexcept;
template<typename __F3>
infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, check_context::CheckContext inference_context, __F3 infer_expr_fn) noexcept{
  auto subject_parsed = infer_expr_fn(subject, inference_context);
  auto substitution = substitution::substitution_from_generic(subject_parsed.inferred_type, inference_context.registry);
  auto collected_errors = subject_parsed.errors;
  if (should_apply_string_match_rules(subject_parsed.inferred_type, match_arms))   {
    (collected_errors = ast::diagnostics_append(collected_errors, string_match_subject_type_diagnostic(subject, subject_parsed.inferred_type)));
    (collected_errors = ast::diagnostics_append(collected_errors, string_match_pattern_diagnostic(match_arms)));
  }
  (collected_errors = ast::diagnostics_append(collected_errors, match_exhaustiveness_diagnostic(subject, subject_parsed.inferred_type, match_arms, inference_context.registry)));
  auto arm_type = std::make_shared<registry::Type>(registry::TUnknown{});
  auto arm_index = 0;
  while ((arm_index < match_arms.length()))   {
    auto arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, match_arms[arm_index]->pattern, inference_context.registry, substitution, subject_parsed.inferred_type);
    auto arm_context = check_context::check_context_child(inference_context, arm_environment);
    if (match_arms[arm_index]->has_guard)     {
      (collected_errors = ast::diagnostics_append(collected_errors, pattern_guard_unsupported_tuple_or_array_diagnostic(match_arms[arm_index]->pattern)));
      auto guard_inference = infer_expr_fn(match_arms[arm_index]->when_condition, arm_context);
      (collected_errors = ast::diagnostics_append(collected_errors, guard_inference.errors));
      if (((!semantic_type_structure::type_is_unknown(guard_inference.inferred_type)) && (!semantic_type_structure::type_is_bool(guard_inference.inferred_type))))       {
        (collected_errors = ast::diagnostics_append(collected_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard must have type bool", 31), ast::expr_span(match_arms[arm_index]->when_condition), diagnostic_codes::diagnostic_code_e032())}));
      }
    }
    auto arm_parsed = infer_expr_fn(match_arms[arm_index]->body, arm_context);
    (collected_errors = ast::diagnostics_append(collected_errors, arm_parsed.errors));
    (arm_type = ((arm_index == 0) ? (arm_parsed.inferred_type) : (arm_type)));
    auto arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_parsed.inferred_type, match_arms[arm_index]->body);
    (collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch));
    (arm_index = (arm_index + 1));
  }
  return infer_result::InferResult{arm_type, collected_errors};
}

} // namespace infer_match

#endif // INFER_MATCH_HPP

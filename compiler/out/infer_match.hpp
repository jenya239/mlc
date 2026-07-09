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
infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_match

#endif // INFER_MATCH_HPP

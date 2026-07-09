#define main mlc_user_main
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

bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [__0, __1] = patternStringLit; return true;
}
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [__0, __1] = patternStr; return true;
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.any([=](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_contains_string_literal(alternative); });
}
return false;
std::abort();
}();
}
bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [__0, __1] = patternStringLit; return true;
}
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [__0, __1] = patternStr; return true;
}
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return true;
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.all([=](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_is_string_match_pattern(alternative); });
}
return false;
std::abort();
}();
}
bool pattern_is_wildcard_only(std::shared_ptr<ast::Pattern> pattern) noexcept{
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
mlc::Array<mlc::String> pattern_string_literals_flat(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> literals_so_far) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternStringLit>((*__match_subject))) {
const ast::PatternStringLit& patternStringLit = std::get<ast::PatternStringLit>((*__match_subject));
auto [value, __1] = patternStringLit; return literals_so_far.concat(mlc::Array<mlc::String>{value});
}
if (std::holds_alternative<ast::PatternStr>((*__match_subject))) {
const ast::PatternStr& patternStr = std::get<ast::PatternStr>((*__match_subject));
auto [value, __1] = patternStr; return literals_so_far.concat(mlc::Array<mlc::String>{value});
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.fold(literals_so_far, [=](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> alternative_pattern) mutable { return pattern_string_literals_flat(alternative_pattern, accumulator); });
}
return literals_so_far;
std::abort();
}();
}
bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  return match_arms.any([=](std::shared_ptr<ast::MatchArm> match_arm) mutable { return pattern_contains_string_literal(match_arm->pattern); });
}
bool should_apply_string_match_rules(std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  return (match_arms_use_string_literals(match_arms) || semantic_type_structure::type_is_string(subject_type));
}
mlc::Array<ast::Diagnostic> string_match_subject_type_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type) noexcept{
  if ((semantic_type_structure::type_is_unknown(subject_type) || semantic_type_structure::type_is_string(subject_type)))   {
    auto empty_list = mlc::Array<ast::Diagnostic>{};
    return empty_list;
  } else   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string literal match requires subject type string", 49), ast::expr_span(subject), diagnostic_codes::diagnostic_code_e077())};
  }
}
mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) noexcept{
  if (pattern_is_string_match_pattern(match_arm->pattern))   {
    return diagnostics_so_far;
  } else   {
    return diagnostics_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string match arm must use string literal or wildcard pattern", 60), ast::pattern_span(match_arm->pattern), diagnostic_codes::diagnostic_code_e079())});
  }
}
mlc::Array<ast::Diagnostic> string_match_invalid_arm_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  return match_arms.fold([&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), [=](mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::MatchArm> match_arm) mutable { return string_match_invalid_arm_pattern_fold_step(diagnostics_so_far, match_arm); });
}
String_match_duplicate_literal_fold_state string_match_duplicate_literal_fold_step(String_match_duplicate_literal_fold_state state, mlc::String literal_value, std::shared_ptr<ast::Pattern> pattern_for_diagnostic) noexcept{
  if (state.seen_literals.has(literal_value))   {
    return String_match_duplicate_literal_fold_state{state.seen_literals, state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("duplicate string literal in match", 33), ast::pattern_span(pattern_for_diagnostic), diagnostic_codes::diagnostic_code_e078())})};
  } else   {
    auto seen_literals = state.seen_literals;
    seen_literals.set(literal_value, true);
    return String_match_duplicate_literal_fold_state{seen_literals, state.errors};
  }
}
mlc::Array<ast::Diagnostic> string_match_duplicate_literal_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  auto empty_literals = mlc::Array<mlc::String>{};
  return match_arms.fold(String_match_duplicate_literal_fold_state{{}, [&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}()}, [=](String_match_duplicate_literal_fold_state fold_state, std::shared_ptr<ast::MatchArm> match_arm) mutable { return pattern_string_literals_flat(match_arm->pattern, empty_literals).fold(fold_state, [=](String_match_duplicate_literal_fold_state state_after_literals, mlc::String literal_value) mutable { return string_match_duplicate_literal_fold_step(state_after_literals, literal_value, match_arm->pattern); }); }).errors;
}
String_match_wildcard_fold_state string_match_wildcard_fold_step(String_match_wildcard_fold_state state, std::shared_ptr<ast::MatchArm> match_arm) noexcept{
  auto is_wildcard_arm = pattern_is_wildcard_only(match_arm->pattern);
  auto wildcard_count_after_arm = (is_wildcard_arm ? ((state.wildcard_count + 1)) : (state.wildcard_count));
  auto errors_after_arm = ((is_wildcard_arm && (state.arm_index != state.last_arm_index)) ? (state.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("wildcard match arm must be last for string match", 48), ast::pattern_span(match_arm->pattern), diagnostic_codes::diagnostic_code_e080())})) : (state.errors));
  return String_match_wildcard_fold_state{errors_after_arm, wildcard_count_after_arm, (state.arm_index + 1), state.last_arm_index};
}
mlc::Array<ast::Diagnostic> string_match_wildcard_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  auto last_arm_index = (-1);
  auto arm_count_index = 0;
  while ((arm_count_index < match_arms.length()))   {
    (last_arm_index = arm_count_index);
    (arm_count_index = (arm_count_index + 1));
  }
  auto fold_result = match_arms.fold(String_match_wildcard_fold_state{[&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), 0, 0, last_arm_index}, [=](String_match_wildcard_fold_state fold_state, std::shared_ptr<ast::MatchArm> match_arm) mutable { return string_match_wildcard_fold_step(fold_state, match_arm); });
  if ((fold_result.wildcard_count > 1))   {
    return fold_result.errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("string match allows at most one wildcard arm", 44), ast::pattern_span(match_arms[0]->pattern), diagnostic_codes::diagnostic_code_e080())});
  } else   {
    return fold_result.errors;
  }
}
mlc::Array<ast::Diagnostic> string_match_pattern_diagnostic(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
  return ast::diagnostics_append(ast::diagnostics_append([&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), string_match_invalid_arm_pattern_diagnostic(match_arms)), ast::diagnostics_append(string_match_duplicate_literal_diagnostic(match_arms), string_match_wildcard_diagnostic(match_arms)));
}
mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternTuple>((*__match_subject))) {
const ast::PatternTuple& patternTuple = std::get<ast::PatternTuple>((*__match_subject));
auto [__0, span] = patternTuple; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard is not supported with tuple patterns", 48), span, diagnostic_codes::diagnostic_code_e032())};
}
if (std::holds_alternative<ast::PatternArray>((*__match_subject))) {
const ast::PatternArray& patternArray = std::get<ast::PatternArray>((*__match_subject));
auto [__0, __1, span] = patternArray; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("match guard is not supported with array patterns", 48), span, diagnostic_codes::diagnostic_code_e032())};
}
return [&]() {
auto empty_list = mlc::Array<ast::Diagnostic>{};
return empty_list;
}();
std::abort();
}();
}
mlc::String subject_algebraic_type_name(std::shared_ptr<registry::Type> subject_type) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = subject_type;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [type_name] = tNamed; return type_name;
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [type_name, __1] = tGeneric; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
bool pattern_is_catch_all(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return true;
}
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [__0, __1] = patternIdent; return true;
}
return false;
std::abort();
}();
}
mlc::Array<mlc::String> append_covered_constructors(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> covered_so_far) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [constructor_name, __1, __2] = patternCtor; return covered_so_far.concat(mlc::Array<mlc::String>{constructor_name});
}
if (std::holds_alternative<ast::PatternRecord>((*__match_subject))) {
const ast::PatternRecord& patternRecord = std::get<ast::PatternRecord>((*__match_subject));
auto [record_name, __1, __2] = patternRecord; return ((record_name != mlc::String("", 0)) ? (covered_so_far.concat(mlc::Array<mlc::String>{record_name})) : (covered_so_far));
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.fold(covered_so_far, [=](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> alternative) mutable { return append_covered_constructors(alternative, accumulator); });
}
return covered_so_far;
std::abort();
}();
}
mlc::String join_variant_names(mlc::Array<mlc::String> variant_names) noexcept{
  return variant_names.fold(mlc::String("", 0), [=](mlc::String joined_so_far, mlc::String variant_name) mutable { return ((joined_so_far == mlc::String("", 0)) ? (variant_name) : (((joined_so_far + mlc::String(", ", 2)) + variant_name))); });
}
bool match_arm_covers_for_exhaustiveness(std::shared_ptr<ast::MatchArm> match_arm) noexcept{
  return (!match_arm->has_guard);
}
mlc::Array<ast::Diagnostic> match_exhaustiveness_diagnostic(std::shared_ptr<ast::Expr> subject, std::shared_ptr<registry::Type> subject_type, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, registry::TypeRegistry registry) noexcept{
  auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
  auto algebraic_type_name = subject_algebraic_type_name(subject_type);
  if ((algebraic_type_name == mlc::String("", 0)))   {
    return empty_diagnostics;
  } else   {
    auto required_variants = registry::TypeRegistry_algebraic_decl_variant_names_for(registry, algebraic_type_name);
    if ((required_variants.length() == 0))     {
      return empty_diagnostics;
    } else     {
      auto covered_constructors = mlc::Array<mlc::String>{};
      auto has_catch_all = false;
      auto arm_index = 0;
      while ((arm_index < match_arms.length()))       {
        auto match_arm = match_arms[arm_index];
        if (match_arm_covers_for_exhaustiveness(match_arm))         {
          if (pattern_is_catch_all(match_arm->pattern))           {
            (has_catch_all = true);
          } else           {
            (covered_constructors = append_covered_constructors(match_arm->pattern, covered_constructors));
          }
        }
        (arm_index = (arm_index + 1));
      }
      if (has_catch_all)       {
        return empty_diagnostics;
      } else       {
        auto missing_variants = required_variants.filter([=](mlc::String variant_name) mutable { return (!covered_constructors.any([=](mlc::String covered_name) mutable { return (covered_name == variant_name); })); });
        if ((missing_variants.length() == 0))         {
          return empty_diagnostics;
        } else         {
          return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("non-exhaustive match: missing patterns for ", 43) + join_variant_names(missing_variants)), ast::expr_span(subject), diagnostic_codes::diagnostic_code_e084())};
        }
      }
    }
  }
}

} // namespace infer_match

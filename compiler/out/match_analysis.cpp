#define main mlc_user_main
#include "match_analysis.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"

namespace match_analysis {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace ast_tokens;

bool subject_needs_deref(std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_ir::sexpr_type(subject);
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; return true;
}
return false;
std::abort();
}();
}
bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SemanticMatchArm> first_arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{
  return [&]() -> bool {
auto __match_subject = first_arm->pattern;
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [name, __1, __2] = patternCtor; return [&]() {
auto resolved = context::CodegenContext_resolve(context, name);
return (((resolved.length() >= 12) && (resolved.substring(0, 12) == mlc::String("ast_tokens::", 12))) ? (false) : ((decl_index::list_contains(context.generic_variants, name) ? (false) : (subject_needs_deref(subject, context)))));
}();
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return ((alternatives.length() > 0) ? (first_arm_needs_deref(std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{alternatives[0], first_arm->has_guard, first_arm->when_condition, first_arm->body}), subject, context)) : (subject_needs_deref(subject, context)));
}
return subject_needs_deref(subject, context);
std::abort();
}();
}
mlc::Array<mlc::String> pattern_binding_name_list(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < sub_patterns.length()))   {
    auto bind_name = [&]() -> mlc::String {
auto __match_subject = sub_patterns[index];
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [name, __1] = patternIdent; return cpp_naming::cpp_safe(name);
}
return (mlc::String("__", 2) + mlc::to_string(index));
std::abort();
}();
    parts.push_back(bind_name);
    (index = mlc::arith::checked_add(index, 1));
  }
  return parts;
}
mlc::String pattern_bind_names(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept{
  return pattern_binding_name_list(sub_patterns).join(mlc::String(", ", 2));
}
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
bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{
  return arms.any([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_contains_string_literal(arm->pattern); });
}
bool subject_is_string_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_ir::sexpr_type(subject);
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool subject_is_bool_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_ir::sexpr_type(subject);
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool should_use_string_match_if_chain(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{
  if ((arms.length() == 0))   {
    return false;
  } else   {
    return ((match_arms_use_string_literals(arms) || subject_is_string_type(subject)) && arms.all([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_is_string_match_pattern(arm->pattern); }));
  }
}

} // namespace match_analysis

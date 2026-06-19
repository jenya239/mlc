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

bool subject_needs_deref(std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SemanticMatchArm> first_arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> pattern_binding_name_list(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept;

mlc::String pattern_bind_names(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept;

bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

bool subject_is_string_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept;

bool should_use_string_match_if_chain(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

bool subject_needs_deref(std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(subject)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(subject))); auto [_w0] = _v_tshared; return true; } return false; }();}

bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SemanticMatchArm> first_arm, std::shared_ptr<semantic_ir::SemanticExpression> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::PatternCtor>((*first_arm->pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*first_arm->pattern)); auto [name, _w0, _w1] = _v_patternctor; return [&]() -> bool { 
  mlc::String resolved = context::CodegenContext_resolve(context, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : decl_index::list_contains(context.generic_variants, name) ? false : subject_needs_deref(subject, context);
 }(); } if (std::holds_alternative<ast::PatternOr>((*first_arm->pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*first_arm->pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.size() > 0 ? first_arm_needs_deref(std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{alternatives[0], first_arm->has_guard, first_arm->when_condition, first_arm->body}), subject, context) : subject_needs_deref(subject, context); } return subject_needs_deref(subject, context); }();}

mlc::Array<mlc::String> pattern_binding_name_list(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < sub_patterns.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatternIdent>((*sub_patterns[index]))) { auto _v_patternident = std::get<ast::PatternIdent>((*sub_patterns[index])); auto [name, _w0] = _v_patternident; return cpp_naming::cpp_safe(name); } return mlc::String("__") + mlc::to_string(index); }();
parts.push_back(bind_name);
index = index + 1;
}
}
return parts;
}

mlc::String pattern_bind_names(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns) noexcept{return pattern_binding_name_list(sub_patterns).join(mlc::String(", "));}

bool pattern_contains_string_literal(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [_w0, _w1] = _v_patternstringlit; return true; } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [_w0, _w1] = _v_patternstr; return true; } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.any([](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_contains_string_literal(alternative); }); } return false; }();}

bool pattern_is_string_match_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternStringLit>((*pattern))) { auto _v_patternstringlit = std::get<ast::PatternStringLit>((*pattern)); auto [_w0, _w1] = _v_patternstringlit; return true; } if (std::holds_alternative<ast::PatternStr>((*pattern))) { auto _v_patternstr = std::get<ast::PatternStr>((*pattern)); auto [_w0, _w1] = _v_patternstr; return true; } if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return true; } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.all([](std::shared_ptr<ast::Pattern> alternative) mutable { return pattern_is_string_match_pattern(alternative); }); } return false; }();}

bool match_arms_use_string_literals(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return arms.any([](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_contains_string_literal(arm->pattern); });}

bool subject_is_string_type(std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{return [&]() { if (std::holds_alternative<registry::TString>((*semantic_ir::sexpr_type(subject)))) {  return true; } return false; }();}

bool should_use_string_match_if_chain(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return arms.size() == 0 ? false : match_arms_use_string_literals(arms) || subject_is_string_type(subject) && arms.all([](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return pattern_is_string_match_pattern(arm->pattern); });}

} // namespace match_analysis

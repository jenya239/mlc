#define main mlc_user_main
#include "match_analysis.hpp"

namespace match_analysis {

bool subject_needs_deref(std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return true; },
[&](const auto& __v) { return false; }
}, (*semantic_ir::sexpr_type(subject)));
}
bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SMatchArm> first_arm, std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [name, __1, __2] = patCtor; return [&]() {
auto resolved = context::context_resolve(context, name);
return (((resolved.length() >= 12) && (resolved.substring(0, 12) == mlc::String("ast_tokens::", 12))) ? false : (decl_index::list_contains(context.generic_variants, name) ? false : subject_needs_deref(subject, context)));
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? first_arm_needs_deref(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{alts[0], first_arm->body}), subject, context) : subject_needs_deref(subject, context)); },
[&](const auto& __v) { return subject_needs_deref(subject, context); }
}, (*first_arm->pat));
}
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < sub_patterns.length())) {
auto bind_name = std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name, __1] = patIdent; return cpp_naming::cpp_safe(name); },
[&](const auto& __v) { return (mlc::String("__", 2) + mlc::to_string(index)); }
}, (*sub_patterns[index]));
parts.push_back(bind_name);
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}

} // namespace match_analysis

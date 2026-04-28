#include "match_analysis.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"

namespace match_analysis {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace ast_tokens;

bool subject_needs_deref(std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SMatchArm> first_arm, std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

bool subject_needs_deref(std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(subject)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(subject))); auto [_w0] = _v_tshared; return true; } return false; }();}

bool first_arm_needs_deref(std::shared_ptr<semantic_ir::SMatchArm> first_arm, std::shared_ptr<semantic_ir::SExpr> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::PatCtor>((*first_arm->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*first_arm->pat)); auto [name, _w0, _w1] = _v_patctor; return [&]() -> bool { 
  mlc::String resolved = context::context_resolve(context, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : decl_index::list_contains(context.generic_variants, name) ? false : subject_needs_deref(subject, context);
 }(); } if (std::holds_alternative<ast::PatOr>((*first_arm->pat))) { auto _v_pator = std::get<ast::PatOr>((*first_arm->pat)); auto [alts, _w0] = _v_pator; return alts.size() > 0 ? first_arm_needs_deref(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{alts[0], first_arm->body}), subject, context) : subject_needs_deref(subject, context); } return subject_needs_deref(subject, context); }();}

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < sub_patterns.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[index])); auto [name, _w0] = _v_patident; return cpp_naming::cpp_safe(name); } return mlc::String("__") + mlc::to_string(index); }();
parts.push_back(bind_name);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

} // namespace match_analysis

#include "match_analysis.hpp"

#include "ast.hpp"
#include "context.hpp"

namespace match_analysis {

using namespace ast;
using namespace context;
using namespace ast_tokens;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept{
return context::expr_returns_shared_sum_visit_ptr(subject) ? true : [&]() { if (std::holds_alternative<ast::ExprIdent>((*subject)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*subject)._); auto [name, _w0] = _v_exprident; return context.match_deref_params.contains(name) ? true : context.shared_params.contains(name) ? true : context.value_params.contains(name) ? false : true; } if (std::holds_alternative<ast::ExprIndex>((*subject)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*subject)._); auto [array_expr, _w0, _w1] = _v_exprindex; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*array_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*array_expr)._); auto [array_name, _w0] = _v_exprident; return context.shared_array_params.contains(array_name); } return true; }(); } if (std::holds_alternative<ast::ExprMethod>((*subject)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*subject)._); auto [_w0, _w1, _w2, _w3] = _v_exprmethod; return false; } return true; }();
}

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::PatCtor>((*first_arm->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*first_arm->pat)); auto [name, _w0, _w1] = _v_patctor; return [&]() -> bool { 
  mlc::String resolved = context::context_resolve(context, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : context::list_contains(context.generic_variants, name) ? false : subject_needs_deref(subject, context);
 }(); } return subject_needs_deref(subject, context); }();}

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < sub_patterns.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*sub_patterns[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_patterns[index])); auto [name, _w0] = _v_patident; return context::cpp_safe(name); } return mlc::String("__") + mlc::to_string(index); }();
parts.push_back(bind_name);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

} // namespace match_analysis

#ifndef MATCH_ANALYSIS_HPP
#define MATCH_ANALYSIS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"

namespace match_analysis {

struct Expr;
struct Stmt;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

} // namespace match_analysis

#endif // MATCH_ANALYSIS_HPP

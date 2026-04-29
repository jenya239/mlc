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

namespace infer_match {

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept;
infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_match

#endif // INFER_MATCH_HPP

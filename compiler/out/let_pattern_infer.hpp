#ifndef LET_PATTERN_INFER_HPP
#define LET_PATTERN_INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "pattern_env.hpp"

namespace let_pattern_infer {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

} // namespace let_pattern_infer

#endif // LET_PATTERN_INFER_HPP

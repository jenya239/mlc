#ifndef CHECK_MUTATIONS_HPP
#define CHECK_MUTATIONS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace check_mutations {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept;

} // namespace check_mutations

#endif // CHECK_MUTATIONS_HPP

#ifndef CHECK_HPP
#define CHECK_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "names.hpp"
#include "registry.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"

namespace check {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};

bool CheckOut_has_errors(check::CheckOut self) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

} // namespace check

#endif // CHECK_HPP

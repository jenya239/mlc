#ifndef PASS_HPP
#define PASS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace pass {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

using CheckerPass = CompilerPass<ast::Program, ast::Program>;

} // namespace pass

#endif // PASS_HPP

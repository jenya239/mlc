#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "transform_decl.hpp"
#include "registry.hpp"
#include "module.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "ast.hpp"

namespace mlc_main {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

} // namespace mlc_main

#endif // MAIN_HPP

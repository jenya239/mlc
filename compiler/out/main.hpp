#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "decl_index.hpp"
#include "ast.hpp"
#include "pipeline.hpp"
#include "compile_options.hpp"
#include "profile.hpp"

namespace mlc_main {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

} // namespace mlc_main

#endif // MAIN_HPP

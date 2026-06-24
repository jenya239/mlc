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
#include "format_cli.hpp"
#include "lsp_cli.hpp"
#include "profile.hpp"
#include "header_import.hpp"

namespace mlc_main {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

} // namespace mlc_main

#endif // MAIN_HPP

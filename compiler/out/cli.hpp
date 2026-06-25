#ifndef CLI_HPP
#define CLI_HPP

#include "mlc.hpp"
#include <variant>

#include "compile_options.hpp"
#include "format_cli.hpp"
#include "lsp_cli.hpp"
#include "compile_driver.hpp"

namespace cli {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

int run_compiler_cli() noexcept;

} // namespace cli

#endif // CLI_HPP

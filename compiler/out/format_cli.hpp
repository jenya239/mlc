#ifndef FORMAT_CLI_HPP
#define FORMAT_CLI_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast_printer.hpp"

namespace format_cli {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String format_usage_message() noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> format_mlc_source_text(mlc::String source, mlc::String source_path) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> format_source_file(mlc::String source_path) noexcept;

} // namespace format_cli

#endif // FORMAT_CLI_HPP

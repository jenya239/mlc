#ifndef HEADER_IMPORT_HPP
#define HEADER_IMPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "ast.hpp"
#include "registry.hpp"

namespace header_import {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct HeaderImportResult {mlc::Array<std::shared_ptr<ast::Decl>> declarations;mlc::Array<mlc::String> errors;};

bool is_cpp_header_path(mlc::String path) noexcept;

std::shared_ptr<ast::TypeExpr> cpp_type_string_to_type_expr(mlc::String type_string) noexcept;

header_import::HeaderImportResult parse_cpp_header_source(mlc::String source_text) noexcept;

header_import::HeaderImportResult load_cpp_header_decls(mlc::String path) noexcept;

registry::TypeRegistry registry_from_cpp_header_source(mlc::String source_text) noexcept;

} // namespace header_import

#endif // HEADER_IMPORT_HPP

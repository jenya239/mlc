#ifndef LOAD_ITEM_HPP
#define LOAD_ITEM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace load_item {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct NamespaceImportAlias {mlc::String alias;mlc::String module_path;};

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;mlc::Array<load_item::NamespaceImportAlias> namespace_import_aliases;};

} // namespace load_item

#endif // LOAD_ITEM_HPP

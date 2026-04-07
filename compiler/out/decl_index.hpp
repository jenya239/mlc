#ifndef DECL_INDEX_HPP
#define DECL_INDEX_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct NamespaceImportAlias {mlc::String alias;mlc::String module_path;};

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;mlc::Array<decl_index::NamespaceImportAlias> namespace_import_aliases;};

struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<decl_index::NamespaceImportAlias> aliases) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<decl_index::LoadItem> all_items) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> build_item_index(mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

} // namespace decl_index

#endif // DECL_INDEX_HPP

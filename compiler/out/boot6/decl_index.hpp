#ifndef DECL_INDEX_HPP
#define DECL_INDEX_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

struct NamespaceImportAlias {mlc::String alias;mlc::String module_path;};
struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;mlc::Array<NamespaceImportAlias> namespace_import_aliases;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<NamespaceImportAlias> aliases) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept;
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>> build_item_index(mlc::Array<LoadItem> all_items) noexcept;

} // namespace decl_index

#endif // DECL_INDEX_HPP

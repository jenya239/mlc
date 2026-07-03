#ifndef DECL_INDEX_HPP
#define DECL_INDEX_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "load_item.hpp"
#include "cpp_naming.hpp"
namespace decl_index {

struct FieldOrder {
  mlc::String type_name;
  mlc::Array<mlc::String> fields;
};
mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String function_name_from_method_declaration(std::shared_ptr<ast::Decl> method) noexcept;
mlc::String extend_type_name_from_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String extend_method_mangled_name(mlc::String type_name, mlc::String method_name) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> ast_decls_for_path(mlc::String module_path, mlc::Array<load_item::LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> extend_qualified_map(mlc::HashMap<mlc::String, mlc::String> base, mlc::Array<load_item::LoadItem> all_items, mlc::String exclude_module_path) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_extend_method_qualified_map(mlc::Array<load_item::LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<load_item::LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<load_item::NamespaceImportAlias> aliases) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<load_item::LoadItem> all_items) noexcept;
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_field_order_index(mlc::Array<std::shared_ptr<FieldOrder>> orders) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> build_item_index(mlc::Array<load_item::LoadItem> all_items) noexcept;

} // namespace decl_index

#endif // DECL_INDEX_HPP

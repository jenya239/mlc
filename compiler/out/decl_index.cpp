#define main mlc_user_main
#include "decl_index.hpp"

#include "ast.hpp"
#include "load_item.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

using namespace ast;
using namespace load_item;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_string_list() noexcept{
  auto empty = mlc::Array<mlc::String>{};
  return empty;
}
mlc::Array<std::shared_ptr<ast::Decl>> empty_decl_list() noexcept{
  auto empty = mlc::Array<std::shared_ptr<ast::Decl>>{};
  return empty;
}
mlc::String type_variant_export_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto unwrapped = variant;
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name, __1] = varUnit; return variant_name; },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1, __2] = varTuple; return variant_name; },
[&](const ast::VarRecord& varRecord) { auto [variant_name, __1, __2] = varRecord; return variant_name; }
}, (*unwrapped));
}
mlc::String method_declaration_function_name(std::shared_ptr<ast::Decl> method) noexcept{
  auto unwrapped = method;
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5, __6] = declFn; return name; },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return mlc::String("", 0); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return mlc::String("", 0); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return mlc::String("", 0); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return mlc::String("", 0); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return method_declaration_function_name(inner); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return mlc::String("", 0); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return mlc::String("", 0); }
}, (*unwrapped));
}
void append_export_names_from_method_declarations(mlc::Array<mlc::String>& names, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    auto function_name = method_declaration_function_name(methods[method_index]);
    if ((function_name != mlc::String("", 0)))     {
      names.push_back(function_name);
    }
    (method_index = (method_index + 1));
  }
  std::make_tuple();
}
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto unwrapped = ast::decl_inner(declaration);
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return empty_decl_list(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return empty_decl_list(); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return empty_decl_list(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, methods, __3] = declTrait; return methods; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return empty_decl_list(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return empty_decl_list(); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return methods_from_trait_declaration(inner); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return empty_decl_list(); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return empty_decl_list(); }
}, (*unwrapped));
}
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto unwrapped = ast::decl_inner(declaration);
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return empty_decl_list(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return empty_decl_list(); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return empty_decl_list(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return empty_decl_list(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, methods, __3] = declExtend; return methods; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return empty_decl_list(); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return methods_from_extend_declaration(inner); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return empty_decl_list(); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return empty_decl_list(); }
}, (*unwrapped));
}
mlc::Array<mlc::String> export_names_from_exported_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto unwrapped = ast::decl_inner(declaration);
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return mlc::Array<mlc::String>{ast::decl_name(declaration)}; },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return mlc::Array<mlc::String>{ast::decl_name(declaration)}; },
[&](const ast::DeclType& declType) { auto [__0, __1, variants, __3, __4] = declType; return [&]() {
auto names = mlc::Array<mlc::String>{ast::decl_name(declaration)};
auto variant_index = 0;
while ((variant_index < variants.length())) {
  names.push_back(type_variant_export_name(variants[variant_index]));
  (variant_index = (variant_index + 1));
}
return names;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return [&]() {
auto names = mlc::Array<mlc::String>{ast::decl_name(declaration)};
append_export_names_from_method_declarations(names, methods_from_trait_declaration(declaration));
return names;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return [&]() {
auto names = empty_string_list();
append_export_names_from_method_declarations(names, methods_from_extend_declaration(declaration));
return names;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return empty_string_list(); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return export_names_from_exported_declaration(inner); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return empty_string_list(); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return empty_string_list(); }
}, (*unwrapped));
}
mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto unwrapped = declaration;
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return empty_string_list(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return empty_string_list(); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return empty_string_list(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return empty_string_list(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return empty_string_list(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return empty_string_list(); },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return export_names_from_exported_declaration(declaration); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return empty_string_list(); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return empty_string_list(); }
}, (*unwrapped));
}
mlc::String function_name_from_method_declaration(std::shared_ptr<ast::Decl> method) noexcept{
  return method_declaration_function_name(method);
}
mlc::String extend_type_name_from_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto unwrapped = ast::decl_inner(declaration);
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return mlc::String("", 0); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return mlc::String("", 0); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return mlc::String("", 0); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return mlc::String("", 0); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, __1, __2, __3] = declExtend; return type_name; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return extend_type_name_from_declaration(inner); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return mlc::String("", 0); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return mlc::String("", 0); }
}, (*unwrapped));
}
mlc::Array<std::shared_ptr<ast::Decl>> ast_decls_for_path(mlc::String module_path, mlc::Array<load_item::LoadItem> all_items) noexcept{
  auto item_index = 0;
  while ((item_index < all_items.length()))   {
    if ((all_items[item_index].path == module_path))     {
      return all_items[item_index].decls;
    }
    (item_index = (item_index + 1));
  }
  auto empty = mlc::Array<std::shared_ptr<ast::Decl>>{};
  return empty;
}
mlc::HashMap<mlc::String, mlc::String> extend_qualified_map(mlc::HashMap<mlc::String, mlc::String> base, mlc::Array<load_item::LoadItem> all_items, mlc::String exclude_module_path) noexcept{
  auto qualified = base;
  auto item_index = 0;
  while ((item_index < all_items.length()))   {
    auto current_item = all_items[item_index];
    if ((current_item.path != exclude_module_path))     {
      auto module_prefix = (cpp_naming::path_to_module_base(current_item.path) + mlc::String("::", 2));
      auto declaration_index = 0;
      [&]() {
while ((declaration_index < current_item.decls.length())) {
auto type_name = extend_type_name_from_declaration(current_item.decls[declaration_index]);
if ((type_name != mlc::String("", 0))) {
  auto methods = methods_from_extend_declaration(current_item.decls[declaration_index]);
  auto method_index = 0;
  [&]() {
while ((method_index < methods.length())) {
auto method_name = function_name_from_method_declaration(methods[method_index]);
if ((method_name != mlc::String("", 0))) {
  auto mangled_name = extend_method_mangled_name(type_name, method_name);
  if ((!qualified.has(mangled_name)))   {
    qualified.set(mangled_name, module_prefix);
  }
}
(method_index = (method_index + 1));
}
}();
}
(declaration_index = (declaration_index + 1));
}
}();
    }
    (item_index = (item_index + 1));
  }
  return qualified;
}
mlc::HashMap<mlc::String, mlc::String> build_extend_method_qualified_map(mlc::Array<load_item::LoadItem> all_items) noexcept{
  return extend_qualified_map({}, all_items, mlc::String("", 0));
}
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<load_item::LoadItem> all_items) noexcept{
  auto prefix = (cpp_naming::path_to_module_base(imp_path) + mlc::String("::", 2));
  auto item_index = 0;
  while ((item_index < all_items.length()))   {
    auto current_item = all_items[item_index];
    if ((current_item.path == imp_path))     {
      auto declaration_index = 0;
      [&]() {
while ((declaration_index < current_item.decls.length())) {
auto name_index = 0;
auto export_names = declaration_export_names(current_item.decls[declaration_index]);
while ((name_index < export_names.length())) {
  if ((!qualified.has(export_names[name_index])))   {
    qualified.set(export_names[name_index], prefix);
  }
  (name_index = (name_index + 1));
}
(declaration_index = (declaration_index + 1));
}
}();
    }
    (item_index = (item_index + 1));
  }
  return qualified;
}
mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<load_item::NamespaceImportAlias> aliases) noexcept{
  auto prefixes = mlc::HashMap<mlc::String, mlc::String>();
  auto index = 0;
  while ((index < aliases.length()))   {
    auto entry = aliases[index];
    prefixes.set(entry.alias, (cpp_naming::path_to_module_base(entry.module_path) + mlc::String("::", 2)));
    (index = (index + 1));
  }
  return prefixes;
}
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<load_item::LoadItem> all_items) noexcept{
  auto qualified = mlc::HashMap<mlc::String, mlc::String>();
  auto import_index = 0;
  while ((import_index < import_paths.length()))   {
    auto import_path = import_paths[import_index];
    (qualified = add_exports_to_qualified(qualified, import_path, all_items));
    auto item_index = 0;
    while ((item_index < all_items.length()))     {
      auto current_item = all_items[item_index];
      if ((current_item.path == import_path))       {
        auto transitive_import_index = 0;
        [&]() {
while ((transitive_import_index < current_item.imports.length())) {
(qualified = add_exports_to_qualified(qualified, current_item.imports[transitive_import_index], all_items));
(transitive_import_index = (transitive_import_index + 1));
}
}();
      }
      (item_index = (item_index + 1));
    }
    (import_index = (import_index + 1));
  }
  return qualified;
}
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
  auto found = false;
  auto index = 0;
  while (((index < list.length()) && (!found)))   {
    if ((list[index] == item))     {
      (found = true);
    }
    (index = (index + 1));
  }
  return found;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
  auto result = mlc::Array<mlc::String>{};
  auto index = 0;
  auto found = false;
  while (((index < orders.length()) && (!found)))   {
    if ((orders[index]->type_name == name))     {
      (result = orders[index]->fields);
      (found = true);
    }
    (index = (index + 1));
  }
  return result;
}
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_field_order_index(mlc::Array<std::shared_ptr<FieldOrder>> orders) noexcept{
  auto index = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
  auto order_index = 0;
  while ((order_index < orders.length()))   {
    index.set(orders[order_index]->type_name, orders[order_index]->fields);
    (order_index = (order_index + 1));
  }
  return index;
}
mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> build_item_index(mlc::Array<load_item::LoadItem> all_items) noexcept{
  auto item_index = mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>>();
  auto index = 0;
  while ((index < all_items.length()))   {
    item_index.set(all_items[index].path, std::make_shared<load_item::LoadItem>(all_items[index]));
    (index = (index + 1));
  }
  return item_index;
}
mlc::String extend_method_mangled_name(mlc::String type_name, mlc::String method_name) noexcept{
  auto prefix = (type_name + mlc::String("_", 1));
  if (((method_name.length() >= prefix.length()) && (method_name.substring(0, prefix.length()) == prefix)))   {
    return method_name;
  } else   {
    return ((type_name + mlc::String("_", 1)) + method_name);
  }
}

} // namespace decl_index

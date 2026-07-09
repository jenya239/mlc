#include "decl_index.hpp"

#include "ast.hpp"
#include "load_item.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

using namespace ast;
using namespace load_item;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_string_list() noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> empty_decl_list() noexcept;

mlc::String type_variant_export_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String method_declaration_function_name(std::shared_ptr<ast::Decl> method) noexcept;

std::tuple<> append_export_names_from_method_declarations(mlc::Array<mlc::String>& names, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> export_names_from_exported_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::String function_name_from_method_declaration(std::shared_ptr<ast::Decl> method) noexcept;

mlc::String extend_type_name_from_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> ast_decls_for_path(mlc::String module_path, mlc::Array<load_item::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> extend_qualified_map(mlc::HashMap<mlc::String, mlc::String> base, mlc::Array<load_item::LoadItem> all_items, mlc::String exclude_module_path) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_extend_method_qualified_map(mlc::Array<load_item::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<load_item::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<load_item::NamespaceImportAlias> aliases) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<load_item::LoadItem> all_items) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_field_order_index(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> build_item_index(mlc::Array<load_item::LoadItem> all_items) noexcept;

mlc::String extend_method_mangled_name(mlc::String type_name, mlc::String method_name) noexcept;

mlc::Array<mlc::String> empty_string_list() noexcept{
mlc::Array<mlc::String> empty = {};
return empty;
}

mlc::Array<std::shared_ptr<ast::Decl>> empty_decl_list() noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> empty = {};
return empty;
}

mlc::String type_variant_export_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<ast::TypeVariant> unwrapped = variant;
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [variant_name, _w0] = varunit; return variant_name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [variant_name, _w0, _w1] = vartuple; return variant_name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [variant_name, _w0, _w1] = varrecord; return variant_name; }
}, (*unwrapped));
}

mlc::String method_declaration_function_name(std::shared_ptr<ast::Decl> method) noexcept{
std::shared_ptr<ast::Decl> unwrapped = method;
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::String { auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = declfn; return name; },
  [&](const DeclType& decltype_) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = decltype_; return mlc::String(""); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::String { auto [_w0, _w1, _w2, _w3] = decltypealias; return mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [_w0, _w1, _w2, _w3] = decltrait; return mlc::String(""); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [_w0, _w1, _w2, _w3] = declextend; return mlc::String(""); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [inner] = declexported; return method_declaration_function_name(inner); },
  [&](const DeclAssocType& declassoctype) -> mlc::String { auto [_w0, _w1] = declassoctype; return mlc::String(""); },
  [&](const DeclAssocBind& declassocbind) -> mlc::String { auto [_w0, _w1, _w2] = declassocbind; return mlc::String(""); }
}, (*unwrapped));
}

std::tuple<> append_export_names_from_method_declarations(mlc::Array<mlc::String>& names, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
int method_index = 0;
while (method_index < methods.size()){
{
mlc::String function_name = method_declaration_function_name(methods[method_index]);
if (function_name != mlc::String("")){
{
names.push_back(function_name);
}
}
method_index = method_index + 1;
}
}
return std::make_tuple();
}

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
std::shared_ptr<ast::Decl> unwrapped = ast::decl_inner(declaration);
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return empty_decl_list(); },
  [&](const DeclType& decltype_) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3, _w4] = decltype_; return empty_decl_list(); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3] = decltypealias; return empty_decl_list(); },
  [&](const DeclTrait& decltrait) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, methods, _w2] = decltrait; return methods; },
  [&](const DeclExtend& declextend) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3] = declextend; return empty_decl_list(); },
  [&](const DeclImport& declimport) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1] = declimport; return empty_decl_list(); },
  [&](const DeclExported& declexported) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [inner] = declexported; return methods_from_trait_declaration(inner); },
  [&](const DeclAssocType& declassoctype) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1] = declassoctype; return empty_decl_list(); },
  [&](const DeclAssocBind& declassocbind) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2] = declassocbind; return empty_decl_list(); }
}, (*unwrapped));
}

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
std::shared_ptr<ast::Decl> unwrapped = ast::decl_inner(declaration);
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return empty_decl_list(); },
  [&](const DeclType& decltype_) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3, _w4] = decltype_; return empty_decl_list(); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3] = decltypealias; return empty_decl_list(); },
  [&](const DeclTrait& decltrait) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2, _w3] = decltrait; return empty_decl_list(); },
  [&](const DeclExtend& declextend) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, methods, _w2] = declextend; return methods; },
  [&](const DeclImport& declimport) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1] = declimport; return empty_decl_list(); },
  [&](const DeclExported& declexported) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [inner] = declexported; return methods_from_extend_declaration(inner); },
  [&](const DeclAssocType& declassoctype) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1] = declassoctype; return empty_decl_list(); },
  [&](const DeclAssocBind& declassocbind) -> mlc::Array<std::shared_ptr<ast::Decl>> { auto [_w0, _w1, _w2] = declassocbind; return empty_decl_list(); }
}, (*unwrapped));
}

mlc::Array<mlc::String> export_names_from_exported_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
std::shared_ptr<ast::Decl> unwrapped = ast::decl_inner(declaration);
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return mlc::Array<mlc::String>{ast::decl_name(declaration)}; },
  [&](const DeclTypeAlias& decltypealias) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = decltypealias; return mlc::Array<mlc::String>{ast::decl_name(declaration)}; },
  [&](const DeclType& decltype_) -> mlc::Array<mlc::String> { auto [_w0, _w1, variants, _w2, _w3] = decltype_; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> names = mlc::Array<mlc::String>{ast::decl_name(declaration)};
  int variant_index = 0;
  while (variant_index < variants.size()){
{
names.push_back(type_variant_export_name(variants[variant_index]));
variant_index = variant_index + 1;
}
}
  return names;
 }(); },
  [&](const DeclTrait& decltrait) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = decltrait; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> names = mlc::Array<mlc::String>{ast::decl_name(declaration)};
  append_export_names_from_method_declarations(names, methods_from_trait_declaration(declaration));
  return names;
 }(); },
  [&](const DeclExtend& declextend) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = declextend; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> names = empty_string_list();
  append_export_names_from_method_declarations(names, methods_from_extend_declaration(declaration));
  return names;
 }(); },
  [&](const DeclImport& declimport) -> mlc::Array<mlc::String> { auto [_w0, _w1] = declimport; return empty_string_list(); },
  [&](const DeclExported& declexported) -> mlc::Array<mlc::String> { auto [inner] = declexported; return export_names_from_exported_declaration(inner); },
  [&](const DeclAssocType& declassoctype) -> mlc::Array<mlc::String> { auto [_w0, _w1] = declassoctype; return empty_string_list(); },
  [&](const DeclAssocBind& declassocbind) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2] = declassocbind; return empty_string_list(); }
}, (*unwrapped));
}

mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept{
std::shared_ptr<ast::Decl> unwrapped = declaration;
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return empty_string_list(); },
  [&](const DeclType& decltype_) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3, _w4] = decltype_; return empty_string_list(); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = decltypealias; return empty_string_list(); },
  [&](const DeclTrait& decltrait) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = decltrait; return empty_string_list(); },
  [&](const DeclExtend& declextend) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2, _w3] = declextend; return empty_string_list(); },
  [&](const DeclImport& declimport) -> mlc::Array<mlc::String> { auto [_w0, _w1] = declimport; return empty_string_list(); },
  [&](const DeclExported& declexported) -> mlc::Array<mlc::String> { auto [_w0] = declexported; return export_names_from_exported_declaration(declaration); },
  [&](const DeclAssocType& declassoctype) -> mlc::Array<mlc::String> { auto [_w0, _w1] = declassoctype; return empty_string_list(); },
  [&](const DeclAssocBind& declassocbind) -> mlc::Array<mlc::String> { auto [_w0, _w1, _w2] = declassocbind; return empty_string_list(); }
}, (*unwrapped));
}

mlc::String function_name_from_method_declaration(std::shared_ptr<ast::Decl> method) noexcept{return method_declaration_function_name(method);}

mlc::String extend_type_name_from_declaration(std::shared_ptr<ast::Decl> declaration) noexcept{
std::shared_ptr<ast::Decl> unwrapped = ast::decl_inner(declaration);
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return mlc::String(""); },
  [&](const DeclType& decltype_) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = decltype_; return mlc::String(""); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::String { auto [_w0, _w1, _w2, _w3] = decltypealias; return mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [_w0, _w1, _w2, _w3] = decltrait; return mlc::String(""); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, _w0, _w1, _w2] = declextend; return type_name; },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [inner] = declexported; return extend_type_name_from_declaration(inner); },
  [&](const DeclAssocType& declassoctype) -> mlc::String { auto [_w0, _w1] = declassoctype; return mlc::String(""); },
  [&](const DeclAssocBind& declassocbind) -> mlc::String { auto [_w0, _w1, _w2] = declassocbind; return mlc::String(""); }
}, (*unwrapped));
}

mlc::Array<std::shared_ptr<ast::Decl>> ast_decls_for_path(mlc::String module_path, mlc::Array<load_item::LoadItem> all_items) noexcept{
int item_index = 0;
while (item_index < all_items.size()){
{
if (all_items[item_index].path == module_path){
{
return all_items[item_index].decls;
}
}
item_index = item_index + 1;
}
}
return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { 
  mlc::Array<std::shared_ptr<ast::Decl>> empty = {};
  return empty;
 }();
}

mlc::HashMap<mlc::String, mlc::String> extend_qualified_map(mlc::HashMap<mlc::String, mlc::String> base, mlc::Array<load_item::LoadItem> all_items, mlc::String exclude_module_path) noexcept{
mlc::HashMap<mlc::String, mlc::String> qualified = base;
int item_index = 0;
while (item_index < all_items.size()){
{
load_item::LoadItem current_item = all_items[item_index];
if (current_item.path != exclude_module_path){
{
mlc::String module_prefix = cpp_naming::path_to_module_base(current_item.path) + mlc::String("::");
int declaration_index = 0;
[&]() { 
  while (declaration_index < current_item.decls.size()){
{
mlc::String type_name = extend_type_name_from_declaration(current_item.decls[declaration_index]);
if (type_name != mlc::String("")){
{
mlc::Array<std::shared_ptr<ast::Decl>> methods = methods_from_extend_declaration(current_item.decls[declaration_index]);
int method_index = 0;
[&]() { 
  while (method_index < methods.size()){
{
mlc::String method_name = function_name_from_method_declaration(methods[method_index]);
if (method_name != mlc::String("")){
{
mlc::String mangled_name = extend_method_mangled_name(type_name, method_name);
if (!qualified.has(mangled_name)){
qualified.set(mangled_name, module_prefix);
}
}
}
method_index = method_index + 1;
}
}
 }();
}
}
declaration_index = declaration_index + 1;
}
}
 }();
}
}
item_index = item_index + 1;
}
}
return qualified;
}

mlc::HashMap<mlc::String, mlc::String> build_extend_method_qualified_map(mlc::Array<load_item::LoadItem> all_items) noexcept{return extend_qualified_map(mlc::HashMap<mlc::String, mlc::String>(), all_items, mlc::String(""));}

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<load_item::LoadItem> all_items) noexcept{
mlc::String prefix = cpp_naming::path_to_module_base(imp_path) + mlc::String("::");
int item_index = 0;
while (item_index < all_items.size()){
{
load_item::LoadItem current_item = all_items[item_index];
if (current_item.path == imp_path){
{
int declaration_index = 0;
[&]() { 
  while (declaration_index < current_item.decls.size()){
{
int name_index = 0;
mlc::Array<mlc::String> export_names = declaration_export_names(current_item.decls[declaration_index]);
while (name_index < export_names.size()){
{
if (!qualified.has(export_names[name_index])){
{
qualified.set(export_names[name_index], prefix);
}
}
name_index = name_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
 }();
}
}
item_index = item_index + 1;
}
}
return qualified;
}

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<load_item::NamespaceImportAlias> aliases) noexcept{
mlc::HashMap<mlc::String, mlc::String> prefixes = mlc::HashMap<mlc::String, mlc::String>();
int index = 0;
while (index < aliases.size()){
{
load_item::NamespaceImportAlias entry = aliases[index];
prefixes.set(entry.alias, cpp_naming::path_to_module_base(entry.module_path) + mlc::String("::"));
index = index + 1;
}
}
return prefixes;
}

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<load_item::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, mlc::String> qualified = mlc::HashMap<mlc::String, mlc::String>();
int import_index = 0;
while (import_index < import_paths.size()){
{
mlc::String import_path = import_paths[import_index];
qualified = add_exports_to_qualified(qualified, import_path, all_items);
int item_index = 0;
while (item_index < all_items.size()){
{
load_item::LoadItem current_item = all_items[item_index];
if (current_item.path == import_path){
{
int transitive_import_index = 0;
[&]() { 
  while (transitive_import_index < current_item.imports.size()){
{
qualified = add_exports_to_qualified(qualified, current_item.imports[transitive_import_index], all_items);
transitive_import_index = transitive_import_index + 1;
}
}
 }();
}
}
item_index = item_index + 1;
}
}
import_index = import_index + 1;
}
}
return qualified;
}

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
bool found = false;
int index = 0;
while (index < list.size() && !found){
{
if (list[index] == item){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept{
mlc::Array<mlc::String> result = {};
int index = 0;
bool found = false;
while (index < orders.size() && !found){
{
if (orders[index]->type_name == name){
{
result = orders[index]->fields;
found = true;
}
}
index = index + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_field_order_index(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders) noexcept{
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> index = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int order_index = 0;
while (order_index < orders.size()){
{
index.set(orders[order_index]->type_name, orders[order_index]->fields);
order_index = order_index + 1;
}
}
return index;
}

mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> build_item_index(mlc::Array<load_item::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> item_index = mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>>();
int index = 0;
while (index < all_items.size()){
{
item_index.set(all_items[index].path, std::make_shared<load_item::LoadItem>(all_items[index]));
index = index + 1;
}
}
return item_index;
}

mlc::String extend_method_mangled_name(mlc::String type_name, mlc::String method_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return method_name.length() >= prefix.length() && method_name.substring(0, prefix.length()) == prefix ? method_name : type_name + mlc::String("_") + method_name;
}

} // namespace decl_index

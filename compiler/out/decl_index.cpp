#include "decl_index.hpp"

#include "ast.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

using namespace ast;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<decl_index::NamespaceImportAlias> aliases) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<decl_index::LoadItem> all_items) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_field_order_index(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> build_item_index(mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::Array<mlc::String> declaration_export_names(std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::Array<mlc::String> names = {};
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*inner_declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*inner_declaration)); auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(name);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTypeAlias>((*inner_declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*inner_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltypealias; return [&]() -> std::tuple<> { 
  names.push_back(type_name);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclType>((*inner_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*inner_declaration)); auto [type_name, _w0, variants, _w1, _w2] = _v_decltype; return [&]() -> std::tuple<> { 
  names.push_back(type_name);
  int variant_index = 0;
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> std::tuple<> { auto [variant_name, _w0, _w1] = varrecord; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) -> std::tuple<> { auto [variant_name, _w0, _w1] = vartuple; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarUnit& varunit) -> std::tuple<> { auto [variant_name, _w0] = varunit; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTrait>((*inner_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*inner_declaration)); auto [name, _w0, methods, _w1] = _v_decltrait; return [&]() -> std::tuple<> { 
  names.push_back(name);
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(function_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*inner_declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*inner_declaration)); auto [_w0, _w1, methods, _w2] = _v_declextend; return [&]() -> std::tuple<> { 
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(function_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
return names;
}

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::String prefix = cpp_naming::path_to_module_base(imp_path) + mlc::String("::");
int item_index = 0;
while (item_index < all_items.size()){
{
decl_index::LoadItem current_item = all_items[item_index];
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

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<decl_index::NamespaceImportAlias> aliases) noexcept{
mlc::HashMap<mlc::String, mlc::String> prefixes = mlc::HashMap<mlc::String, mlc::String>();
int index = 0;
while (index < aliases.size()){
{
decl_index::NamespaceImportAlias entry = aliases[index];
prefixes.set(entry.alias, cpp_naming::path_to_module_base(entry.module_path) + mlc::String("::"));
index = index + 1;
}
}
return prefixes;
}

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, mlc::String> qualified = mlc::HashMap<mlc::String, mlc::String>();
int import_index = 0;
while (import_index < import_paths.size()){
{
mlc::String import_path = import_paths[import_index];
qualified = add_exports_to_qualified(qualified, import_path, all_items);
int item_index = 0;
while (item_index < all_items.size()){
{
decl_index::LoadItem current_item = all_items[item_index];
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

mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> build_item_index(mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> item_index = mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>>();
int index = 0;
while (index < all_items.size()){
{
item_index.set(all_items[index].path, std::make_shared<decl_index::LoadItem>(all_items[index]));
index = index + 1;
}
}
return item_index;
}

} // namespace decl_index

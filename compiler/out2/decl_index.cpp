#include "decl_index.hpp"

#include "ast.hpp"
#include "cpp_naming.hpp"

namespace decl_index {

using namespace ast;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<decl_index::NamespaceImportAlias> aliases) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<decl_index::LoadItem> all_items) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> build_item_index(mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept{
mlc::Array<mlc::String> names = {};
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclExported>((*decl))) { auto _v_declexported = std::get<ast::DeclExported>((*decl)); auto [d] = _v_declexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*d))) { auto _v_declfn = std::get<ast::DeclFn>((*d)); auto [name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(name);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclType>((*d))) { auto _v_decltype = std::get<ast::DeclType>((*d)); auto [type_name, _w0, variants, _w1] = _v_decltype; return [&]() -> std::tuple<> { 
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
 }(); } if (std::holds_alternative<ast::DeclTrait>((*d))) { auto _v_decltrait = std::get<ast::DeclTrait>((*d)); auto [name, _w0, methods] = _v_decltrait; return [&]() -> std::tuple<> { 
  names.push_back(name);
  int mi = 0;
  while (mi < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(fn_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
mi = mi + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*d))) { auto _v_declextend = std::get<ast::DeclExtend>((*d)); auto [_w0, _w1, methods] = _v_declextend; return [&]() -> std::tuple<> { 
  int mi = 0;
  while (mi < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(fn_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
mi = mi + 1;
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
int item_i = 0;
while (item_i < all_items.size()){
{
decl_index::LoadItem cur_item = all_items[item_i];
if (cur_item.path == imp_path){
{
int decl_index = 0;
[&]() { 
  while (decl_index < cur_item.decls.size()){
{
int name_index = 0;
mlc::Array<mlc::String> exp_names = decl_export_names(cur_item.decls[decl_index]);
while (name_index < exp_names.size()){
{
if (!qualified.has(exp_names[name_index])){
{
qualified.set(exp_names[name_index], prefix);
}
}
name_index = name_index + 1;
}
}
decl_index = decl_index + 1;
}
}
 }();
}
}
item_i = item_i + 1;
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
int imp_i = 0;
while (imp_i < import_paths.size()){
{
mlc::String imp_path = import_paths[imp_i];
qualified = add_exports_to_qualified(qualified, imp_path, all_items);
int item_i = 0;
while (item_i < all_items.size()){
{
decl_index::LoadItem cur_item = all_items[item_i];
if (cur_item.path == imp_path){
{
int trans_i = 0;
[&]() { 
  while (trans_i < cur_item.imports.size()){
{
qualified = add_exports_to_qualified(qualified, cur_item.imports[trans_i], all_items);
trans_i = trans_i + 1;
}
}
 }();
}
}
item_i = item_i + 1;
}
}
imp_i = imp_i + 1;
}
}
return qualified;
}

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
bool found = false;
int i = 0;
while (i < list.size() && !found){
{
if (list[i] == item){
{
found = true;
}
}
i = i + 1;
}
}
return found;
}

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders, mlc::String name) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
bool found = false;
while (i < orders.size() && !found){
{
if (orders[i]->type_name == name){
{
result = orders[i]->fields;
found = true;
}
}
i = i + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> build_item_index(mlc::Array<decl_index::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> item_index = mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>>();
int i = 0;
while (i < all_items.size()){
{
item_index.set(all_items[i].path, std::make_shared<decl_index::LoadItem>(all_items[i]));
i = i + 1;
}
}
return item_index;
}

} // namespace decl_index

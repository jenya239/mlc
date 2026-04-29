#define main mlc_user_main
#include "decl_index.hpp"

namespace decl_index {

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept{
auto names = mlc::Array<mlc::String>{};
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [type_name, __1, variants, __3] = declType; return [&]() {
names.push_back(type_name);
auto variant_index = 0;
while ((variant_index < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); }
}, (*variants[variant_index]));
variant_index = (variant_index + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods] = declTrait; return [&]() {
names.push_back(name);
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(fn_name);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[mi]));
mi = (mi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, methods] = declExtend; return [&]() {
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(fn_name);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[mi]));
mi = (mi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*d));
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*decl));
return names;
}
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept{
auto prefix = (cpp_naming::path_to_module_base(imp_path) + mlc::String("::", 2));
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto decl_index = 0;
[&]() {
while ((decl_index < cur_item.decls.length())) {
auto name_index = 0;
auto exp_names = decl_export_names(cur_item.decls[decl_index]);
while ((name_index < exp_names.length())) {
if ((!qualified.has(exp_names[name_index]))) {
qualified.set(exp_names[name_index], prefix);
}
name_index = (name_index + 1);
}
decl_index = (decl_index + 1);
}
}();
}
item_i = (item_i + 1);
}
return qualified;
}
mlc::HashMap<mlc::String, mlc::String> build_namespace_alias_prefixes(mlc::Array<NamespaceImportAlias> aliases) noexcept{
auto prefixes = mlc::HashMap<mlc::String, mlc::String>();
auto index = 0;
while ((index < aliases.length())) {
auto entry = aliases[index];
prefixes.set(entry.alias, (cpp_naming::path_to_module_base(entry.module_path) + mlc::String("::", 2)));
index = (index + 1);
}
return prefixes;
}
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept{
auto qualified = mlc::HashMap<mlc::String, mlc::String>();
auto imp_i = 0;
while ((imp_i < import_paths.length())) {
auto imp_path = import_paths[imp_i];
qualified = add_exports_to_qualified(qualified, imp_path, all_items);
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto trans_i = 0;
[&]() {
while ((trans_i < cur_item.imports.length())) {
qualified = add_exports_to_qualified(qualified, cur_item.imports[trans_i], all_items);
trans_i = (trans_i + 1);
}
}();
}
item_i = (item_i + 1);
}
imp_i = (imp_i + 1);
}
return qualified;
}
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
auto found = false;
auto i = 0;
while (((i < list.length()) && (!found))) {
if ((list[i] == item)) {
found = true;
}
i = (i + 1);
}
return found;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
auto found = false;
while (((i < orders.length()) && (!found))) {
if ((orders[i]->type_name == name)) {
result = orders[i]->fields;
found = true;
}
i = (i + 1);
}
return result;
}
mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>> build_item_index(mlc::Array<LoadItem> all_items) noexcept{
auto item_index = mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>>();
auto i = 0;
while ((i < all_items.length())) {
item_index.set(all_items[i].path, std::make_shared<LoadItem>(all_items[i]));
i = (i + 1);
}
return item_index;
}

} // namespace decl_index

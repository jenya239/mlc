#define main mlc_user_main
#include "type_index.hpp"

namespace type_index {

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
auto prefix = (type_name + mlc::String("_", 1));
if (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix))) {
return fn_name.substring(prefix.length(), (fn_name.length() - prefix.length()));
} else {
return fn_name;
}
}
mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept{
auto i = 0;
while ((i < decls.length())) {
std::visit(overloaded{[&](const ast::DeclTrait& declTrait) { auto [trait_name, __1, methods] = declTrait; return [&]() {
auto mi = 0;
return [&]() {
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
auto unmangled = extract_method_name(fn_name, trait_name);
if ((!owners.has(unmangled))) {
owners.set(unmangled, fn_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
}();
}(); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, __1, methods] = declExtend; return [&]() {
auto mi = 0;
return [&]() {
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
auto unmangled = extract_method_name(fn_name, type_name);
if ((!owners.has(unmangled))) {
owners.set(unmangled, fn_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*ast::decl_inner(decls[i])));
i = (i + 1);
}
return owners;
}
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
return add_method_owners_from_decls(decls, {});
}
mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < decls.length())) {
auto inner_decl = ast::decl_inner(decls[i]);
std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, type_params, variants, __3] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
auto should_add = (type_params.length() > 0);
auto vname = std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [n, __1] = varRecord; return n; },
[&](const ast::VarTuple& varTuple) { auto [n, __1] = varTuple; return n; },
[&](const ast::VarUnit& varUnit) { auto [n] = varUnit; return n; }
}, (*variants[vi]));
if (should_add) {
result.push_back(vname);
} else {
result.push_back(mlc::String("__skip__", 8));
}
vi = (vi + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*inner_decl));
i = (i + 1);
}
return result;
}
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto variant_map = mlc::HashMap<mlc::String, mlc::String>();
auto i = 0;
while ((i < decls.length())) {
auto inner_decl = ast::decl_inner(decls[i]);
std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants, __3] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return variant_map.set(variant_name, type_name); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return variant_map.set(variant_name, type_name); },
[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return variant_map.set(variant_name, type_name); }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*inner_decl));
i = (i + 1);
}
return variant_map;
}
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program prog) noexcept{
auto orders = mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants, __3] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto fi = 0;
while ((fi < field_defs.length())) {
field_names.push_back(field_defs[fi]->name);
fi = (fi + 1);
}
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
}
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return /* unit */; },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return /* unit */; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return /* unit */; },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return /* unit */; }
}, (*ast::decl_inner(inner))); },
[&](const ast::DeclType& declType) { auto [type_name, __1, variants, __3] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto fi = 0;
while ((fi < field_defs.length())) {
field_names.push_back(field_defs[fi]->name);
fi = (fi + 1);
}
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
}
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return /* unit */; },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return /* unit */; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return /* unit */; }
}, (*prog.decls[i]));
i = (i + 1);
}
return orders;
}

} // namespace type_index

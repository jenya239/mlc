#define main mlc_user_main
#include "ctor_info.hpp"

namespace ctor_info {

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{
return fd->typ;
}
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
auto shared_positions = mlc::Array<int>{};
auto shared_array_positions = mlc::Array<int>{};
auto index = 0;
while ((index < types.length())) {
if (param_analysis::is_shared_type(types[index])) {
shared_positions.push_back(index);
}
if (param_analysis::is_shared_array_type(types[index])) {
shared_array_positions.push_back(index);
}
index = (index + 1);
}
return std::make_shared<CtorTypeInfo>(CtorTypeInfo{ctor_name, shared_positions, shared_array_positions});
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto result = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
auto vi = 0;
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [variant_name, types] = varTuple; return result.push_back(ctor_type_info_for(variant_name, types)); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto fi = 0;
while ((fi < field_defs.length())) {
types.push_back(field_def_type(field_defs[fi]));
fi = (fi + 1);
}
return result.push_back(ctor_type_info_for(variant_name, types));
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
return result;
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto result = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
auto i = 0;
while ((i < decls.length())) {
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, variants, __3] = declType; return [&]() {
auto infos = add_ctor_infos_from_variants(variants);
auto ii = 0;
return [&]() {
while ((ii < infos.length())) {
result.push_back(infos[ii]);
ii = (ii + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*ast::decl_inner(inner))); },
[&](const ast::DeclType& declType) { auto [__0, __1, variants, __3] = declType; return [&]() {
auto infos = add_ctor_infos_from_variants(variants);
auto ii = 0;
return [&]() {
while ((ii < infos.length())) {
result.push_back(infos[ii]);
ii = (ii + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*decls[i]));
i = (i + 1);
}
return result;
}
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String cti_name) noexcept{
auto result = std::make_shared<CtorTypeInfo>(CtorTypeInfo{mlc::String("", 0), {}, {}});
auto i = 0;
while ((i < infos.length())) {
if ((infos[i]->name == cti_name)) {
result = infos[i];
}
i = (i + 1);
}
return result;
}

} // namespace ctor_info

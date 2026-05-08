#include "ctor_info.hpp"

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

using namespace ast;
using namespace param_analysis;
using namespace ast_tokens;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{return fd->typ;}

std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
mlc::Array<int> shared_positions = {};
mlc::Array<int> shared_array_positions = {};
int index = 0;
while (index < types.size()){
{
if (param_analysis::is_shared_type(types[index])){
{
shared_positions.push_back(index);
}
}
if (param_analysis::is_shared_array_type(types[index])){
{
shared_array_positions.push_back(index);
}
}
index = index + 1;
}
}
return std::make_shared<ctor_info::CtorTypeInfo>(ctor_info::CtorTypeInfo{ctor_name, shared_positions, shared_array_positions});
}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [variant_name, types, _w0] = vartuple; return mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>>{ctor_type_info_for(variant_name, types)}; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [variant_name, field_defs, _w0] = varrecord; return mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>>{ctor_type_info_for(variant_name, field_defs.map([](std::shared_ptr<ast::FieldDef> fd)  { return field_def_type(fd); }))}; },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [_w0, _w1] = varunit; return {}; }
}, (*variant));}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> variant)  { return variant_to_ctor_info(variant); });}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { if (std::holds_alternative<ast::DeclExported>((*decl))) { auto _v_declexported = std::get<ast::DeclExported>((*decl)); auto [inner] = _v_declexported; return [&]() -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(inner)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(inner))); auto [_w0, _w1, variants, _w2] = _v_decltype; return add_ctor_infos_from_variants(variants); } return {}; }(); } if (std::holds_alternative<ast::DeclType>((*decl))) { auto _v_decltype = std::get<ast::DeclType>((*decl)); auto [_w0, _w1, variants, _w2] = _v_decltype; return add_ctor_infos_from_variants(variants); } return {}; }();}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return mlc::collections::flat_map(decls, [](std::shared_ptr<ast::Decl> decl)  { return decl_to_ctor_infos(decl); });}

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos, mlc::String cti_name) noexcept{
std::shared_ptr<ctor_info::CtorTypeInfo> result = std::make_shared<ctor_info::CtorTypeInfo>(ctor_info::CtorTypeInfo{mlc::String(""), {}, {}});
int i = 0;
while (i < infos.size()){
{
if (infos[i]->name == cti_name){
{
result = infos[i];
}
}
i = i + 1;
}
}
return result;
}

} // namespace ctor_info

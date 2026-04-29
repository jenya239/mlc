#include "ctor_info.hpp"

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

using namespace ast;
using namespace param_analysis;
using namespace ast_tokens;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

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

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> result = {};
int vi = 0;
while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarTuple& vartuple) -> void { auto [variant_name, types, _w0] = vartuple; return result.push_back(ctor_type_info_for(variant_name, types)); },
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs, _w0] = varrecord; return [&]() { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
types.push_back(field_def_type(field_defs[fi]));
fi = fi + 1;
}
}
  return result.push_back(ctor_type_info_for(variant_name, types));
 }(); },
  [&](const VarUnit& varunit) -> void { auto [_w0, _w1] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> result = {};
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclExported>((*decls[i]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[i])); auto [inner] = _v_declexported; return [&]() -> void { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(inner)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(inner))); auto [_w0, _w1, variants, _w2] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
  int ii = 0;
  return [&]() { 
  while (ii < infos.size()){
{
result.push_back(infos[ii]);
ii = ii + 1;
}
}
 }();
 }(); } return; }(); } if (std::holds_alternative<ast::DeclType>((*decls[i]))) { auto _v_decltype = std::get<ast::DeclType>((*decls[i])); auto [_w0, _w1, variants, _w2] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
  int ii = 0;
  return [&]() { 
  while (ii < infos.size()){
{
result.push_back(infos[ii]);
ii = ii + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return result;
}

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

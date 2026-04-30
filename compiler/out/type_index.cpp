#include "type_index.hpp"

#include "ast.hpp"
#include "decl_index.hpp"

namespace type_index {

using namespace ast;
using namespace decl_index;
using namespace ast_tokens;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept{
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(decls[i])))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(decls[i]))); auto [trait_name, _w0, methods] = _v_decltrait; return [&]() { 
  int mi = 0;
  return [&]() { 
  while (mi < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(fn_name, trait_name);
  if (!owners.has(unmangled)){
owners.set(unmangled, fn_name);
}
 }(); } return; }();
mi = mi + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(decls[i])))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(decls[i]))); auto [type_name, _w0, methods] = _v_declextend; return [&]() { 
  int mi = 0;
  return [&]() { 
  while (mi < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(fn_name, type_name);
  if (!owners.has(unmangled)){
owners.set(unmangled, fn_name);
}
 }(); } return; }();
mi = mi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return owners;
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return add_method_owners_from_decls(decls, mlc::HashMap<mlc::String, mlc::String>());}

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < decls.size()){
{
std::shared_ptr<ast::Decl> inner_decl = ast::decl_inner(decls[i]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_decl))) { auto _v_decltype = std::get<ast::DeclType>((*inner_decl)); auto [_w0, type_params, variants, _w1] = _v_decltype; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
bool should_add = type_params.size() > 0;
mlc::String vname = std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [n, _w0, _w1] = varrecord; return n; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [n, _w0, _w1] = vartuple; return n; },
  [&](const VarUnit& varunit) -> mlc::String { auto [n, _w0] = varunit; return n; }
}, (*variants[vi]));
if (should_add){
{
result.push_back(vname);
}
} else {
{
result.push_back(mlc::String("__skip__"));
}
}
vi = vi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::HashMap<mlc::String, mlc::String> variant_map = mlc::HashMap<mlc::String, mlc::String>();
int i = 0;
while (i < decls.size()){
{
std::shared_ptr<ast::Decl> inner_decl = ast::decl_inner(decls[i]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_decl))) { auto _v_decltype = std::get<ast::DeclType>((*inner_decl)); auto [type_name, _w0, variants, _w1] = _v_decltype; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, _w0, _w1] = varrecord; return variant_map.set(variant_name, type_name); },
  [&](const VarTuple& vartuple) -> void { auto [variant_name, _w0, _w1] = vartuple; return variant_map.set(variant_name, type_name); },
  [&](const VarUnit& varunit) -> void { auto [variant_name, _w0] = varunit; return variant_map.set(variant_name, type_name); }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return variant_map;
}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclExported& declexported) -> void { auto [inner] = declexported; return std::visit(overloaded{
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants, _w1] = decltype_; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs, _w0] = varrecord; return [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
field_names.push_back(field_defs[fi]->name);
fi = fi + 1;
}
}
  orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
}
}
  return;
 }(); },
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1, _w2] = vartuple; return ; },
  [&](const VarUnit& varunit) -> void { auto [_w0, _w1] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return ; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2] = decltrait; return ; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2] = declextend; return ; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; return ; },
  [&](const DeclExported& declexported) -> void { auto [_w0] = declexported; return ; },
  [&](const DeclAssocType& declassoctype) -> void { auto [_w0, _w1] = declassoctype; return ; },
  [&](const DeclAssocBind& declassocbind) -> void { auto [_w0, _w1, _w2] = declassocbind; return ; }
}, (*ast::decl_inner(inner))); },
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants, _w1] = decltype_; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs, _w0] = varrecord; return [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
field_names.push_back(field_defs[fi]->name);
fi = fi + 1;
}
}
  orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
}
}
  return;
 }(); },
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1, _w2] = vartuple; return ; },
  [&](const VarUnit& varunit) -> void { auto [_w0, _w1] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; return ; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2] = decltrait; return ; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2] = declextend; return ; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; return ; },
  [&](const DeclAssocType& declassoctype) -> void { auto [_w0, _w1] = declassoctype; return ; },
  [&](const DeclAssocBind& declassocbind) -> void { auto [_w0, _w1, _w2] = declassocbind; return ; }
}, (*prog.decls[i]));
i = i + 1;
}
}
return orders;
}

} // namespace type_index

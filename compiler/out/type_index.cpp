#include "type_index.hpp"

#include "ast.hpp"
#include "decl_index.hpp"

namespace type_index {

using namespace ast;
using namespace decl_index;
using namespace ast_tokens;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept;

mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String> owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept{
int index = 0;
mlc::String result = fn_name;
while (index < fn_name.length()){
{
if (fn_name.substring(index, 1) == mlc::String("_")){
{
result = fn_name.substring(0, index);
index = fn_name.length();
}
} else {
{
index = index + 1;
}
}
}
}
return result;
}

mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::HashMap<mlc::String, bool> trait_names = mlc::HashMap<mlc::String, bool>();
int index = 0;
while (index < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(decls[index])))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(decls[index]))); auto [trait_name, _w0, _w1, _w2] = _v_decltrait; return trait_names.set(trait_name, true); } return; }();
index = index + 1;
}
}
return trait_names;
}

bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String> owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
return !owners.has(unmangled) ? true : [&]() -> bool { 
  mlc::String existing_prefix = extract_owner_prefix_from_mangled(owners.get(unmangled));
  return existing_prefix == type_name || trait_names.has(existing_prefix);
 }();
}

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String> owners) noexcept{
mlc::HashMap<mlc::String, bool> trait_names = build_trait_names_from_decls(declarations);
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(declarations[declaration_index])))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(declarations[declaration_index]))); auto [trait_name, _w0, methods, _w1] = _v_decltrait; return [&]() { 
  int method_index = 0;
  return [&]() { 
  while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(function_name, trait_name);
  if (!owners.has(unmangled)){
owners.set(unmangled, function_name);
}
 }(); } return; }();
method_index = method_index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(declarations[declaration_index])))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(declarations[declaration_index]))); auto [type_name, _w0, methods, _w1] = _v_declextend; return [&]() { 
  int method_index = 0;
  return [&]() { 
  while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(function_name, type_name);
  mlc::String type_owner_key = type_name + mlc::String("_") + unmangled;
  owners.set(type_owner_key, function_name);
  if (extend_may_register_method_owner(owners, unmangled, type_name, trait_names)){
owners.set(unmangled, function_name);
}
 }(); } return; }();
method_index = method_index + 1;
}
}
 }();
 }(); } return; }();
declaration_index = declaration_index + 1;
}
}
return owners;
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{return add_method_owners_from_decls(declarations, mlc::HashMap<mlc::String, mlc::String>());}

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::Array<mlc::String> result = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
std::shared_ptr<ast::Decl> inner_declaration = ast::decl_inner(declarations[declaration_index]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*inner_declaration)); auto [_w0, type_parameters, variants, _w1, _w2] = _v_decltype; return [&]() { 
  int variant_index = 0;
  return [&]() { 
  while (variant_index < variants.size()){
{
bool should_add = type_parameters.size() > 0;
mlc::String variant_name = std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; }
}, (*variants[variant_index]));
if (should_add){
{
result.push_back(variant_name);
}
} else {
{
result.push_back(mlc::String("__skip__"));
}
}
variant_index = variant_index + 1;
}
}
 }();
 }(); } return; }();
declaration_index = declaration_index + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::HashMap<mlc::String, mlc::String> variant_map = mlc::HashMap<mlc::String, mlc::String>();
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
std::shared_ptr<ast::Decl> inner_declaration = ast::decl_inner(declarations[declaration_index]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*inner_declaration)); auto [type_name, _w0, variants, _w1, _w2] = _v_decltype; return [&]() { 
  int variant_index = 0;
  return [&]() { 
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, _w0, _w1] = varrecord; variant_map.set(variant_name, type_name); },
  [&](const VarTuple& vartuple) -> void { auto [variant_name, _w0, _w1] = vartuple; variant_map.set(variant_name, type_name); },
  [&](const VarUnit& varunit) -> void { auto [variant_name, _w0] = varunit; variant_map.set(variant_name, type_name); }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
 }();
 }(); } return; }();
declaration_index = declaration_index + 1;
}
}
return variant_map;
}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders = {};
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
std::visit(overloaded{
  [&](const DeclExported& declexported) -> void { auto [inner] = declexported; std::visit(overloaded{
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants, _w1, _w2] = decltype_; [&]() { 
  int variant_index = 0;
  return [&]() { 
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_definitions, _w0] = varrecord; [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int field_index = 0;
  while (field_index < field_definitions.size()){
{
field_names.push_back(field_definitions[field_index]->name);
field_index = field_index + 1;
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
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1, _w2] = vartuple; },
  [&](const VarUnit& varunit) -> void { auto [_w0, _w1] = varunit; }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2, _w3] = decltrait; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2, _w3] = declextend; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; },
  [&](const DeclExported& declexported) -> void { auto [_w0] = declexported; },
  [&](const DeclAssocType& declassoctype) -> void { auto [_w0, _w1] = declassoctype; },
  [&](const DeclAssocBind& declassocbind) -> void { auto [_w0, _w1, _w2] = declassocbind; },
  [&](const DeclTypeAlias& decltypealias) -> void { auto [_w0, _w1, _w2, _w3] = decltypealias; }
}, (*ast::decl_inner(inner))); },
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants, _w1, _w2] = decltype_; [&]() { 
  int variant_index = 0;
  return [&]() { 
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_definitions, _w0] = varrecord; [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int field_index = 0;
  while (field_index < field_definitions.size()){
{
field_names.push_back(field_definitions[field_index]->name);
field_index = field_index + 1;
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
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1, _w2] = vartuple; },
  [&](const VarUnit& varunit) -> void { auto [_w0, _w1] = varunit; }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = declfn; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2, _w3] = decltrait; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2, _w3] = declextend; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; },
  [&](const DeclAssocType& declassoctype) -> void { auto [_w0, _w1] = declassoctype; },
  [&](const DeclAssocBind& declassocbind) -> void { auto [_w0, _w1, _w2] = declassocbind; },
  [&](const DeclTypeAlias& decltypealias) -> void { auto [_w0, _w1, _w2, _w3] = decltypealias; }
}, (*program.decls[declaration_index]));
declaration_index = declaration_index + 1;
}
}
return orders;
}

} // namespace type_index

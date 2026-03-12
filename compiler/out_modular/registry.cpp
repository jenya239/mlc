#include "registry.hpp"

#include "ast.hpp"

namespace registry {

using namespace ast;
using namespace ast_tokens;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program prog) noexcept;

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept;

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.fn_types.get(name);}

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.get(name);}

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_params.has(name) ? self.ctor_params.get(name) : mlc::Array<std::shared_ptr<registry::Type>>{};}

registry::TypeRegistry empty_registry() noexcept{return registry::TypeRegistry{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>()};}

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<ast::TyI32>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TI32{})); } if (std::holds_alternative<ast::TyString>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<ast::TyBool>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TBool{})); } if (std::holds_alternative<ast::TyUnit>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TUnit{})); } if (std::holds_alternative<ast::TyNamed>((*type_expr))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expr)); auto [name] = _v_tynamed; return std::make_shared<registry::Type>(registry::TNamed(name)); } if (std::holds_alternative<ast::TyArray>((*type_expr))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<registry::Type>(registry::TArray(registry::type_from_annotation(inner))); } if (std::holds_alternative<ast::TyShared>((*type_expr))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<registry::Type>(registry::TShared(registry::type_from_annotation(inner))); } if (std::holds_alternative<ast::TyFn>((*type_expr))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expr)); auto [params, ret] = _v_tyfn; return [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(registry::type_from_annotation(params[i]));
i = i + 1;
}
}
  return std::make_shared<registry::Type>(registry::TFn(param_types, registry::type_from_annotation(ret)));
 }(); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

registry::TypeRegistry build_registry(ast::Program prog) noexcept{
registry::TypeRegistry registry = registry::empty_registry();
int i = 0;
while (i < prog.decls.size()){
{
registry = registry::register_decl(registry, prog.decls[i]);
i = i + 1;
}
}
return registry;
}

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return registry; },
  [&](const DeclFn& declfn) { auto [name, params, return_type, _w0] = declfn; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(registry::type_from_annotation(params[i]->typ));
i = i + 1;
}
}
  registry.fn_types.set(name, std::make_shared<registry::Type>(registry::TFn(param_types, registry::type_from_annotation(return_type))));
  return registry;
 }(); },
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() -> registry::TypeRegistry { 
  int i = 0;
  while (i < variants.size()){
{
registry = registry::register_variant(registry, type_name, variants[i]);
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return registry; },
  [&](const DeclExported& declexported) { auto [d] = declexported; return registry::register_decl(registry, d); }
}, (*decl));
}

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<registry::Type> result_type = std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return [&]() -> registry::TypeRegistry { 
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  return registry;
 }(); },
  [&](const VarTuple& vartuple) { auto [variant_name, field_types] = vartuple; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> field_type_list = {};
  int i = 0;
  while (i < field_types.size()){
{
field_type_list.push_back(registry::type_from_annotation(field_types[i]));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, field_type_list);
  return registry;
 }(); },
  [&](const VarRecord& varrecord) { auto [variant_name, field_defs] = varrecord; return [&]() -> registry::TypeRegistry { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int i = 0;
  while (i < field_defs.size()){
{
field_map.set(field_defs[i]->name, registry::type_from_annotation(field_defs[i]->typ));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  registry.field_types.set(variant_name, field_map);
  registry.field_types.set(type_name, field_map);
  return registry;
 }(); }
}, (*variant));
}

} // namespace registry

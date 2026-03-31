#include "registry.hpp"

#include "ast.hpp"

namespace registry {

using namespace ast;
using namespace ast_tokens;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program prog) noexcept;

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept;

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.fn_types.get(name);}

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept{return self.fn_types.has(name);}

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.get(name);}

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.has(name);}

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_params.has(name) ? self.ctor_params.get(name) : mlc::Array<std::shared_ptr<registry::Type>>{};}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.get(type_name);}

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.has(type_name);}

registry::TypeRegistry empty_registry() noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> ctor_types = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> ctor_params = mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>>();
ctor_types.set(mlc::String("Ok"), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Result"))));
ctor_types.set(mlc::String("Err"), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Result"))));
mlc::Array<std::shared_ptr<registry::Type>> ok_params = {};
mlc::Array<std::shared_ptr<registry::Type>> err_params = {};
ok_params.push_back(std::make_shared<registry::Type>((registry::TUnknown{})));
err_params.push_back(std::make_shared<registry::Type>((registry::TUnknown{})));
ctor_params.set(mlc::String("Ok"), ok_params);
ctor_params.set(mlc::String("Err"), err_params);
return registry::TypeRegistry{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), ctor_types, ctor_params, mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>()};
}

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<ast::TyI32>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TI32{})); } if (std::holds_alternative<ast::TyString>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<ast::TyBool>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TBool{})); } if (std::holds_alternative<ast::TyUnit>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TUnit{})); } if (std::holds_alternative<ast::TyNamed>((*type_expr))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expr)); auto [name] = _v_tynamed; return std::make_shared<registry::Type>(registry::TNamed(name)); } if (std::holds_alternative<ast::TyArray>((*type_expr))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyShared>((*type_expr))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyGeneric>((*type_expr))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expr)); auto [name, targs] = _v_tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? type_from_annotation(targs[0]) : [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> typed_args = {};
  int i = 0;
  while (i < targs.size()){
{
typed_args.push_back(type_from_annotation(targs[i]));
i = i + 1;
}
}
  return std::make_shared<registry::Type>(registry::TGeneric(name, typed_args));
 }(); } if (std::holds_alternative<ast::TyFn>((*type_expr))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expr)); auto [params, ret] = _v_tyfn; return [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(params[i]));
i = i + 1;
}
}
  return std::make_shared<registry::Type>(registry::TFn(param_types, type_from_annotation(ret)));
 }(); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

registry::TypeRegistry build_registry(ast::Program prog) noexcept{
registry::TypeRegistry registry = empty_registry();
int i = 0;
while (i < prog.decls.size()){
{
registry = register_decl(registry, prog.decls[i]);
i = i + 1;
}
}
return registry;
}

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{
  [&](const DeclTrait& decltrait) -> registry::TypeRegistry { auto [_w0, _w1, methods] = decltrait; return [&]() -> registry::TypeRegistry { 
  int i = 0;
  while (i < methods.size()){
{
registry = register_decl(registry, methods[i]);
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclExtend& declextend) -> registry::TypeRegistry { auto [_w0, _w1, _w2] = declextend; return registry; },
  [&](const DeclFn& declfn) -> registry::TypeRegistry { auto [name, _w0, _w1, params, return_type, _w2] = declfn; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(ast::param_typ(params[i])));
i = i + 1;
}
}
  registry.fn_types.set(name, std::make_shared<registry::Type>(registry::TFn(param_types, type_from_annotation(return_type))));
  return registry;
 }(); },
  [&](const DeclType& decltype_) -> registry::TypeRegistry { auto [type_name, _w0, variants] = decltype_; return [&]() -> registry::TypeRegistry { 
  int i = 0;
  while (i < variants.size()){
{
registry = register_variant(registry, type_name, variants[i]);
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclImport& declimport) -> registry::TypeRegistry { auto [_w0, _w1] = declimport; return registry; },
  [&](const DeclExported& declexported) -> registry::TypeRegistry { auto [d] = declexported; return register_decl(registry, d); }
}, (*decl));
}

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<registry::Type> result_type = std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> registry::TypeRegistry { auto [variant_name] = varunit; return [&]() -> registry::TypeRegistry { 
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  return registry;
 }(); },
  [&](const VarTuple& vartuple) -> registry::TypeRegistry { auto [variant_name, field_types] = vartuple; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> field_type_list = {};
  int i = 0;
  while (i < field_types.size()){
{
field_type_list.push_back(type_from_annotation(field_types[i]));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, field_type_list);
  return registry;
 }(); },
  [&](const VarRecord& varrecord) -> registry::TypeRegistry { auto [variant_name, field_defs] = varrecord; return [&]() -> registry::TypeRegistry { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int i = 0;
  while (i < field_defs.size()){
{
field_map.set(field_defs[i]->name, type_from_annotation(field_defs[i]->typ));
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

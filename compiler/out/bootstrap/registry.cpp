#define main mlc_user_main
#include "registry.hpp"

namespace registry {

std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept{
return self.fn_types.get(name);
}
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept{
return self.fn_types.has(name);
}
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept{
return self.ctor_types.get(name);
}
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept{
return self.ctor_types.has(name);
}
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept{
if (self.ctor_params.has(name)) {
return self.ctor_params.get(name);
} else {
return {};
}
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept{
return self.field_types.get(type_name);
}
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept{
return self.field_types.has(type_name);
}
TypeRegistry empty_registry() noexcept{
return TypeRegistry{{}, {}, {}, {}};
}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return std::make_shared<Type>(TI32{}); },
[&](const ast::TyString& tyString) { return std::make_shared<Type>(TString{}); },
[&](const ast::TyBool& tyBool) { return std::make_shared<Type>(TBool{}); },
[&](const ast::TyUnit& tyUnit) { return std::make_shared<Type>(TUnit{}); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return std::make_shared<Type>(TNamed(name)); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::make_shared<Type>(TArray(type_from_annotation(inner))); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return std::make_shared<Type>(TShared(type_from_annotation(inner))); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(params[i]));
i = (i + 1);
}
return std::make_shared<Type>(TFn(param_types, type_from_annotation(ret)));
}(); },
[&](const auto& __v) { return std::make_shared<Type>(TUnknown{}); }
}, (*type_expr));
}
TypeRegistry build_registry(ast::Program prog) noexcept{
auto registry = empty_registry();
auto i = 0;
while ((i < prog.decls.length())) {
registry = register_decl(registry, prog.decls[i]);
i = (i + 1);
}
return registry;
}
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{[&](const ast::DeclTrait& declTrait) { auto [__0, __1, methods] = declTrait; return [&]() {
auto i = 0;
while ((i < methods.length())) {
registry = register_decl(registry, methods[i]);
i = (i + 1);
}
return registry;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return registry; },
[&](const ast::DeclFn& declFn) { auto [name, __1, __2, params, return_type, __5] = declFn; return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(ast::param_typ(params[i])));
i = (i + 1);
}
registry.fn_types.set(name, std::make_shared<Type>(TFn(param_types, type_from_annotation(return_type))));
return registry;
}(); },
[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return [&]() {
auto i = 0;
while ((i < variants.length())) {
registry = register_variant(registry, type_name, variants[i]);
i = (i + 1);
}
return registry;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return registry; },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return register_decl(registry, d); }
}, (*decl));
}
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
auto result_type = std::make_shared<Type>(TNamed(type_name));
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return [&]() {
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, {});
return registry;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, field_types] = varTuple; return [&]() {
auto field_type_list = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < field_types.length())) {
field_type_list.push_back(type_from_annotation(field_types[i]));
i = (i + 1);
}
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, field_type_list);
return registry;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_map = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
auto i = 0;
while ((i < field_defs.length())) {
field_map.set(field_defs[i]->name, type_from_annotation(field_defs[i]->typ));
i = (i + 1);
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

#define main mlc_user_main
#include "registry.hpp"

namespace registry {

std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept{
return self.fn_types.get(name);
}
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept{
return self.fn_types.has(name);
}
mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(TypeRegistry self, mlc::String function_name) noexcept{
if (self.function_type_parameter_names.has(function_name)) {
return self.function_type_parameter_names.get(function_name);
} else {
auto empty_function_type_names = mlc::Array<mlc::String>{};
return empty_function_type_names;
}
}
mlc::Array<mlc::String> TypeRegistry_parameter_names_for(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_parameter_names.has(fn_name)) {
return self.function_parameter_names.get(fn_name);
} else {
auto empty_param_names = mlc::Array<mlc::String>{};
return empty_param_names;
}
}
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept{
if (self.algebraic_decl_type_parameter_names.has(algebraic_type_name)) {
return self.algebraic_decl_type_parameter_names.get(algebraic_type_name);
} else {
auto empty_algebraic_type_names = mlc::Array<mlc::String>{};
return empty_algebraic_type_names;
}
}
mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(TypeRegistry self, mlc::String type_name) noexcept{
if (self.algebraic_decl_phantom_type_params.has(type_name)) {
return self.algebraic_decl_phantom_type_params.get(type_name);
} else {
auto empty_phantom = mlc::Array<mlc::String>{};
return empty_phantom;
}
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
bool TypeRegistry_type_implements_trait(TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept{
if (self.trait_impls.has(type_name)) {
auto index = 0;
/* unit */;
auto found = false;
auto impls = self.trait_impls.get(type_name);
while (((index < impls.length()) && (!found))) {
if ((impls[index] == trait_name)) {
found = true;
}
index = (index + 1);
}
return found;
} else {
return false;
}
}
mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_trait_bounds.has(fn_name)) {
return self.function_trait_bounds.get(fn_name);
} else {
auto empty_bounds = mlc::Array<mlc::Array<mlc::String>>{};
return empty_bounds;
}
}
int TypeRegistry_required_arity_for_fn(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_required_arity.has(fn_name)) {
return self.function_required_arity.get(fn_name);
} else {
return (-1);
}
}
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept{
auto i = 0;
auto found = false;
while ((i < self.private_ctors.length())) {
if ((self.private_ctors[i] == name)) {
found = true;
}
i = (i + 1);
}
return found;
}
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept{
if (self.trait_assoc_types.has(trait_name)) {
return self.trait_assoc_types.get(trait_name);
} else {
auto empty = mlc::Array<mlc::String>{};
return empty;
}
}
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept{
auto key = ((type_name + mlc::String("::", 2)) + trait_name);
if (self.assoc_type_bindings.has(key)) {
auto bindings = self.assoc_type_bindings.get(key);
if (bindings.has(assoc_name)) {
return bindings.get(assoc_name);
} else {
return std::make_shared<Type>(TUnknown{});
}
} else {
return std::make_shared<Type>(TUnknown{});
}
}
TypeRegistry empty_registry() noexcept{
auto ctor_types = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
auto ctor_params = mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>>();
ctor_types.set(mlc::String("Ok", 2), std::make_shared<Type>(TNamed{mlc::String("Result", 6)}));
ctor_types.set(mlc::String("Err", 3), std::make_shared<Type>(TNamed{mlc::String("Result", 6)}));
auto ok_params = mlc::Array<std::shared_ptr<Type>>{};
auto err_params = mlc::Array<std::shared_ptr<Type>>{};
ok_params.push_back(std::make_shared<Type>(TUnknown{}));
err_params.push_back(std::make_shared<Type>(TUnknown{}));
ctor_params.set(mlc::String("Ok", 2), ok_params);
ctor_params.set(mlc::String("Err", 3), err_params);
auto builtin_function_types = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
builtin_function_types.set(mlc::String("print", 5), std::make_shared<Type>(TFn{mlc::Array{std::make_shared<Type>(TString{})}, std::make_shared<Type>(TUnit{})}));
builtin_function_types.set(mlc::String("println", 7), std::make_shared<Type>(TFn{mlc::Array{std::make_shared<Type>(TString{})}, std::make_shared<Type>(TUnit{})}));
builtin_function_types.set(mlc::String("exit", 4), std::make_shared<Type>(TFn{mlc::Array{std::make_shared<Type>(TI32{})}, std::make_shared<Type>(TUnit{})}));
builtin_function_types.set(mlc::String("args", 4), std::make_shared<Type>(TFn{{}, std::make_shared<Type>(TArray{std::make_shared<Type>(TString{})})}));
auto fn_req = mlc::HashMap<mlc::String, int>();
fn_req.set(mlc::String("print", 5), 1);
fn_req.set(mlc::String("println", 7), 1);
fn_req.set(mlc::String("exit", 4), 1);
fn_req.set(mlc::String("args", 4), 0);
auto empty_private_ctors = mlc::Array<mlc::String>{};
return TypeRegistry{builtin_function_types, {}, {}, fn_req, {}, ctor_types, ctor_params, {}, {}, {}, {}, empty_private_ctors, {}, {}};
}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return std::make_shared<Type>(TI32{}); },
[&](const ast::TyString& tyString) { return std::make_shared<Type>(TString{}); },
[&](const ast::TyBool& tyBool) { return std::make_shared<Type>(TBool{}); },
[&](const ast::TyUnit& tyUnit) { return std::make_shared<Type>(TUnit{}); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return ((name == mlc::String("i64", 3)) ? std::make_shared<Type>(TI64{}) : ((name == mlc::String("f64", 3)) ? std::make_shared<Type>(TF64{}) : ((name == mlc::String("u8", 2)) ? std::make_shared<Type>(TU8{}) : ((name == mlc::String("usize", 5)) ? std::make_shared<Type>(TUsize{}) : ((name == mlc::String("char", 4)) ? std::make_shared<Type>(TChar{}) : std::make_shared<Type>(TNamed{name})))))); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::make_shared<Type>(TArray{type_from_annotation(inner)}); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return std::make_shared<Type>(TShared{type_from_annotation(inner)}); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, targs] = tyGeneric; return (((name == mlc::String("ref", 3)) && (targs.length() == 1)) ? type_from_annotation(targs[0]) : [&]() {
auto typed_args = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < targs.length())) {
typed_args.push_back(type_from_annotation(targs[i]));
i = (i + 1);
}
return std::make_shared<Type>(TGeneric{name, typed_args});
}()); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(params[i]));
i = (i + 1);
}
return std::make_shared<Type>(TFn{param_types, type_from_annotation(ret)});
}(); },
[&](const ast::TyAssoc& tyAssoc) { auto [param, assoc] = tyAssoc; return std::make_shared<Type>(TAssoc{param, assoc}); },
[&](const auto& __v) { return std::make_shared<Type>(TUnknown{}); }
}, (*type_expr));
}
int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto i = 0;
while ((i < params.length())) {
if (params[i]->has_default) {
return i;
}
i = (i + 1);
}
return params.length();
}
TypeRegistry build_registry(ast::Program program) noexcept{
auto expanded_program = trait_param_expand::expand_trait_as_param_program(program);
auto registry = empty_registry();
auto declaration_index = 0;
while ((declaration_index < expanded_program.decls.length())) {
registry = register_decl(registry, expanded_program.decls[declaration_index]);
declaration_index = (declaration_index + 1);
}
return registry;
}
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{[&](const ast::DeclTrait& declTrait) { auto [trait_name, __1, methods] = declTrait; return [&]() {
auto i = 0;
while ((i < methods.length())) {
auto m = methods[i];
std::visit(overloaded{[&](const ast::DeclAssocType& declAssocType) { auto [assoc_name, __1] = declAssocType; return [&]() {
auto existing = (registry.trait_assoc_types.has(trait_name) ? registry.trait_assoc_types.get(trait_name) : [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}());
existing.push_back(assoc_name);
registry.trait_assoc_types.set(trait_name, existing);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
registry = register_decl(registry, m);
/* unit */;
return /* unit */;
}(); }
}, (*m));
i = (i + 1);
}
return registry;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, trait_name, methods] = declExtend; return [&]() {
if ((trait_name.length() > 0)) {
auto existing = (registry.trait_impls.has(type_name) ? registry.trait_impls.get(type_name) : [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}());
existing.push_back(trait_name);
registry.trait_impls.set(type_name, existing);
}
auto i = 0;
while ((i < methods.length())) {
auto m = methods[i];
std::visit(overloaded{[&](const ast::DeclAssocBind& declAssocBind) { auto [assoc_name, type_expr, __2] = declAssocBind; return [&]() {
auto key = ((type_name + mlc::String("::", 2)) + trait_name);
auto empty_bindings = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
auto bindings = (registry.assoc_type_bindings.has(key) ? registry.assoc_type_bindings.get(key) : empty_bindings);
bindings.set(assoc_name, type_from_annotation(type_expr));
registry.assoc_type_bindings.set(key, bindings);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
registry = register_decl(registry, m);
/* unit */;
return /* unit */;
}(); }
}, (*m));
i = (i + 1);
}
return registry;
}(); },
[&](const ast::DeclFn& declFn) { auto [name, type_parameters, trait_bounds, params, return_type, __5] = declFn; return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto pnames = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(ast::param_typ(params[i])));
pnames.push_back(ast::param_name(params[i]));
i = (i + 1);
}
registry.fn_types.set(name, std::make_shared<Type>(TFn{param_types, type_from_annotation(return_type)}));
registry.function_type_parameter_names.set(name, type_parameters);
registry.function_parameter_names.set(name, pnames);
registry.function_required_arity.set(name, required_arity_from_params(params));
if ((trait_bounds.length() > 0)) {
registry.function_trait_bounds.set(name, trait_bounds);
}
return registry;
}(); },
[&](const ast::DeclType& declType) { auto [type_name, type_parameters, variants, __3] = declType; return [&]() {
registry.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
auto i = 0;
while ((i < variants.length())) {
registry = register_variant(registry, type_name, variants[i]);
i = (i + 1);
}
auto phantom = compute_phantom_type_params(type_parameters, variants);
registry.algebraic_decl_phantom_type_params.set(type_name, phantom);
return registry;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return registry; },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return register_decl(registry, d); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return registry; },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return registry; }
}, (*decl));
}
bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return (name == param); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return type_param_in_annotation(param, inner); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return type_param_in_annotation(param, inner); },
[&](const ast::TyGeneric& tyGeneric) { auto [__0, targs] = tyGeneric; return type_param_in_annotation_list(param, targs); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return (type_param_in_annotation_list(param, params) || type_param_in_annotation(param, ret)); },
[&](const ast::TyAssoc& tyAssoc) { auto [p, __1] = tyAssoc; return (p == param); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
auto found = false;
auto i = 0;
while ((i < types.length())) {
if ((!found)) {
found = type_param_in_annotation(param, types[i]);
}
i = (i + 1);
}
return found;
}
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [__0, fts, __2] = varTuple; return fts; },
[&](const ast::VarRecord& varRecord) { auto [__0, fds, __2] = varRecord; return [&]() {
auto result = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto i = 0;
while ((i < fds.length())) {
result.push_back(fds[i]->typ);
i = (i + 1);
}
return result;
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto r = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
return r;
}(); }
}, (*variant));
}
mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto all_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto vi = 0;
while ((vi < variants.length())) {
auto fts = variant_annotation_typeexprs(variants[vi]);
auto fi = 0;
while ((fi < fts.length())) {
all_fields.push_back(fts[fi]);
fi = (fi + 1);
}
vi = (vi + 1);
}
auto phantom = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < type_params.length())) {
if ((!type_param_in_annotation_list(type_params[i], all_fields))) {
phantom.push_back(type_params[i]);
}
i = (i + 1);
}
return phantom;
}
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
auto result_type = std::make_shared<Type>(TNamed{type_name});
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name, is_private] = varUnit; return [&]() {
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, {});
if (is_private) {
registry.private_ctors.push_back(variant_name);
}
return registry;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, field_types, is_private] = varTuple; return [&]() {
auto field_type_list = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < field_types.length())) {
field_type_list.push_back(type_from_annotation(field_types[i]));
i = (i + 1);
}
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, field_type_list);
if (is_private) {
registry.private_ctors.push_back(variant_name);
}
return registry;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs, is_private] = varRecord; return [&]() {
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
if (is_private) {
registry.private_ctors.push_back(variant_name);
}
return registry;
}(); }
}, (*variant));
}
std::shared_ptr<Type> field_type_from_object(std::shared_ptr<Type> object_type, mlc::String field_name, TypeRegistry registry) noexcept{
auto inner_type = std::visit(overloaded{[&](const TShared& tShared) { auto [inner] = tShared; return inner; },
[&](const auto& __v) { return object_type; }
}, (*object_type));
auto type_name = std::visit(overloaded{[&](const TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner_type));
if (((type_name != mlc::String("", 0)) && TypeRegistry_has_fields(registry, type_name))) {
auto field_map = TypeRegistry_fields_for(registry, type_name);
if (field_map.has(field_name)) {
return field_map.get(field_name);
} else {
return std::make_shared<Type>(TUnknown{});
}
} else {
return std::make_shared<Type>(TUnknown{});
}
}
std::shared_ptr<Type> method_return_type_from_object(std::shared_ptr<Type> object_type, mlc::String method_name, TypeRegistry registry) noexcept{
auto inner_type = std::visit(overloaded{[&](const TShared& tShared) { auto [inner] = tShared; return inner; },
[&](const auto& __v) { return object_type; }
}, (*object_type));
auto type_name = std::visit(overloaded{[&](const TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner_type));
if ((type_name == mlc::String("", 0))) {
return std::make_shared<Type>(TUnknown{});
} else {
auto mangled = ((type_name + mlc::String("_", 1)) + method_name);
if (TypeRegistry_has_fn(registry, mangled)) {
return std::visit(overloaded{[&](const TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const auto& __v) { return std::make_shared<Type>(TUnknown{}); }
}, (*TypeRegistry_fn_type(registry, mangled)));
} else {
return std::make_shared<Type>(TUnknown{});
}
}
}

} // namespace registry

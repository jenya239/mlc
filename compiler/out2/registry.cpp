#include "registry.hpp"

#include "ast.hpp"

namespace registry {

using namespace ast;
using namespace ast_tokens;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(registry::TypeRegistry self, mlc::String function_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_parameter_names_for(registry::TypeRegistry self, mlc::String fn_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept;

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept;

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

registry::TypeRegistry build_registry(ast::Program prog) noexcept;

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept;

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept;

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.fn_types.get(name);}

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept{return self.fn_types.has(name);}

mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(registry::TypeRegistry self, mlc::String function_name) noexcept{return self.function_type_parameter_names.has(function_name) ? self.function_type_parameter_names.get(function_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_function_type_names = {};
  return empty_function_type_names;
 }();}

mlc::Array<mlc::String> TypeRegistry_parameter_names_for(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_parameter_names.has(fn_name) ? self.function_parameter_names.get(fn_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_param_names = {};
  return empty_param_names;
 }();}

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept{return self.algebraic_decl_type_parameter_names.has(algebraic_type_name) ? self.algebraic_decl_type_parameter_names.get(algebraic_type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_algebraic_type_names = {};
  return empty_algebraic_type_names;
 }();}

mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.algebraic_decl_phantom_type_params.has(type_name) ? self.algebraic_decl_phantom_type_params.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_phantom = {};
  return empty_phantom;
 }();}

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.get(name);}

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.has(name);}

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept{return self.ctor_params.has(name) ? self.ctor_params.get(name) : mlc::Array<std::shared_ptr<registry::Type>>{};}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.get(type_name);}

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.has(type_name);}

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept{return self.trait_impls.has(type_name) ? [&]() -> bool { 
  int index = 0;
  bool found = false;
  mlc::Array<mlc::String> impls = self.trait_impls.get(type_name);
  while (index < impls.size() && !found){
{
if (impls[index] == trait_name){
{
found = true;
}
}
index = index + 1;
}
}
  return found;
 }() : false;}

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_trait_bounds.has(fn_name) ? self.function_trait_bounds.get(fn_name) : [&]() -> mlc::Array<mlc::Array<mlc::String>> { 
  mlc::Array<mlc::Array<mlc::String>> empty_bounds = {};
  return empty_bounds;
 }();}

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_required_arity.has(fn_name) ? self.function_required_arity.get(fn_name) : -1;}

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept{
int i = 0;
bool found = false;
while (i < self.private_ctors.size()){
{
if (self.private_ctors[i] == name){
{
found = true;
}
}
i = i + 1;
}
}
return found;
}

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
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> builtin_function_types = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
builtin_function_types.set(mlc::String("print"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("println"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("exit"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("args"), std::make_shared<registry::Type>(registry::TFn({}, std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TString{})))))));
mlc::HashMap<mlc::String, int> fn_req = mlc::HashMap<mlc::String, int>();
fn_req.set(mlc::String("print"), 1);
fn_req.set(mlc::String("println"), 1);
fn_req.set(mlc::String("exit"), 1);
fn_req.set(mlc::String("args"), 0);
mlc::Array<mlc::String> empty_private_ctors = {};
return registry::TypeRegistry{builtin_function_types, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), fn_req, mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>>(), ctor_types, ctor_params, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), empty_private_ctors};
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

int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
int i = 0;
while (i < params.size()){
{
if (params[i]->has_default){
{
return i;
}
}
i = i + 1;
}
}
return params.size();
}

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
  [&](const DeclExtend& declextend) -> registry::TypeRegistry { auto [type_name, trait_name, methods] = declextend; return [&]() -> registry::TypeRegistry { 
  if (trait_name.length() > 0){
{
mlc::Array<mlc::String> existing = registry.trait_impls.has(type_name) ? registry.trait_impls.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
existing.push_back(trait_name);
registry.trait_impls.set(type_name, existing);
}
}
  int i = 0;
  while (i < methods.size()){
{
registry = register_decl(registry, methods[i]);
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclFn& declfn) -> registry::TypeRegistry { auto [name, type_parameters, trait_bounds, params, return_type, _w0] = declfn; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = {};
  mlc::Array<mlc::String> pnames = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(ast::param_typ(params[i])));
pnames.push_back(ast::param_name(params[i]));
i = i + 1;
}
}
  registry.fn_types.set(name, std::make_shared<registry::Type>(registry::TFn(param_types, type_from_annotation(return_type))));
  registry.function_type_parameter_names.set(name, type_parameters);
  registry.function_parameter_names.set(name, pnames);
  registry.function_required_arity.set(name, required_arity_from_params(params));
  if (trait_bounds.size() > 0){
{
registry.function_trait_bounds.set(name, trait_bounds);
}
}
  return registry;
 }(); },
  [&](const DeclType& decltype_) -> registry::TypeRegistry { auto [type_name, type_parameters, variants, _w0] = decltype_; return [&]() -> registry::TypeRegistry { 
  registry.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
  int i = 0;
  while (i < variants.size()){
{
registry = register_variant(registry, type_name, variants[i]);
i = i + 1;
}
}
  mlc::Array<mlc::String> phantom = compute_phantom_type_params(type_parameters, variants);
  registry.algebraic_decl_phantom_type_params.set(type_name, phantom);
  return registry;
 }(); },
  [&](const DeclImport& declimport) -> registry::TypeRegistry { auto [_w0, _w1] = declimport; return registry; },
  [&](const DeclExported& declexported) -> registry::TypeRegistry { auto [d] = declexported; return register_decl(registry, d); }
}, (*decl));
}

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*t))) { auto _v_tynamed = std::get<ast::TyNamed>((*t)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [inner] = _v_tyshared; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [_w0, targs] = _v_tygeneric; return type_param_in_annotation_list(param, targs); } if (std::holds_alternative<ast::TyFn>((*t))) { auto _v_tyfn = std::get<ast::TyFn>((*t)); auto [params, ret] = _v_tyfn; return type_param_in_annotation_list(param, params) || type_param_in_annotation(param, ret); } return false; }();}

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
bool found = false;
int i = 0;
while (i < types.size()){
{
if (!found){
{
found = type_param_in_annotation(param, types[i]);
}
}
i = i + 1;
}
}
return found;
}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fts, _w1] = vartuple; return fts; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fds, _w1] = varrecord; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> result = {};
  int i = 0;
  while (i < fds.size()){
{
result.push_back(fds[i]->typ);
i = i + 1;
}
}
  return result;
 }(); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> r = {};
  return r;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> all_fields = {};
int vi = 0;
while (vi < variants.size()){
{
mlc::Array<std::shared_ptr<ast::TypeExpr>> fts = variant_annotation_typeexprs(variants[vi]);
int fi = 0;
while (fi < fts.size()){
{
all_fields.push_back(fts[fi]);
fi = fi + 1;
}
}
vi = vi + 1;
}
}
mlc::Array<mlc::String> phantom = {};
int i = 0;
while (i < type_params.size()){
{
if (!type_param_in_annotation_list(type_params[i], all_fields)){
{
phantom.push_back(type_params[i]);
}
}
i = i + 1;
}
}
return phantom;
}

registry::TypeRegistry register_variant(registry::TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<registry::Type> result_type = std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> registry::TypeRegistry { auto [variant_name, is_private] = varunit; return [&]() -> registry::TypeRegistry { 
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  if (is_private){
{
registry.private_ctors.push_back(variant_name);
}
}
  return registry;
 }(); },
  [&](const VarTuple& vartuple) -> registry::TypeRegistry { auto [variant_name, field_types, is_private] = vartuple; return [&]() -> registry::TypeRegistry { 
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
  if (is_private){
{
registry.private_ctors.push_back(variant_name);
}
}
  return registry;
 }(); },
  [&](const VarRecord& varrecord) -> registry::TypeRegistry { auto [variant_name, field_defs, is_private] = varrecord; return [&]() -> registry::TypeRegistry { 
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
  if (is_private){
{
registry.private_ctors.push_back(variant_name);
}
}
  return registry;
 }(); }
}, (*variant));
}

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*object_type))) { auto _v_tshared = std::get<registry::TShared>((*object_type)); auto [inner] = _v_tshared; return inner; } return object_type; }();
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && TypeRegistry_has_fields(registry, type_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*object_type))) { auto _v_tshared = std::get<registry::TShared>((*object_type)); auto [inner] = _v_tshared; return inner; } return object_type; }();
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name == mlc::String("") ? std::make_shared<registry::Type>((registry::TUnknown{})) : [&]() -> std::shared_ptr<registry::Type> { 
  mlc::String mangled = type_name + mlc::String("_") + method_name;
  return TypeRegistry_has_fn(registry, mangled) ? [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*TypeRegistry_fn_type(registry, mangled)))) { auto _v_tfn = std::get<registry::TFn>((*TypeRegistry_fn_type(registry, mangled))); auto [_w0, return_type] = _v_tfn; return return_type; } return std::make_shared<registry::Type>((registry::TUnknown{})); }() : std::make_shared<registry::Type>((registry::TUnknown{}));
 }();
}

} // namespace registry

#include "registry.hpp"

#include "ast.hpp"
#include "trait_param_expand.hpp"

namespace registry {

using namespace ast;
using namespace trait_param_expand;
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

mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(registry::TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept;

bool TypeRegistry_record_field_has_default_expression(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;

std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept;

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept;

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(registry::TypeRegistry self, mlc::String trait_name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_resolve_assoc(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

registry::TypeRegistry build_registry(ast::Program program) noexcept;

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept;

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept;

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

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

mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(registry::TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept{return self.record_field_names_ordered.has(algebraic_or_variant_name) ? self.record_field_names_ordered.get(algebraic_or_variant_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_names = {};
  return empty_names;
 }();}

bool TypeRegistry_record_field_has_default_expression(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{return self.record_literal_field_defaults.has(nominal_record_key) && self.record_literal_field_defaults.get(nominal_record_key).has(field_label);}

std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{return self.record_literal_field_defaults.get(nominal_record_key).get(field_label);}

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.has(type_name);}

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept{return self.trait_impls.has(type_name) && self.trait_impls.get(type_name).any([trait_name](mlc::String trait_implementation) mutable { return trait_implementation == trait_name; });}

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_trait_bounds.has(fn_name) ? self.function_trait_bounds.get(fn_name) : [&]() -> mlc::Array<mlc::Array<mlc::String>> { 
  mlc::Array<mlc::Array<mlc::String>> empty_bounds = {};
  return empty_bounds;
 }();}

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_required_arity.has(fn_name) ? self.function_required_arity.get(fn_name) : -1;}

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept{return self.private_ctors.any([name](mlc::String constructor_name) mutable { return constructor_name == name; });}

mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(registry::TypeRegistry self, mlc::String trait_name) noexcept{return self.trait_assoc_types.has(trait_name) ? self.trait_assoc_types.get(trait_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();}

std::shared_ptr<registry::Type> TypeRegistry_resolve_assoc(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept{
mlc::String key = type_name + mlc::String("::") + trait_name;
return self.assoc_type_bindings.has(key) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> bindings = self.assoc_type_bindings.get(key);
  return bindings.has(assoc_name) ? bindings.get(assoc_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
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
return registry::TypeRegistry{builtin_function_types, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), fn_req, mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>>(), ctor_types, ctor_params, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), empty_private_ctors, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>()};
}

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<ast::TyI32>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TI32{})); } if (std::holds_alternative<ast::TyString>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<ast::TyBool>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TBool{})); } if (std::holds_alternative<ast::TyUnit>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TUnit{})); } if (std::holds_alternative<ast::TyNamed>((*type_expr))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expr)); auto [name] = _v_tynamed; return name == mlc::String("i64") ? std::make_shared<registry::Type>((registry::TI64{})) : name == mlc::String("f64") ? std::make_shared<registry::Type>((registry::TF64{})) : name == mlc::String("u8") ? std::make_shared<registry::Type>((registry::TU8{})) : name == mlc::String("usize") ? std::make_shared<registry::Type>((registry::TUsize{})) : name == mlc::String("char") ? std::make_shared<registry::Type>((registry::TChar{})) : std::make_shared<registry::Type>(registry::TNamed(name)); } if (std::holds_alternative<ast::TyArray>((*type_expr))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyShared>((*type_expr))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyGeneric>((*type_expr))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expr)); auto [name, targs] = _v_tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? type_from_annotation(targs[0]) : std::make_shared<registry::Type>(registry::TGeneric(name, targs.map([](std::shared_ptr<ast::TypeExpr> argument) mutable { return type_from_annotation(argument); }))); } if (std::holds_alternative<ast::TyFn>((*type_expr))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expr)); auto [parameters, ret] = _v_tyfn; return std::make_shared<registry::Type>(registry::TFn(parameters.map([](std::shared_ptr<ast::TypeExpr> annotation) mutable { return type_from_annotation(annotation); }), type_from_annotation(ret))); } if (std::holds_alternative<ast::TyAssoc>((*type_expr))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expr)); auto [param, assoc] = _v_tyassoc; return std::make_shared<registry::Type>(registry::TAssoc(param, assoc)); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

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

registry::TypeRegistry build_registry(ast::Program program) noexcept{return trait_param_expand::expand_trait_as_param_program(program).decls.fold(empty_registry(), [](registry::TypeRegistry registry_so_far, std::shared_ptr<ast::Decl> declaration) mutable { return register_decl(registry_so_far, declaration); });}

registry::TypeRegistry register_decl(registry::TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{
  [&](const DeclTrait& decltrait) -> registry::TypeRegistry { auto [trait_name, _w0, methods] = decltrait; return [&]() -> registry::TypeRegistry { 
  int i = 0;
  while (i < methods.size()){
{
std::shared_ptr<ast::Decl> m = methods[i];
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclAssocType>((*m))) { auto _v_declassoctype = std::get<ast::DeclAssocType>((*m)); auto [assoc_name, _w0] = _v_declassoctype; return [&]() -> std::tuple<> { 
  mlc::Array<mlc::String> existing = registry.trait_assoc_types.has(trait_name) ? registry.trait_assoc_types.get(trait_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
  existing.push_back(assoc_name);
  registry.trait_assoc_types.set(trait_name, existing);
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  registry = register_decl(registry, m);
  return std::make_tuple();
 }(); }();
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
std::shared_ptr<ast::Decl> m = methods[i];
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclAssocBind>((*m))) { auto _v_declassocbind = std::get<ast::DeclAssocBind>((*m)); auto [assoc_name, type_expr, _w0] = _v_declassocbind; return [&]() -> std::tuple<> { 
  mlc::String key = type_name + mlc::String("::") + trait_name;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_bindings = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> bindings = registry.assoc_type_bindings.has(key) ? registry.assoc_type_bindings.get(key) : empty_bindings;
  bindings.set(assoc_name, type_from_annotation(type_expr));
  registry.assoc_type_bindings.set(key, bindings);
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  registry = register_decl(registry, m);
  return std::make_tuple();
 }(); }();
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclFn& declfn) -> registry::TypeRegistry { auto [name, type_parameters, trait_bounds, parameters, return_type, _w0, _w1] = declfn; return [&]() -> registry::TypeRegistry { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return type_from_annotation(ast::param_typ(parameter)); });
  mlc::Array<mlc::String> parameter_names = parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return ast::param_name(parameter); });
  registry.fn_types.set(name, std::make_shared<registry::Type>(registry::TFn(param_types, type_from_annotation(return_type))));
  registry.function_type_parameter_names.set(name, type_parameters);
  registry.function_parameter_names.set(name, parameter_names);
  registry.function_required_arity.set(name, required_arity_from_params(parameters));
  if (trait_bounds.size() > 0){
{
registry.function_trait_bounds.set(name, trait_bounds);
}
}
  return registry;
 }(); },
  [&](const DeclType& decltype_) -> registry::TypeRegistry { auto [type_name, type_parameters, variants, _w0] = decltype_; return [&]() -> registry::TypeRegistry { 
  registry.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
  registry = variants.fold(registry, [type_name](registry::TypeRegistry registry_so_far, std::shared_ptr<ast::TypeVariant> variant_definition) mutable { return register_variant(registry_so_far, type_name, variant_definition); });
  mlc::Array<mlc::String> phantom = compute_phantom_type_params(type_parameters, variants);
  registry.algebraic_decl_phantom_type_params.set(type_name, phantom);
  return registry;
 }(); },
  [&](const DeclImport& declimport) -> registry::TypeRegistry { auto [_w0, _w1] = declimport; return registry; },
  [&](const DeclExported& declexported) -> registry::TypeRegistry { auto [d] = declexported; return register_decl(registry, d); },
  [&](const DeclAssocType& declassoctype) -> registry::TypeRegistry { auto [_w0, _w1] = declassoctype; return registry; },
  [&](const DeclAssocBind& declassocbind) -> registry::TypeRegistry { auto [_w0, _w1, _w2] = declassocbind; return registry; }
}, (*decl));
}

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*t))) { auto _v_tynamed = std::get<ast::TyNamed>((*t)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [inner] = _v_tyshared; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [_w0, targs] = _v_tygeneric; return type_param_in_annotation_list(param, targs); } if (std::holds_alternative<ast::TyFn>((*t))) { auto _v_tyfn = std::get<ast::TyFn>((*t)); auto [params, ret] = _v_tyfn; return type_param_in_annotation_list(param, params) || type_param_in_annotation(param, ret); } if (std::holds_alternative<ast::TyAssoc>((*t))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*t)); auto [p, _w0] = _v_tyassoc; return p == param; } return false; }();}

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{return types.any([param](std::shared_ptr<ast::TypeExpr> type_expression) mutable { return type_param_in_annotation(param, type_expression); });}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fts, _w1] = vartuple; return fts; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fds, _w1] = varrecord; return fds.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->typ; }); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> empty_type_expressions = {};
  return empty_type_expressions;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto all_field_type_expressions = mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> variant_definition) mutable { return variant_annotation_typeexprs(variant_definition); });
return type_parameters.filter([all_field_type_expressions](mlc::String type_parameter) mutable { return !type_param_in_annotation_list(type_parameter, all_field_type_expressions); });
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
  mlc::Array<std::shared_ptr<registry::Type>> field_type_list = field_types.map([](std::shared_ptr<ast::TypeExpr> field_type_expression) mutable { return type_from_annotation(field_type_expression); });
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
  mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>> defaults_for_variant = mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>();
  mlc::Array<mlc::String> ordered_names = field_defs.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->name; });
  int index = 0;
  while (index < field_defs.size()){
{
std::shared_ptr<ast::FieldDef> field_definition = field_defs[index];
field_map.set(field_definition->name, type_from_annotation(field_definition->typ));
if (field_definition->has_default_expression){
{
defaults_for_variant.set(field_definition->name, field_definition->default_expression);
}
}
index = index + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  registry.field_types.set(variant_name, field_map);
  registry.field_types.set(type_name, field_map);
  registry.record_field_names_ordered.set(variant_name, ordered_names);
  registry.record_field_names_ordered.set(type_name, ordered_names);
  registry.record_literal_field_defaults.set(variant_name, defaults_for_variant);
  registry.record_literal_field_defaults.set(type_name, defaults_for_variant);
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
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [name, _w0] = _v_tgeneric; return name; } return mlc::String(""); }();
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

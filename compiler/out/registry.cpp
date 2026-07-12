#define main mlc_user_main
#include "registry.hpp"

#include "ast.hpp"
#include "trait_param_expand.hpp"

namespace registry {

using namespace ast;
using namespace trait_param_expand;
using namespace ast_tokens;

std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(TypeRegistry self, mlc::String function_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_parameter_names_for(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<int> TypeRegistry_parameter_mutability_flags_for(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(TypeRegistry self, mlc::String type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_variant_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept;
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept;
bool TypeRegistry_record_field_has_default_expression(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;
std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;
bool TypeRegistry_has_type_alias(TypeRegistry self, mlc::String alias_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_type_alias_type_parameter_names_for(TypeRegistry self, mlc::String alias_name) noexcept;
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_type_implements_trait(TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;
mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(TypeRegistry self, mlc::String fn_name) noexcept;
int TypeRegistry_required_arity_for_fn(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_function(TypeRegistry self, mlc::String function_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_type(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_function_is_blocking(TypeRegistry self, mlc::String function_name) noexcept;
bool TypeRegistry_type_is_thread_safe(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_type_is_thread_affine(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;
mlc::String TypeRegistry_defining_path_for_type(TypeRegistry self, mlc::String type_name) noexcept;
mlc::String TypeRegistry_defining_path_for_trait(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(TypeRegistry self, mlc::String function_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_parameter_names_for(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<int> TypeRegistry_parameter_mutability_flags_for(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(TypeRegistry self, mlc::String type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_variant_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept;
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept;
bool TypeRegistry_record_field_has_default_expression(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;
std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;
bool TypeRegistry_has_type_alias(TypeRegistry self, mlc::String alias_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_type_alias_type_parameter_names_for(TypeRegistry self, mlc::String alias_name) noexcept;
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_type_implements_trait(TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;
mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(TypeRegistry self, mlc::String fn_name) noexcept;
int TypeRegistry_required_arity_for_fn(TypeRegistry self, mlc::String fn_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_function(TypeRegistry self, mlc::String function_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_type(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_function_is_blocking(TypeRegistry self, mlc::String function_name) noexcept;
bool TypeRegistry_type_is_thread_safe(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_type_is_thread_affine(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;
mlc::String TypeRegistry_defining_path_for_type(TypeRegistry self, mlc::String type_name) noexcept;
mlc::String TypeRegistry_defining_path_for_trait(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept{
return self.function_index.function_types.get(name);
}
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept{
return self.function_index.function_types.has(name);
}
mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(TypeRegistry self, mlc::String function_name) noexcept{
if (self.function_index.function_type_parameter_names.has(function_name)) {
  return self.function_index.function_type_parameter_names.get(function_name);
} else {
  auto empty_function_type_names = mlc::Array<mlc::String>{};
  return empty_function_type_names;
}
}
mlc::Array<mlc::String> TypeRegistry_parameter_names_for(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_index.function_parameter_names.has(fn_name)) {
  return self.function_index.function_parameter_names.get(fn_name);
} else {
  auto empty_param_names = mlc::Array<mlc::String>{};
  return empty_param_names;
}
}
mlc::Array<int> TypeRegistry_parameter_mutability_flags_for(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_index.function_parameter_mutability_flags.has(fn_name)) {
  return self.function_index.function_parameter_mutability_flags.get(fn_name);
} else {
  auto empty_mutability_pattern = mlc::Array<int>{};
  return empty_mutability_pattern;
}
}
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept{
if (self.adt_index.algebraic_decl_type_parameter_names.has(algebraic_type_name)) {
  return self.adt_index.algebraic_decl_type_parameter_names.get(algebraic_type_name);
} else {
  auto empty_algebraic_type_names = mlc::Array<mlc::String>{};
  return empty_algebraic_type_names;
}
}
mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(TypeRegistry self, mlc::String type_name) noexcept{
if (self.adt_index.algebraic_decl_phantom_type_params.has(type_name)) {
  return self.adt_index.algebraic_decl_phantom_type_params.get(type_name);
} else {
  auto empty_phantom = mlc::Array<mlc::String>{};
  return empty_phantom;
}
}
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_variant_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept{
if (self.adt_index.algebraic_decl_variant_names.has(algebraic_type_name)) {
  return self.adt_index.algebraic_decl_variant_names.get(algebraic_type_name);
} else {
  auto empty_variant_names = mlc::Array<mlc::String>{};
  return empty_variant_names;
}
}
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept{
return self.adt_index.constructor_types.get(name);
}
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept{
return self.adt_index.constructor_types.has(name);
}
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept{
if (self.adt_index.constructor_parameters.has(name)) {
  return self.adt_index.constructor_parameters.get(name);
} else {
  return mlc::Array<std::shared_ptr<Type>>{};
}
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept{
return self.record_index.field_types.get(type_name);
}
mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept{
if (self.record_index.record_field_names_ordered.has(algebraic_or_variant_name)) {
  return self.record_index.record_field_names_ordered.get(algebraic_or_variant_name);
} else {
  auto empty_names = mlc::Array<mlc::String>{};
  return empty_names;
}
}
bool TypeRegistry_record_field_has_default_expression(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{
return (self.record_index.record_literal_field_defaults.has(nominal_record_key) && self.record_index.record_literal_field_defaults.get(nominal_record_key).has(field_label));
}
std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{
return self.record_index.record_literal_field_defaults.get(nominal_record_key).get(field_label);
}
bool TypeRegistry_has_type_alias(TypeRegistry self, mlc::String alias_name) noexcept{
return self.type_alias_annotations.has(alias_name);
}
mlc::Array<mlc::String> TypeRegistry_type_alias_type_parameter_names_for(TypeRegistry self, mlc::String alias_name) noexcept{
if (self.type_alias_type_parameter_names.has(alias_name)) {
  return self.type_alias_type_parameter_names.get(alias_name);
} else {
  auto empty = mlc::Array<mlc::String>{};
  return empty;
}
}
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept{
return self.record_index.field_types.has(resolve_nominal_type_name_for_fields(self, type_name, mlc::Array<mlc::String>{}));
}
bool TypeRegistry_type_implements_trait(TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept{
return (self.adt_index.trait_impls.has(type_name) && self.adt_index.trait_impls.get(type_name).any([=](auto trait_implementation) mutable { return (trait_implementation == trait_name); }));
}
mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_index.function_trait_bounds.has(fn_name)) {
  return self.function_index.function_trait_bounds.get(fn_name);
} else {
  auto empty_bounds = mlc::Array<mlc::Array<mlc::String>>{};
  return empty_bounds;
}
}
int TypeRegistry_required_arity_for_fn(TypeRegistry self, mlc::String fn_name) noexcept{
if (self.function_index.function_required_arity.has(fn_name)) {
  return self.function_index.function_required_arity.get(fn_name);
} else {
  return (-1);
}
}
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_function(TypeRegistry self, mlc::String function_name) noexcept{
if (self.function_index.function_concurrency_attributes.has(function_name)) {
  return self.function_index.function_concurrency_attributes.get(function_name);
} else {
  auto empty_attributes = mlc::Array<mlc::String>{};
  return empty_attributes;
}
}
mlc::Array<mlc::String> TypeRegistry_concurrency_attributes_for_type(TypeRegistry self, mlc::String type_name) noexcept{
if (self.function_index.type_concurrency_attributes.has(type_name)) {
  return self.function_index.type_concurrency_attributes.get(type_name);
} else {
  auto empty_attributes = mlc::Array<mlc::String>{};
  return empty_attributes;
}
}
bool TypeRegistry_function_is_blocking(TypeRegistry self, mlc::String function_name) noexcept{
return TypeRegistry_concurrency_attributes_for_function(self, function_name).any([=](mlc::String attribute) mutable { return (attribute == mlc::String("blocking", 8)); });
}
bool TypeRegistry_type_is_thread_safe(TypeRegistry self, mlc::String type_name) noexcept{
return TypeRegistry_concurrency_attributes_for_type(self, type_name).any([=](mlc::String attribute) mutable { return (attribute == mlc::String("thread_safe", 11)); });
}
bool TypeRegistry_type_is_thread_affine(TypeRegistry self, mlc::String type_name) noexcept{
return TypeRegistry_concurrency_attributes_for_type(self, type_name).any([=](mlc::String attribute) mutable { return ((attribute == mlc::String("thread_affine", 13)) || ((attribute.length() >= 14) && (attribute.substring(0, 14) == mlc::String("thread_affine(", 14)))); });
}
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept{
return self.adt_index.private_constructors.has(name);
}
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept{
if (self.adt_index.trait_assoc_types.has(trait_name)) {
  return self.adt_index.trait_assoc_types.get(trait_name);
} else {
  auto empty = mlc::Array<mlc::String>{};
  return empty;
}
}
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept{
auto key = ((type_name + mlc::String("::", 2)) + trait_name);
if (self.adt_index.assoc_type_bindings.has(key)) {
  auto bindings = self.adt_index.assoc_type_bindings.get(key);
  if (bindings.has(assoc_name))   {
    return bindings.get(assoc_name);
  } else   {
    return std::make_shared<Type>(TUnknown{});
  }
} else {
  return std::make_shared<Type>(TUnknown{});
}
}
mlc::String TypeRegistry_defining_path_for_type(TypeRegistry self, mlc::String type_name) noexcept{
if (self.type_defining_path.has(type_name)) {
  return self.type_defining_path.get(type_name);
} else {
  return mlc::String("", 0);
}
}
mlc::String TypeRegistry_defining_path_for_trait(TypeRegistry self, mlc::String trait_name) noexcept{
if (self.trait_defining_path.has(trait_name)) {
  return self.trait_defining_path.get(trait_name);
} else {
  return mlc::String("", 0);
}
}
TypeRegistry empty_registry() noexcept{
  auto constructor_types_map = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  auto constructor_parameters_map = mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>>();
  constructor_types_map.set(mlc::String("Ok", 2), std::make_shared<Type>(TNamed{mlc::String("Result", 6)}));
  constructor_types_map.set(mlc::String("Err", 3), std::make_shared<Type>(TNamed{mlc::String("Result", 6)}));
  auto ok_variant_parameter_types = mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})};
  auto error_variant_parameter_types = mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})};
  constructor_parameters_map.set(mlc::String("Ok", 2), ok_variant_parameter_types);
  constructor_parameters_map.set(mlc::String("Err", 3), error_variant_parameter_types);
  auto builtin_function_types = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  builtin_function_types.set(mlc::String("print", 5), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TString{})}, std::make_shared<Type>(TUnit{})}));
  builtin_function_types.set(mlc::String("println", 7), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TString{})}, std::make_shared<Type>(TUnit{})}));
  builtin_function_types.set(mlc::String("eprintln", 8), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TString{})}, std::make_shared<Type>(TUnit{})}));
  builtin_function_types.set(mlc::String("exit", 4), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TI32{})}, std::make_shared<Type>(TUnit{})}));
  builtin_function_types.set(mlc::String("args", 4), std::make_shared<Type>(TFn{{}, std::make_shared<Type>(TArray{std::make_shared<Type>(TString{})})}));
  builtin_function_types.set(mlc::String("read_line", 9), std::make_shared<Type>(TFn{{}, std::make_shared<Type>(TString{})}));
  builtin_function_types.set(mlc::String("read_all", 8), std::make_shared<Type>(TFn{{}, std::make_shared<Type>(TString{})}));
  auto builtin_required_arity = mlc::HashMap<mlc::String, int>();
  builtin_required_arity.set(mlc::String("print", 5), 1);
  builtin_required_arity.set(mlc::String("println", 7), 1);
  builtin_required_arity.set(mlc::String("eprintln", 8), 1);
  builtin_required_arity.set(mlc::String("exit", 4), 1);
  builtin_required_arity.set(mlc::String("args", 4), 0);
  builtin_function_types.set(mlc::String("spawn_task", 10), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})}, std::make_shared<Type>(TGeneric{mlc::String("Task", 4), mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})}})}));
  builtin_required_arity.set(mlc::String("spawn_task", 10), 1);
  builtin_function_types.set(mlc::String("__task_scope_new", 16), std::make_shared<Type>(TFn{{}, std::make_shared<Type>(TNamed{mlc::String("TaskScope", 9)})}));
  builtin_required_arity.set(mlc::String("__task_scope_new", 16), 0);
  builtin_function_types.set(mlc::String("block_on", 8), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TGeneric{mlc::String("Task", 4), mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})}})}, std::make_shared<Type>(TUnknown{})}));
  builtin_required_arity.set(mlc::String("block_on", 8), 1);
  builtin_function_types.set(mlc::String("is_ready", 8), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TGeneric{mlc::String("Task", 4), mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})}})}, std::make_shared<Type>(TBool{})}));
  builtin_required_arity.set(mlc::String("is_ready", 8), 1);
  builtin_function_types.set(mlc::String("make_channel", 12), std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUsize{})}, std::make_shared<Type>(TGeneric{mlc::String("Channel", 7), mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TUnknown{})}})}));
  builtin_required_arity.set(mlc::String("make_channel", 12), 1);
  builtin_required_arity.set(mlc::String("read_line", 9), 0);
  builtin_required_arity.set(mlc::String("read_all", 8), 0);
  auto empty_private_constructors = mlc::HashMap<mlc::String, bool>();
  return TypeRegistry{FunctionIndex{builtin_function_types, {}, {}, {}, builtin_required_arity, {}, {}, {}}, AdtIndex{constructor_types_map, constructor_parameters_map, {}, {}, {}, {}, empty_private_constructors, {}, {}}, RecordIndex{{}, {}, {}}, {}, {}, {}, {}};
}
void record_type_defining_path(TypeRegistry& registry, mlc::String type_name, mlc::String defining_path) noexcept{
  if ((defining_path.length() > 0))   {
    registry.type_defining_path.set(type_name, defining_path);
  }
}
void record_trait_defining_path(TypeRegistry& registry, mlc::String trait_name, mlc::String defining_path) noexcept{
  if ((defining_path.length() > 0))   {
    registry.trait_defining_path.set(trait_name, defining_path);
  }
}
std::shared_ptr<Type> primitive_type_from_name(mlc::String name) noexcept{
  if ((name == mlc::String("i64", 3)))   {
    return std::make_shared<Type>(TI64{});
  } else if ((name == mlc::String("f64", 3)))   {
    return std::make_shared<Type>(TF64{});
  } else if ((name == mlc::String("u8", 2)))   {
    return std::make_shared<Type>(TU8{});
  } else if ((name == mlc::String("usize", 5)))   {
    return std::make_shared<Type>(TUsize{});
  } else if ((name == mlc::String("char", 4)))   {
    return std::make_shared<Type>(TChar{});
  } else if ((name == mlc::String("i32", 3)))   {
    return std::make_shared<Type>(TI32{});
  } else if ((name == mlc::String("string", 6)))   {
    return std::make_shared<Type>(TString{});
  } else if ((name == mlc::String("bool", 4)))   {
    return std::make_shared<Type>(TBool{});
  } else if ((name == mlc::String("unit", 4)))   {
    return std::make_shared<Type>(TUnit{});
  } else   {
    return std::make_shared<Type>(TNamed{name});
  }
}
std::shared_ptr<ast::TypeExpr> type_expression_substitute_for_named(mlc::String name, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept{
  if ((name == parameter_name))   {
    return argument_expression;
  } else   {
    return type_expression;
  }
}
std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept{
  return std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::make_shared<ast::TypeExpr>(ast::TyArray{type_expression_substitute_named_parameter(inner, parameter_name, argument_expression)}); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return std::make_shared<ast::TypeExpr>(ast::TyShared{type_expression_substitute_named_parameter(inner, parameter_name, argument_expression)}); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return std::make_shared<ast::TypeExpr>(ast::TyGeneric{name, type_arguments.map([=](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_expression_substitute_named_parameter(type_argument, parameter_name, argument_expression); })}); },
[&](const ast::TyFn& tyFn) { auto [parameters, return_type] = tyFn; return std::make_shared<ast::TypeExpr>(ast::TyFn{parameters.map([=](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_expression_substitute_named_parameter(parameter_type, parameter_name, argument_expression); }), type_expression_substitute_named_parameter(return_type, parameter_name, argument_expression)}); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return type_expression_substitute_for_named(name, type_expression, parameter_name, argument_expression); },
[&](const ast::TyI32& tyI32) { return type_expression; },
[&](const ast::TyString& tyString) { return type_expression; },
[&](const ast::TyBool& tyBool) { return type_expression; },
[&](const ast::TyUnit& tyUnit) { return type_expression; },
[&](const ast::TyAssoc& tyAssoc) { auto [__0, __1] = tyAssoc; return type_expression; }
}, (*type_expression));
}
std::shared_ptr<ast::TypeExpr> substitute_type_expression_type_parameters(std::shared_ptr<ast::TypeExpr> type_expression, mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_expressions) noexcept{
  auto result = type_expression;
  auto index = 0;
  while ((index < parameter_names.length()))   {
    if ((index < argument_expressions.length()))     {
      (result = type_expression_substitute_named_parameter(result, parameter_names[index], argument_expressions[index]));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_parameter_substitution_from_type_arguments(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<Type>> type_arguments) noexcept{
  auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  auto index = 0;
  while (((index < parameter_names.length()) && (index < type_arguments.length())))   {
    substitution.set(parameter_names[index], type_arguments[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return substitution;
}
bool resolution_stack_contains_name(mlc::Array<mlc::String> resolution_stack, mlc::String candidate_name) noexcept{
  return resolution_stack.any([=](mlc::String stack_name) mutable { return (stack_name == candidate_name); });
}
mlc::String type_alias_named_target(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [name] = tyNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String resolve_nominal_type_name_for_fields(TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept{
  if (registry.type_alias_annotations.has(type_name))   {
    if (resolution_stack_contains_name(resolution_stack, type_name))     {
      return mlc::String("", 0);
    } else     {
      auto extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{type_name});
      auto alias_target_name = type_alias_named_target(registry.type_alias_annotations.get(type_name));
      if ((alias_target_name == mlc::String("", 0)))       {
        return mlc::String("", 0);
      } else       {
        return resolve_nominal_type_name_for_fields(registry, alias_target_name, extended_stack);
      }
    }
  } else   {
    return type_name;
  }
}
std::shared_ptr<Type> resolve_named_type_with_registry(mlc::String name, TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept{
  if ((((((((((name == mlc::String("i32", 3)) || (name == mlc::String("string", 6))) || (name == mlc::String("bool", 4))) || (name == mlc::String("unit", 4))) || (name == mlc::String("i64", 3))) || (name == mlc::String("f64", 3))) || (name == mlc::String("u8", 2))) || (name == mlc::String("usize", 5))) || (name == mlc::String("char", 4))))   {
    return primitive_type_from_name(name);
  } else if (registry.type_alias_annotations.has(name))   {
    if (resolution_stack_contains_name(resolution_stack, name))     {
      return std::make_shared<Type>(TUnknown{});
    } else     {
      auto extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{name});
      return type_from_annotation_with_registry_and_stack(registry.type_alias_annotations.get(name), registry, extended_stack);
    }
  } else   {
    return std::make_shared<Type>(TNamed{name});
  }
}
std::shared_ptr<Type> type_from_annotation_generic_with_registry_and_stack(mlc::String generic_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments, TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept{
  if (((generic_name == mlc::String("ref", 3)) && (type_arguments.length() == 1)))   {
    return type_from_annotation_with_registry_and_stack(type_arguments[0], type_registry, resolution_stack);
  } else if ((type_registry.type_alias_annotations.has(generic_name) && type_registry.type_alias_type_parameter_names.has(generic_name)))   {
    if (resolution_stack_contains_name(resolution_stack, generic_name))     {
      return std::make_shared<Type>(TUnknown{});
    } else     {
      auto extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{generic_name});
      auto substituted_expression = substitute_type_expression_type_parameters(type_registry.type_alias_annotations.get(generic_name), type_registry.type_alias_type_parameter_names.get(generic_name), type_arguments);
      return type_from_annotation_with_registry_and_stack(substituted_expression, type_registry, extended_stack);
    }
  } else   {
    return std::make_shared<Type>(TGeneric{generic_name, type_arguments.map([=](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation_with_registry_and_stack(type_argument, type_registry, resolution_stack); })});
  }
}
std::shared_ptr<Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept{
  return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return std::make_shared<Type>(TI32{}); },
[&](const ast::TyString& tyString) { return std::make_shared<Type>(TString{}); },
[&](const ast::TyBool& tyBool) { return std::make_shared<Type>(TBool{}); },
[&](const ast::TyUnit& tyUnit) { return std::make_shared<Type>(TUnit{}); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::make_shared<Type>(TArray{type_from_annotation_with_registry_and_stack(inner, type_registry, resolution_stack)}); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return std::make_shared<Type>(TShared{type_from_annotation_with_registry_and_stack(inner, type_registry, resolution_stack)}); },
[&](const ast::TyFn& tyFn) { auto [parameters, return_type] = tyFn; return std::make_shared<Type>(TFn{parameters.map([=](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_from_annotation_with_registry_and_stack(parameter_type, type_registry, resolution_stack); }), type_from_annotation_with_registry_and_stack(return_type, type_registry, resolution_stack)}); },
[&](const ast::TyAssoc& tyAssoc) { auto [parameter_name, associated_name] = tyAssoc; return std::make_shared<Type>(TAssoc{parameter_name, associated_name}); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return resolve_named_type_with_registry(name, type_registry, resolution_stack); },
[&](const ast::TyGeneric& tyGeneric) { auto [generic_name, type_arguments] = tyGeneric; return type_from_annotation_generic_with_registry_and_stack(generic_name, type_arguments, type_registry, resolution_stack); }
}, (*type_expression));
}
std::shared_ptr<Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, TypeRegistry registry) noexcept{
  return type_from_annotation_with_registry_and_stack(type_expression, registry, mlc::Array<mlc::String>{});
}
bool type_alias_has_cycle_from_name(TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept{
  if ((!registry.type_alias_annotations.has(current_name)))   {
    return false;
  } else if (resolution_stack_contains_name(resolution_stack, current_name))   {
    return true;
  } else   {
    auto extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{current_name});
    auto target_name = type_alias_named_target(registry.type_alias_annotations.get(current_name));
    if ((target_name == mlc::String("", 0)))     {
      return false;
    } else     {
      return type_alias_has_cycle_from_name(registry, target_name, extended_stack);
    }
  }
}
bool type_alias_has_cycle(TypeRegistry registry, mlc::String alias_name) noexcept{
  return type_alias_has_cycle_from_name(registry, alias_name, mlc::Array<mlc::String>{});
}
std::shared_ptr<Type> type_from_named_annotation(mlc::String name) noexcept{
  if ((name == mlc::String("i64", 3)))   {
    return std::make_shared<Type>(TI64{});
  } else if ((name == mlc::String("f64", 3)))   {
    return std::make_shared<Type>(TF64{});
  } else if ((name == mlc::String("u8", 2)))   {
    return std::make_shared<Type>(TU8{});
  } else if ((name == mlc::String("usize", 5)))   {
    return std::make_shared<Type>(TUsize{});
  } else if ((name == mlc::String("char", 4)))   {
    return std::make_shared<Type>(TChar{});
  } else   {
    return std::make_shared<Type>(TNamed{name});
  }
}
std::shared_ptr<Type> type_from_generic_annotation(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept{
  if (((name == mlc::String("ref", 3)) && (type_arguments.length() == 1)))   {
    return type_from_annotation(type_arguments[0]);
  } else   {
    return std::make_shared<Type>(TGeneric{name, type_arguments.map([=](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation(type_argument); })});
  }
}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
  return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return std::make_shared<Type>(TI32{}); },
[&](const ast::TyString& tyString) { return std::make_shared<Type>(TString{}); },
[&](const ast::TyBool& tyBool) { return std::make_shared<Type>(TBool{}); },
[&](const ast::TyUnit& tyUnit) { return std::make_shared<Type>(TUnit{}); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::make_shared<Type>(TArray{type_from_annotation(inner)}); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return std::make_shared<Type>(TShared{type_from_annotation(inner)}); },
[&](const ast::TyFn& tyFn) { auto [parameters, return_type] = tyFn; return std::make_shared<Type>(TFn{parameters.map([=](std::shared_ptr<ast::TypeExpr> annotation) mutable { return type_from_annotation(annotation); }), type_from_annotation(return_type)}); },
[&](const ast::TyAssoc& tyAssoc) { auto [param, assoc] = tyAssoc; return std::make_shared<Type>(TAssoc{param, assoc}); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return type_from_named_annotation(name); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return type_from_generic_annotation(name, type_arguments); }
}, (*type_expr));
}
int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto i = 0;
  while ((i < params.length()))   {
    if (params[i]->has_default)     {
      return i;
    }
    (i = mlc::arith::checked_add(i, 1));
  }
  return params.length();
}
TypeRegistry build_registry(ast::Program program) noexcept{
  auto expanded = trait_param_expand::expand_trait_as_param_program(program);
  auto registry = empty_registry();
  auto index = 0;
  while ((index < expanded.decls.length()))   {
    register_decl_into(registry, expanded.decls[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return registry;
}
mlc::Array<mlc::String> accumulate_trait_associated_type_name(mlc::Array<mlc::String> accumulated_names, std::shared_ptr<ast::Decl> method) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = method;
if (std::holds_alternative<ast::DeclAssocType>((*__match_subject))) {
const ast::DeclAssocType& declAssocType = std::get<ast::DeclAssocType>((*__match_subject));
auto [assoc_name, __1] = declAssocType; return accumulated_names.concat(mlc::Array<mlc::String>{assoc_name});
}
return accumulated_names;
std::abort();
}();
}
mlc::Array<mlc::String> trait_associated_type_names_for(TypeRegistry registry, mlc::String trait_name) noexcept{
  if (registry.adt_index.trait_assoc_types.has(trait_name))   {
    return registry.adt_index.trait_assoc_types.get(trait_name);
  } else   {
    auto empty = mlc::Array<mlc::String>{};
    return empty;
  }
}
bool trait_method_is_assoc_type_declaration(std::shared_ptr<ast::Decl> method) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<ast::DeclAssocType>((*__match_subject))) {
const ast::DeclAssocType& declAssocType = std::get<ast::DeclAssocType>((*__match_subject));
auto [__0, __1] = declAssocType; return true;
}
return false;
std::abort();
}();
}
void register_trait_method_into_registry(TypeRegistry& registry, std::shared_ptr<ast::Decl> method) noexcept{
  if ((!trait_method_is_assoc_type_declaration(method)))   {
    register_decl_into(registry, method);
  }
}
void register_decl_trait_into_registry(TypeRegistry& registry, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::String defining_path) noexcept{
  auto index = 0;
  while ((index < methods.length()))   {
    register_trait_method_into_registry(registry, methods[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  auto initial_trait_associated_type_names = trait_associated_type_names_for(registry, trait_name);
  auto trait_associated_type_names = methods.fold(initial_trait_associated_type_names, accumulate_trait_associated_type_name);
  if (true)   {
    registry.adt_index.trait_assoc_types.set(trait_name, trait_associated_type_names);
  }
  return record_trait_defining_path(registry, trait_name, defining_path);
}
mlc::String trait_base_name(mlc::String trait_name) noexcept{
  auto index = 0;
  while ((index < trait_name.length()))   {
    if ((trait_name.char_at(index) == mlc::String("<", 1)))     {
      return trait_name.substring(0, index);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return trait_name;
}
void register_decl_extend_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
  auto bare_trait_name = trait_base_name(trait_name);
  if ((trait_name.length() > 0))   {
    auto trait_implementations = [&]() -> auto {
  if (registry.adt_index.trait_impls.has(type_name))   {
    return registry.adt_index.trait_impls.get(type_name);
  } else   {
    return [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}();
  }
}();
    (trait_implementations = trait_implementations.concat(mlc::Array<mlc::String>{trait_name}));
    if ((bare_trait_name != trait_name))     {
      (trait_implementations = trait_implementations.concat(mlc::Array<mlc::String>{bare_trait_name}));
    }
    registry.adt_index.trait_impls.set(type_name, trait_implementations);
  }
  auto assoc_binding_key = ((type_name + mlc::String("::", 2)) + bare_trait_name);
  auto assoc_type_bindings = [&]() -> auto {
  if (registry.adt_index.assoc_type_bindings.has(assoc_binding_key))   {
    return registry.adt_index.assoc_type_bindings.get(assoc_binding_key);
  } else   {
    return [&]() {
auto empty_bindings = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
return empty_bindings;
}();
  }
}();
  auto assoc_bindings_dirty = false;
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    if (register_extend_method_into_registry(registry, methods[method_index], assoc_type_bindings))     {
      (assoc_bindings_dirty = true);
    }
    (method_index = mlc::arith::checked_add(method_index, 1));
  }
  if (assoc_bindings_dirty)   {
    registry.adt_index.assoc_type_bindings.set(assoc_binding_key, assoc_type_bindings);
  }
}
void register_decl_fn_into_registry(TypeRegistry& registry, mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> trait_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type, mlc::Array<mlc::String> concurrency_attributes) noexcept{
  auto param_types = parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return type_from_annotation_with_registry(ast::param_type_value(parameter), registry); });
  auto parameter_names = parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return ast::param_name(parameter); });
  auto parameter_mutability_flags = parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return (ast::param_is_mut(parameter) ? (1) : (0)); });
  registry.function_index.function_types.set(name, std::make_shared<Type>(TFn{param_types, type_from_annotation_with_registry(return_type, registry)}));
  registry.function_index.function_type_parameter_names.set(name, type_parameters);
  registry.function_index.function_parameter_names.set(name, parameter_names);
  registry.function_index.function_parameter_mutability_flags.set(name, parameter_mutability_flags);
  registry.function_index.function_required_arity.set(name, required_arity_from_params(parameters));
  if ((trait_bounds.length() > 0))   {
    registry.function_index.function_trait_bounds.set(name, trait_bounds);
  }
  if ((concurrency_attributes.length() > 0))   {
    registry.function_index.function_concurrency_attributes.set(name, concurrency_attributes);
  }
}
mlc::String variant_name_of(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name, __1] = varUnit; return variant_name; },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1, __2] = varTuple; return variant_name; },
[&](const ast::VarRecord& varRecord) { auto [variant_name, __1, __2] = varRecord; return variant_name; }
}, (*variant));
}
void register_decl_type_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String defining_path) noexcept{
  registry.adt_index.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
  auto variant_names = mlc::Array<mlc::String>{};
  auto variant_index = 0;
  while ((variant_index < variants.length()))   {
    register_variant_into(registry, type_name, variants[variant_index]);
    (variant_names = variant_names.concat(mlc::Array<mlc::String>{variant_name_of(variants[variant_index])}));
    (variant_index = mlc::arith::checked_add(variant_index, 1));
  }
  registry.adt_index.algebraic_decl_variant_names.set(type_name, variant_names);
  auto phantom = compute_phantom_type_params(type_parameters, variants);
  if (true)   {
    registry.adt_index.algebraic_decl_phantom_type_params.set(type_name, phantom);
  }
  return record_type_defining_path(registry, type_name, defining_path);
}
void register_decl_type_alias_into_registry(TypeRegistry& registry, mlc::String alias_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String defining_path) noexcept{
  registry.type_alias_annotations.set(alias_name, type_expression);
  if ((type_parameters.length() > 0))   {
    registry.type_alias_type_parameter_names.set(alias_name, type_parameters);
  }
  return record_type_defining_path(registry, alias_name, defining_path);
}
bool register_extend_method_into_registry(TypeRegistry& registry, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, std::shared_ptr<Type>>& assoc_type_bindings) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<ast::DeclAssocBind>((*__match_subject))) {
const ast::DeclAssocBind& declAssocBind = std::get<ast::DeclAssocBind>((*__match_subject));
auto [assoc_name, type_expr, __2] = declAssocBind; return [&]() {
assoc_type_bindings.set(assoc_name, type_from_annotation_with_registry(type_expr, registry));
return true;
}();
}
return [&]() {
register_decl_into(registry, method);
return false;
}();
std::abort();
}();
}
bool register_decl_trait_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, methods, name_span] = declTrait; return [&]() {
register_decl_trait_into_registry(registry, trait_name, methods, name_span.file);
return true;
}();
}
return false;
std::abort();
}();
}
bool register_decl_extend_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [type_name, trait_name, methods, __3] = declExtend; return [&]() {
register_decl_extend_into_registry(registry, type_name, trait_name, methods);
return true;
}();
}
return false;
std::abort();
}();
}
bool register_decl_fn_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, type_parameters, trait_bounds, parameters, return_type, body, __6] = declFn; return [&]() {
auto concurrency_attributes = [&]() -> mlc::Array<mlc::String> {
auto __match_subject = body;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [__0, __1, attributes, __3] = exprExtern; return attributes;
}
return [&]() {
auto empty_attributes = mlc::Array<mlc::String>{};
return empty_attributes;
}();
std::abort();
}();
register_decl_fn_into_registry(registry, name, type_parameters, trait_bounds, parameters, return_type, concurrency_attributes);
return true;
}();
}
return false;
std::abort();
}();
}
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  return haystack.any([=](mlc::String item) mutable { return (item == needle); });
}
void register_derive_json_functions(TypeRegistry& registry, mlc::String type_name) noexcept{
  auto to_json_name = (type_name + mlc::String("_to_json", 8));
  auto from_json_name = (type_name + mlc::String("_from_json", 10));
  auto empty_type_parameters = mlc::Array<mlc::String>{};
  registry.function_index.function_types.set(to_json_name, std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TNamed{type_name})}, std::make_shared<Type>(TNamed{mlc::String("JsonValue", 9)})}));
  registry.function_index.function_type_parameter_names.set(to_json_name, empty_type_parameters);
  registry.function_index.function_parameter_names.set(to_json_name, mlc::Array<mlc::String>{mlc::String("self", 4)});
  registry.function_index.function_parameter_mutability_flags.set(to_json_name, mlc::Array<int>{0});
  registry.function_index.function_required_arity.set(to_json_name, 1);
  registry.function_index.function_types.set(from_json_name, std::make_shared<Type>(TFn{mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TNamed{mlc::String("JsonValue", 9)})}, std::make_shared<Type>(TGeneric{mlc::String("Result", 6), mlc::Array<std::shared_ptr<Type>>{std::make_shared<Type>(TNamed{type_name}), std::make_shared<Type>(TNamed{mlc::String("JsonError", 9)})}})}));
  registry.function_index.function_type_parameter_names.set(from_json_name, empty_type_parameters);
  registry.function_index.function_parameter_names.set(from_json_name, mlc::Array<mlc::String>{mlc::String("value", 5)});
  registry.function_index.function_parameter_mutability_flags.set(from_json_name, mlc::Array<int>{0});
  return registry.function_index.function_required_arity.set(from_json_name, 1);
}
bool register_decl_type_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, type_parameters, variants, derive_traits, name_span] = declType; return [&]() {
register_decl_type_into_registry(registry, type_name, type_parameters, variants, name_span.file);
if (string_list_contains(derive_traits, mlc::String("Json", 4))) {
  register_derive_json_functions(registry, type_name);
}
return true;
}();
}
return false;
std::abort();
}();
}
bool register_decl_type_alias_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [alias_name, type_parameters, type_expression, name_span] = declTypeAlias; return [&]() {
register_decl_type_alias_into_registry(registry, alias_name, type_parameters, type_expression, name_span.file);
return true;
}();
}
return false;
std::abort();
}();
}
bool register_decl_exported_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [exported_declaration] = declExported; return [&]() {
register_decl_into(registry, exported_declaration);
return true;
}();
}
return false;
std::abort();
}();
}
void registry_decl_noop() noexcept{

}
void register_decl_extern_type_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::String defining_path, mlc::Array<mlc::String> concurrency_attributes) noexcept{
  registry.adt_index.algebraic_decl_type_parameter_names.set(type_name, {});
  registry.adt_index.algebraic_decl_variant_names.set(type_name, {});
  registry.adt_index.algebraic_decl_phantom_type_params.set(type_name, {});
  record_type_defining_path(registry, type_name, defining_path);
  if ((concurrency_attributes.length() > 0))   {
    registry.function_index.type_concurrency_attributes.set(type_name, concurrency_attributes);
  }
}
bool register_decl_extern_type_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExternType>((*__match_subject))) {
const ast::DeclExternType& declExternType = std::get<ast::DeclExternType>((*__match_subject));
auto [type_name, __1, __2, __3, concurrency_attributes, name_span] = declExternType; return [&]() {
register_decl_extern_type_into_registry(registry, type_name, name_span.file, concurrency_attributes);
return true;
}();
}
return false;
std::abort();
}();
}
void register_decl_into(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto handled = ((((((register_decl_trait_if(registry, declaration) || register_decl_extend_if(registry, declaration)) || register_decl_fn_if(registry, declaration)) || register_decl_type_if(registry, declaration)) || register_decl_type_alias_if(registry, declaration)) || register_decl_extern_type_if(registry, declaration)) || register_decl_exported_if(registry, declaration));
}
bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [name] = tyNamed; return (name == param);
}
if (std::holds_alternative<ast::TyArray>((*__match_subject))) {
const ast::TyArray& tyArray = std::get<ast::TyArray>((*__match_subject));
auto [inner] = tyArray; return type_param_in_annotation(param, inner);
}
if (std::holds_alternative<ast::TyShared>((*__match_subject))) {
const ast::TyShared& tyShared = std::get<ast::TyShared>((*__match_subject));
auto [inner] = tyShared; return type_param_in_annotation(param, inner);
}
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [__0, type_arguments] = tyGeneric; return type_param_in_annotation_list(param, type_arguments);
}
if (std::holds_alternative<ast::TyFn>((*__match_subject))) {
const ast::TyFn& tyFn = std::get<ast::TyFn>((*__match_subject));
auto [params, return_type] = tyFn; return (type_param_in_annotation_list(param, params) || type_param_in_annotation(param, return_type));
}
if (std::holds_alternative<ast::TyAssoc>((*__match_subject))) {
const ast::TyAssoc& tyAssoc = std::get<ast::TyAssoc>((*__match_subject));
auto [parameter_name, __1] = tyAssoc; return (parameter_name == param);
}
return false;
std::abort();
}();
}
bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
  return types.any([=](std::shared_ptr<ast::TypeExpr> type_expression) mutable { return type_param_in_annotation(param, type_expression); });
}
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [__0, field_types, __2] = varTuple; return field_types; },
[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions, __2] = varRecord; return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto empty_type_expressions = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
return empty_type_expressions;
}(); }
}, (*variant));
}
mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto all_field_type_expressions = variants.flat_map([=](std::shared_ptr<ast::TypeVariant> variant_definition) mutable { return variant_annotation_typeexprs(variant_definition); });
  return type_parameters.filter([=](mlc::String type_parameter) mutable { return (!type_param_in_annotation_list(type_parameter, all_field_type_expressions)); });
}
void register_variant_unit_into_registry(TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<Type> result_type, bool is_private) noexcept{
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, {});
  if (is_private)   {
    registry.adt_index.private_constructors.set(variant_name, true);
  }
}
void register_variant_tuple_into_registry(TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<Type> result_type, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, bool is_private) noexcept{
  auto field_type_list = field_types.map([=](std::shared_ptr<ast::TypeExpr> field_type_expression) mutable { return type_from_annotation_with_registry(field_type_expression, registry); });
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, field_type_list);
  if (is_private)   {
    registry.adt_index.private_constructors.set(variant_name, true);
  }
}
void register_variant_record_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::String variant_name, std::shared_ptr<Type> result_type, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, bool is_private) noexcept{
  auto field_map = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  auto defaults_for_variant = mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>();
  auto ordered_names = field_defs.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->name; });
  auto index = 0;
  while ((index < field_defs.length()))   {
    auto field_definition = field_defs[index];
    field_map.set(field_definition->name, type_from_annotation_with_registry(field_definition->type_value, registry));
    if (field_definition->has_default_expression)     {
      defaults_for_variant.set(field_definition->name, field_definition->default_expression);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, {});
  registry.record_index.field_types.set(variant_name, field_map);
  registry.record_index.field_types.set(type_name, field_map);
  registry.record_index.record_field_names_ordered.set(variant_name, ordered_names);
  registry.record_index.record_field_names_ordered.set(type_name, ordered_names);
  registry.record_index.record_literal_field_defaults.set(variant_name, defaults_for_variant);
  registry.record_index.record_literal_field_defaults.set(type_name, defaults_for_variant);
  if (is_private)   {
    registry.adt_index.private_constructors.set(variant_name, true);
  }
}
void register_variant_into(TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto result_type = std::make_shared<Type>(TNamed{type_name});
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) -> void { auto [variant_name, is_private] = varUnit; register_variant_unit_into_registry(registry, variant_name, result_type, is_private); },
[&](const ast::VarTuple& varTuple) -> void { auto [variant_name, field_types, is_private] = varTuple; register_variant_tuple_into_registry(registry, variant_name, result_type, field_types, is_private); },
[&](const ast::VarRecord& varRecord) -> void { auto [variant_name, field_defs, is_private] = varRecord; register_variant_record_into_registry(registry, type_name, variant_name, result_type, field_defs, is_private); }
}, (*variant));
}
std::shared_ptr<Type> substitute_semantic_type_for_named(mlc::String name, std::shared_ptr<Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<Type>> substitution) noexcept{
  if (substitution.has(name))   {
    return substitute_semantic_type(substitution.get(name), substitution);
  } else   {
    return type_value;
  }
}
std::shared_ptr<Type> substitute_semantic_type(std::shared_ptr<Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<Type>> substitution) noexcept{
  return std::visit(overloaded{[&](const TArray& tArray) { auto [inner] = tArray; return std::make_shared<Type>(TArray{substitute_semantic_type(inner, substitution)}); },
[&](const TShared& tShared) { auto [inner] = tShared; return std::make_shared<Type>(TShared{substitute_semantic_type(inner, substitution)}); },
[&](const TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return std::make_shared<Type>(TGeneric{name, type_arguments.map([=](std::shared_ptr<Type> argument) mutable { return substitute_semantic_type(argument, substitution); })}); },
[&](const TPair& tPair) { auto [a, b] = tPair; return std::make_shared<Type>(TPair{substitute_semantic_type(a, substitution), substitute_semantic_type(b, substitution)}); },
[&](const TTuple& tTuple) { auto [parameter_types] = tTuple; return std::make_shared<Type>(TTuple{parameter_types.map([=](std::shared_ptr<Type> entry) mutable { return substitute_semantic_type(entry, substitution); })}); },
[&](const TFn& tFn) { auto [parameter_list, return_type] = tFn; return std::make_shared<Type>(TFn{parameter_list.map([=](std::shared_ptr<Type> parameter) mutable { return substitute_semantic_type(parameter, substitution); }), substitute_semantic_type(return_type, substitution)}); },
[&](const TNamed& tNamed) { auto [name] = tNamed; return substitute_semantic_type_for_named(name, type_value, substitution); },
[&](const TI32& tI32) { return type_value; },
[&](const TString& tString) { return type_value; },
[&](const TBool& tBool) { return type_value; },
[&](const TUnit& tUnit) { return type_value; },
[&](const TI64& tI64) { return type_value; },
[&](const TF64& tF64) { return type_value; },
[&](const TU8& tU8) { return type_value; },
[&](const TUsize& tUsize) { return type_value; },
[&](const TChar& tChar) { return type_value; },
[&](const TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return type_value; },
[&](const TUnknown& tUnknown) { return type_value; }
}, (*type_value));
}
std::shared_ptr<Type> unwrap_shared_semantic_type(std::shared_ptr<Type> object_type) noexcept{
  return [&]() -> std::shared_ptr<Type> {
auto __match_subject = object_type;
if (std::holds_alternative<TShared>((*__match_subject))) {
const TShared& tShared = std::get<TShared>((*__match_subject));
auto [inner] = tShared; return inner;
}
return object_type;
std::abort();
}();
}
mlc::String field_lookup_type_name_from_semantic_type(std::shared_ptr<Type> inner_type, TypeRegistry registry) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = inner_type;
if (std::holds_alternative<TNamed>((*__match_subject))) {
const TNamed& tNamed = std::get<TNamed>((*__match_subject));
auto [name] = tNamed; return resolve_nominal_type_name_for_fields(registry, name, mlc::Array<mlc::String>{});
}
if (std::holds_alternative<TGeneric>((*__match_subject))) {
const TGeneric& tGeneric = std::get<TGeneric>((*__match_subject));
auto [name, __1] = tGeneric; return resolve_nominal_type_name_for_fields(registry, name, mlc::Array<mlc::String>{});
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<std::shared_ptr<Type>> generic_type_arguments_from_semantic_type(std::shared_ptr<Type> inner_type) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<Type>> {
auto __match_subject = inner_type;
if (std::holds_alternative<TGeneric>((*__match_subject))) {
const TGeneric& tGeneric = std::get<TGeneric>((*__match_subject));
auto [__0, type_argument_list] = tGeneric; return type_argument_list;
}
return [&]() {
auto empty_type_arguments = mlc::Array<std::shared_ptr<Type>>{};
return empty_type_arguments;
}();
std::abort();
}();
}
mlc::String method_owner_type_name_from_semantic_type(std::shared_ptr<Type> inner_type) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = inner_type;
if (std::holds_alternative<TNamed>((*__match_subject))) {
const TNamed& tNamed = std::get<TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
std::shared_ptr<Type> field_type_from_object(std::shared_ptr<Type> object_type, mlc::String field_name, TypeRegistry registry) noexcept{
  auto inner_type = unwrap_shared_semantic_type(object_type);
  auto type_name = field_lookup_type_name_from_semantic_type(inner_type, registry);
  auto type_arguments = generic_type_arguments_from_semantic_type(inner_type);
  if (((type_name == mlc::String("", 0)) || (!registry.record_index.field_types.has(type_name))))   {
    return std::make_shared<Type>(TUnknown{});
  }
  auto field_map = TypeRegistry_fields_for(registry, type_name);
  if ((!field_map.has(field_name)))   {
    return std::make_shared<Type>(TUnknown{});
  }
  auto raw_field_type = field_map.get(field_name);
  if ((type_arguments.length() > 0))   {
    auto parameter_names = TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
    return substitute_semantic_type(raw_field_type, type_parameter_substitution_from_type_arguments(parameter_names, type_arguments));
  }
  return raw_field_type;
}
std::shared_ptr<Type> method_return_type_from_object(std::shared_ptr<Type> object_type, mlc::String method_name, TypeRegistry registry) noexcept{
  auto inner_type = unwrap_shared_semantic_type(object_type);
  auto type_name = method_owner_type_name_from_semantic_type(inner_type);
  if ((type_name == mlc::String("", 0)))   {
    return std::make_shared<Type>(TUnknown{});
  }
  auto mangled = ((type_name + mlc::String("_", 1)) + method_name);
  if ((!TypeRegistry_has_fn(registry, mangled)))   {
    return std::make_shared<Type>(TUnknown{});
  }
  return [&]() -> std::shared_ptr<Type> {
auto __match_subject = TypeRegistry_fn_type(registry, mangled);
if (std::holds_alternative<TFn>((*__match_subject))) {
const TFn& tFn = std::get<TFn>((*__match_subject));
auto [__0, return_type] = tFn; return return_type;
}
return std::make_shared<Type>(TUnknown{});
std::abort();
}();
}

} // namespace registry

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

mlc::Array<int> TypeRegistry_parameter_mutability_flags_for(registry::TypeRegistry self, mlc::String fn_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(registry::TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept;

bool TypeRegistry_record_field_has_default_expression(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;

std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept;

bool TypeRegistry_has_type_alias(registry::TypeRegistry self, mlc::String alias_name) noexcept;

mlc::Array<mlc::String> TypeRegistry_type_alias_type_parameter_names_for(registry::TypeRegistry self, mlc::String alias_name) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept;

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept;

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(registry::TypeRegistry self, mlc::String trait_name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_resolve_assoc(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> primitive_type_from_name(mlc::String name) noexcept;

std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept;

std::shared_ptr<ast::TypeExpr> substitute_type_expression_type_parameters(std::shared_ptr<ast::TypeExpr> type_expression, mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_expressions) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_parameter_substitution_from_type_arguments(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;

bool resolution_stack_contains_name(mlc::Array<mlc::String> resolution_stack, mlc::String candidate_name) noexcept;

mlc::String type_alias_named_target(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::String resolve_nominal_type_name_for_fields(registry::TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> resolve_named_type_with_registry(mlc::String name, registry::TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry) noexcept;

bool type_alias_has_cycle_from_name(registry::TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept;

bool type_alias_has_cycle(registry::TypeRegistry registry, mlc::String alias_name) noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

registry::TypeRegistry build_registry(ast::Program program) noexcept;

mlc::Array<mlc::String> accumulate_trait_associated_type_name(mlc::Array<mlc::String> accumulated_names, std::shared_ptr<ast::Decl> method) noexcept;

void register_decl_into(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

void register_variant_into(registry::TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<registry::Type> substitute_semantic_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.function_index.function_types.get(name);}

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept{return self.function_index.function_types.has(name);}

mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(registry::TypeRegistry self, mlc::String function_name) noexcept{return self.function_index.function_type_parameter_names.has(function_name) ? self.function_index.function_type_parameter_names.get(function_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_function_type_names = {};
  return empty_function_type_names;
 }();}

mlc::Array<mlc::String> TypeRegistry_parameter_names_for(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_index.function_parameter_names.has(fn_name) ? self.function_index.function_parameter_names.get(fn_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_param_names = {};
  return empty_param_names;
 }();}

mlc::Array<int> TypeRegistry_parameter_mutability_flags_for(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_index.function_parameter_mutability_flags.has(fn_name) ? self.function_index.function_parameter_mutability_flags.get(fn_name) : [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }();}

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept{return self.adt_index.algebraic_decl_type_parameter_names.has(algebraic_type_name) ? self.adt_index.algebraic_decl_type_parameter_names.get(algebraic_type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_algebraic_type_names = {};
  return empty_algebraic_type_names;
 }();}

mlc::Array<mlc::String> TypeRegistry_phantom_type_params_for(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.adt_index.algebraic_decl_phantom_type_params.has(type_name) ? self.adt_index.algebraic_decl_phantom_type_params.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_phantom = {};
  return empty_phantom;
 }();}

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept{return self.adt_index.constructor_types.get(name);}

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept{return self.adt_index.constructor_types.has(name);}

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept{return self.adt_index.constructor_parameters.has(name) ? self.adt_index.constructor_parameters.get(name) : mlc::Array<std::shared_ptr<registry::Type>>{};}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.record_index.field_types.get(type_name);}

mlc::Array<mlc::String> TypeRegistry_record_field_names_ordered_for(registry::TypeRegistry self, mlc::String algebraic_or_variant_name) noexcept{return self.record_index.record_field_names_ordered.has(algebraic_or_variant_name) ? self.record_index.record_field_names_ordered.get(algebraic_or_variant_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_names = {};
  return empty_names;
 }();}

bool TypeRegistry_record_field_has_default_expression(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{return self.record_index.record_literal_field_defaults.has(nominal_record_key) && self.record_index.record_literal_field_defaults.get(nominal_record_key).has(field_label);}

std::shared_ptr<ast::Expr> TypeRegistry_record_field_default_expression_ast(registry::TypeRegistry self, mlc::String nominal_record_key, mlc::String field_label) noexcept{return self.record_index.record_literal_field_defaults.get(nominal_record_key).get(field_label);}

bool TypeRegistry_has_type_alias(registry::TypeRegistry self, mlc::String alias_name) noexcept{return self.type_alias_annotations.has(alias_name);}

mlc::Array<mlc::String> TypeRegistry_type_alias_type_parameter_names_for(registry::TypeRegistry self, mlc::String alias_name) noexcept{return self.type_alias_type_parameter_names.has(alias_name) ? self.type_alias_type_parameter_names.get(alias_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();}

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept{return self.record_index.field_types.has(resolve_nominal_type_name_for_fields(self, type_name, {}));}

bool TypeRegistry_type_implements_trait(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept{return self.adt_index.trait_impls.has(type_name) && self.adt_index.trait_impls.get(type_name).any([trait_name](mlc::String trait_implementation) mutable { return trait_implementation == trait_name; });}

mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_index.function_trait_bounds.has(fn_name) ? self.function_index.function_trait_bounds.get(fn_name) : [&]() -> mlc::Array<mlc::Array<mlc::String>> { 
  mlc::Array<mlc::Array<mlc::String>> empty_bounds = {};
  return empty_bounds;
 }();}

int TypeRegistry_required_arity_for_fn(registry::TypeRegistry self, mlc::String fn_name) noexcept{return self.function_index.function_required_arity.has(fn_name) ? self.function_index.function_required_arity.get(fn_name) : -1;}

bool TypeRegistry_is_private_ctor(registry::TypeRegistry self, mlc::String name) noexcept{return self.adt_index.private_constructors.has(name);}

mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(registry::TypeRegistry self, mlc::String trait_name) noexcept{return self.adt_index.trait_assoc_types.has(trait_name) ? self.adt_index.trait_assoc_types.get(trait_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();}

std::shared_ptr<registry::Type> TypeRegistry_resolve_assoc(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept{
mlc::String key = type_name + mlc::String("::") + trait_name;
return self.adt_index.assoc_type_bindings.has(key) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> bindings = self.adt_index.assoc_type_bindings.get(key);
  return bindings.has(assoc_name) ? bindings.get(assoc_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

registry::TypeRegistry empty_registry() noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> constructor_types_map = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> constructor_parameters_map = mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>>();
constructor_types_map.set(mlc::String("Ok"), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Result"))));
constructor_types_map.set(mlc::String("Err"), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Result"))));
mlc::Array<std::shared_ptr<registry::Type>> ok_variant_parameter_types = mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUnknown{}))};
mlc::Array<std::shared_ptr<registry::Type>> error_variant_parameter_types = mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUnknown{}))};
constructor_parameters_map.set(mlc::String("Ok"), ok_variant_parameter_types);
constructor_parameters_map.set(mlc::String("Err"), error_variant_parameter_types);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> builtin_function_types = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
builtin_function_types.set(mlc::String("print"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("println"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("exit"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("args"), std::make_shared<registry::Type>(registry::TFn({}, std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TString{})))))));
builtin_function_types.set(mlc::String("read_line"), std::make_shared<registry::Type>(registry::TFn({}, std::make_shared<registry::Type>((registry::TString{})))));
mlc::HashMap<mlc::String, int> builtin_required_arity = mlc::HashMap<mlc::String, int>();
builtin_required_arity.set(mlc::String("print"), 1);
builtin_required_arity.set(mlc::String("println"), 1);
builtin_required_arity.set(mlc::String("exit"), 1);
builtin_required_arity.set(mlc::String("args"), 0);
builtin_required_arity.set(mlc::String("read_line"), 0);
mlc::HashMap<mlc::String, bool> empty_private_constructors = mlc::HashMap<mlc::String, bool>();
return registry::TypeRegistry{registry::FunctionIndex{builtin_function_types, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<int>>(), builtin_required_arity, mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>>()}, registry::AdtIndex{constructor_types_map, constructor_parameters_map, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), empty_private_constructors, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>()}, registry::RecordIndex{mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>>()}, mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};
}

std::shared_ptr<registry::Type> primitive_type_from_name(mlc::String name) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (name == mlc::String("i64")) { return std::make_shared<registry::Type>((registry::TI64{})); } if (name == mlc::String("f64")) { return std::make_shared<registry::Type>((registry::TF64{})); } if (name == mlc::String("u8")) { return std::make_shared<registry::Type>((registry::TU8{})); } if (name == mlc::String("usize")) { return std::make_shared<registry::Type>((registry::TUsize{})); } if (name == mlc::String("char")) { return std::make_shared<registry::Type>((registry::TChar{})); } if (name == mlc::String("i32")) { return std::make_shared<registry::Type>((registry::TI32{})); } if (name == mlc::String("string")) { return std::make_shared<registry::Type>((registry::TString{})); } if (name == mlc::String("bool")) { return std::make_shared<registry::Type>((registry::TBool{})); } if (name == mlc::String("unit")) { return std::make_shared<registry::Type>((registry::TUnit{})); } return std::make_shared<registry::Type>(registry::TNamed(name)); }();}

std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept{return [&]() -> std::shared_ptr<ast::TypeExpr> { if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name == parameter_name ? argument_expression : type_expression; } if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return std::make_shared<ast::TypeExpr>(ast::TyArray(type_expression_substitute_named_parameter(inner, parameter_name, argument_expression))); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [inner] = _v_tyshared; return std::make_shared<ast::TypeExpr>(ast::TyShared(type_expression_substitute_named_parameter(inner, parameter_name, argument_expression))); } if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [name, type_arguments] = _v_tygeneric; return std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, type_arguments.map([parameter_name, argument_expression](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_expression_substitute_named_parameter(type_argument, parameter_name, argument_expression); }))); } if (std::holds_alternative<ast::TyFn>((*type_expression))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expression)); auto [parameters, return_type] = _v_tyfn; return std::make_shared<ast::TypeExpr>(ast::TyFn(parameters.map([parameter_name, argument_expression](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_expression_substitute_named_parameter(parameter_type, parameter_name, argument_expression); }), type_expression_substitute_named_parameter(return_type, parameter_name, argument_expression))); } return type_expression; }();}

std::shared_ptr<ast::TypeExpr> substitute_type_expression_type_parameters(std::shared_ptr<ast::TypeExpr> type_expression, mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_expressions) noexcept{
std::shared_ptr<ast::TypeExpr> result = type_expression;
int index = 0;
while (index < parameter_names.size()){
{
if (index < argument_expressions.size()){
{
result = type_expression_substitute_named_parameter(result, parameter_names[index], argument_expressions[index]);
}
}
index = index + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_parameter_substitution_from_type_arguments(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
int index = 0;
while (index < parameter_names.size() && index < type_arguments.size()){
{
substitution.set(parameter_names[index], type_arguments[index]);
index = index + 1;
}
}
return substitution;
}

bool resolution_stack_contains_name(mlc::Array<mlc::String> resolution_stack, mlc::String candidate_name) noexcept{return resolution_stack.any([candidate_name](mlc::String stack_name) mutable { return stack_name == candidate_name; });}

mlc::String type_alias_named_target(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name; } return mlc::String(""); }();}

mlc::String resolve_nominal_type_name_for_fields(registry::TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept{return registry.type_alias_annotations.has(type_name) ? resolution_stack_contains_name(resolution_stack, type_name) ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{type_name});
  mlc::String alias_target_name = type_alias_named_target(registry.type_alias_annotations.get(type_name));
  return alias_target_name == mlc::String("") ? mlc::String("") : resolve_nominal_type_name_for_fields(registry, alias_target_name, extended_stack);
 }() : type_name;}

std::shared_ptr<registry::Type> resolve_named_type_with_registry(mlc::String name, registry::TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept{return name == mlc::String("i32") || name == mlc::String("string") || name == mlc::String("bool") || name == mlc::String("unit") || name == mlc::String("i64") || name == mlc::String("f64") || name == mlc::String("u8") || name == mlc::String("usize") || name == mlc::String("char") ? primitive_type_from_name(name) : registry.type_alias_annotations.has(name) ? resolution_stack_contains_name(resolution_stack, name) ? std::make_shared<registry::Type>((registry::TUnknown{})) : [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{name});
  return type_from_annotation_with_registry_and_stack(registry.type_alias_annotations.get(name), registry, extended_stack);
 }() : std::make_shared<registry::Type>(registry::TNamed(name));}

std::shared_ptr<registry::Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<ast::TyI32>((*type_expression))) {  return std::make_shared<registry::Type>((registry::TI32{})); } if (std::holds_alternative<ast::TyString>((*type_expression))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<ast::TyBool>((*type_expression))) {  return std::make_shared<registry::Type>((registry::TBool{})); } if (std::holds_alternative<ast::TyUnit>((*type_expression))) {  return std::make_shared<registry::Type>((registry::TUnit{})); } if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return resolve_named_type_with_registry(name, registry, resolution_stack); } if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation_with_registry_and_stack(inner, registry, resolution_stack))); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [inner] = _v_tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation_with_registry_and_stack(inner, registry, resolution_stack))); } if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [generic_name, type_arguments] = _v_tygeneric; return generic_name == mlc::String("ref") && type_arguments.size() == 1 ? type_from_annotation_with_registry_and_stack(type_arguments[0], registry, resolution_stack) : registry.type_alias_annotations.has(generic_name) && registry.type_alias_type_parameter_names.has(generic_name) ? resolution_stack_contains_name(resolution_stack, generic_name) ? std::make_shared<registry::Type>((registry::TUnknown{})) : [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{generic_name});
  std::shared_ptr<ast::TypeExpr> substituted_expression = substitute_type_expression_type_parameters(registry.type_alias_annotations.get(generic_name), registry.type_alias_type_parameter_names.get(generic_name), type_arguments);
  return type_from_annotation_with_registry_and_stack(substituted_expression, registry, extended_stack);
 }() : std::make_shared<registry::Type>(registry::TGeneric(generic_name, type_arguments.map([registry, resolution_stack](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation_with_registry_and_stack(type_argument, registry, resolution_stack); }))); } if (std::holds_alternative<ast::TyFn>((*type_expression))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expression)); auto [parameters, return_type] = _v_tyfn; return std::make_shared<registry::Type>(registry::TFn(parameters.map([registry, resolution_stack](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_from_annotation_with_registry_and_stack(parameter_type, registry, resolution_stack); }), type_from_annotation_with_registry_and_stack(return_type, registry, resolution_stack))); } if (std::holds_alternative<ast::TyAssoc>((*type_expression))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expression)); auto [parameter_name, associated_name] = _v_tyassoc; return std::make_shared<registry::Type>(registry::TAssoc(parameter_name, associated_name)); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

std::shared_ptr<registry::Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry) noexcept{return type_from_annotation_with_registry_and_stack(type_expression, registry, {});}

bool type_alias_has_cycle_from_name(registry::TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept{return !registry.type_alias_annotations.has(current_name) ? false : resolution_stack_contains_name(resolution_stack, current_name) ? true : [&]() -> bool { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{current_name});
  mlc::String target_name = type_alias_named_target(registry.type_alias_annotations.get(current_name));
  return target_name == mlc::String("") ? false : type_alias_has_cycle_from_name(registry, target_name, extended_stack);
 }();}

bool type_alias_has_cycle(registry::TypeRegistry registry, mlc::String alias_name) noexcept{return type_alias_has_cycle_from_name(registry, alias_name, {});}

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<ast::TyI32>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TI32{})); } if (std::holds_alternative<ast::TyString>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<ast::TyBool>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TBool{})); } if (std::holds_alternative<ast::TyUnit>((*type_expr))) {  return std::make_shared<registry::Type>((registry::TUnit{})); } if (std::holds_alternative<ast::TyNamed>((*type_expr))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expr)); auto [name] = _v_tynamed; return [&]() { if (name == mlc::String("i64")) { return std::make_shared<registry::Type>((registry::TI64{})); } if (name == mlc::String("f64")) { return std::make_shared<registry::Type>((registry::TF64{})); } if (name == mlc::String("u8")) { return std::make_shared<registry::Type>((registry::TU8{})); } if (name == mlc::String("usize")) { return std::make_shared<registry::Type>((registry::TUsize{})); } if (name == mlc::String("char")) { return std::make_shared<registry::Type>((registry::TChar{})); } return std::make_shared<registry::Type>(registry::TNamed(name)); }(); } if (std::holds_alternative<ast::TyArray>((*type_expr))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyShared>((*type_expr))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation(inner))); } if (std::holds_alternative<ast::TyGeneric>((*type_expr))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expr)); auto [name, type_arguments] = _v_tygeneric; return name == mlc::String("ref") && type_arguments.size() == 1 ? type_from_annotation(type_arguments[0]) : std::make_shared<registry::Type>(registry::TGeneric(name, type_arguments.map([](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation(type_argument); }))); } if (std::holds_alternative<ast::TyFn>((*type_expr))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expr)); auto [parameters, return_type] = _v_tyfn; return std::make_shared<registry::Type>(registry::TFn(parameters.map([](std::shared_ptr<ast::TypeExpr> annotation) mutable { return type_from_annotation(annotation); }), type_from_annotation(return_type))); } if (std::holds_alternative<ast::TyAssoc>((*type_expr))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expr)); auto [param, assoc] = _v_tyassoc; return std::make_shared<registry::Type>(registry::TAssoc(param, assoc)); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

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

registry::TypeRegistry build_registry(ast::Program program) noexcept{
ast::Program expanded = trait_param_expand::expand_trait_as_param_program(program);
registry::TypeRegistry registry = empty_registry();
int index = 0;
while (index < expanded.decls.size()){
{
register_decl_into(registry, expanded.decls[index]);
index = index + 1;
}
}
return registry;
}

mlc::Array<mlc::String> accumulate_trait_associated_type_name(mlc::Array<mlc::String> accumulated_names, std::shared_ptr<ast::Decl> method) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::DeclAssocType>((*method))) { auto _v_declassoctype = std::get<ast::DeclAssocType>((*method)); auto [assoc_name, _w0] = _v_declassoctype; return accumulated_names.concat(mlc::Array<mlc::String>{assoc_name}); } return accumulated_names; }();}

void register_decl_into(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
return std::visit(overloaded{
  [&](const DeclTrait& decltrait) -> void { auto [trait_name, _w0, methods, _w1] = decltrait; [&]() { 
  int index = 0;
  while (index < methods.size()){
{
std::shared_ptr<ast::Decl> method = methods[index];
bool is_assoc_type_declaration = [&]() { if (std::holds_alternative<ast::DeclAssocType>((*method))) { auto _v_declassoctype = std::get<ast::DeclAssocType>((*method)); auto [_w0, _w1] = _v_declassoctype; return true; } return false; }();
if (!is_assoc_type_declaration){
{
register_decl_into(registry, method);
}
}
index = index + 1;
}
}
  mlc::Array<mlc::String> initial_trait_associated_type_names = registry.adt_index.trait_assoc_types.has(trait_name) ? registry.adt_index.trait_assoc_types.get(trait_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
  mlc::Array<mlc::String> trait_associated_type_names = methods.fold(initial_trait_associated_type_names, accumulate_trait_associated_type_name);
  return registry.adt_index.trait_assoc_types.set(trait_name, trait_associated_type_names);
 }(); },
  [&](const DeclExtend& declextend) -> void { auto [type_name, trait_name, methods, _w0] = declextend; [&]() { 
  if (trait_name.length() > 0){
{
mlc::Array<mlc::String> trait_implementations = registry.adt_index.trait_impls.has(type_name) ? registry.adt_index.trait_impls.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }().concat(mlc::Array<mlc::String>{trait_name});
registry.adt_index.trait_impls.set(type_name, trait_implementations);
}
}
  mlc::String assoc_binding_key = type_name + mlc::String("::") + trait_name;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> assoc_type_bindings = registry.adt_index.assoc_type_bindings.has(assoc_binding_key) ? registry.adt_index.assoc_type_bindings.get(assoc_binding_key) : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_bindings = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  return empty_bindings;
 }();
  bool assoc_bindings_dirty = false;
  int i = 0;
  while (i < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclAssocBind>((*methods[i]))) { auto _v_declassocbind = std::get<ast::DeclAssocBind>((*methods[i])); auto [assoc_name, type_expr, _w0] = _v_declassocbind; return [&]() { 
  assoc_type_bindings.set(assoc_name, type_from_annotation_with_registry(type_expr, registry));
  assoc_bindings_dirty = true;
 }(); } return register_decl_into(registry, methods[i]); }();
i = i + 1;
}
}
  if (assoc_bindings_dirty){
registry.adt_index.assoc_type_bindings.set(assoc_binding_key, assoc_type_bindings);
}
 }(); },
  [&](const DeclFn& declfn) -> void { auto [name, type_parameters, trait_bounds, parameters, return_type, _w0, _w1] = declfn; [&]() { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = parameters.map([registry](std::shared_ptr<ast::Param> parameter) mutable { return type_from_annotation_with_registry(ast::param_type_value(parameter), registry); });
  mlc::Array<mlc::String> parameter_names = parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return ast::param_name(parameter); });
  mlc::Array<int> parameter_mutability_flags = parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return ast::param_is_mut(parameter) ? 1 : 0; });
  registry.function_index.function_types.set(name, std::make_shared<registry::Type>(registry::TFn(param_types, type_from_annotation_with_registry(return_type, registry))));
  registry.function_index.function_type_parameter_names.set(name, type_parameters);
  registry.function_index.function_parameter_names.set(name, parameter_names);
  registry.function_index.function_parameter_mutability_flags.set(name, parameter_mutability_flags);
  registry.function_index.function_required_arity.set(name, required_arity_from_params(parameters));
  if (trait_bounds.size() > 0){
registry.function_index.function_trait_bounds.set(name, trait_bounds);
}
 }(); },
  [&](const DeclType& decltype_) -> void { auto [type_name, type_parameters, variants, _w0, _w1] = decltype_; [&]() { 
  registry.adt_index.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
  int vi = 0;
  while (vi < variants.size()){
{
register_variant_into(registry, type_name, variants[vi]);
vi = vi + 1;
}
}
  mlc::Array<mlc::String> phantom = compute_phantom_type_params(type_parameters, variants);
  return registry.adt_index.algebraic_decl_phantom_type_params.set(type_name, phantom);
 }(); },
  [&](const DeclTypeAlias& decltypealias) -> void { auto [alias_name, type_parameters, type_expression, _w0] = decltypealias; [&]() { 
  registry.type_alias_annotations.set(alias_name, type_expression);
  if (type_parameters.size() > 0){
registry.type_alias_type_parameter_names.set(alias_name, type_parameters);
}
 }(); },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; std::make_tuple(); },
  [&](const DeclExported& declexported) -> void { auto [exported_declaration] = declexported; register_decl_into(registry, exported_declaration); },
  [&](const DeclAssocType& declassoctype) -> void { auto [_w0, _w1] = declassoctype; std::make_tuple(); },
  [&](const DeclAssocBind& declassocbind) -> void { auto [_w0, _w1, _w2] = declassocbind; std::make_tuple(); }
}, (*declaration));
}

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [inner] = _v_tyshared; return type_param_in_annotation(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [_w0, type_arguments] = _v_tygeneric; return type_param_in_annotation_list(param, type_arguments); } if (std::holds_alternative<ast::TyFn>((*type_expression))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expression)); auto [params, return_type] = _v_tyfn; return type_param_in_annotation_list(param, params) || type_param_in_annotation(param, return_type); } if (std::holds_alternative<ast::TyAssoc>((*type_expression))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expression)); auto [parameter_name, _w0] = _v_tyassoc; return parameter_name == param; } return false; }();}

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{return types.any([param](std::shared_ptr<ast::TypeExpr> type_expression) mutable { return type_param_in_annotation(param, type_expression); });}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_types, _w1] = vartuple; return field_types; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_definitions, _w1] = varrecord; return field_definitions.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> empty_type_expressions = {};
  return empty_type_expressions;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto all_field_type_expressions = mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> variant_definition) mutable { return variant_annotation_typeexprs(variant_definition); });
return type_parameters.filter([all_field_type_expressions](mlc::String type_parameter) mutable { return !type_param_in_annotation_list(type_parameter, all_field_type_expressions); });
}

void register_variant_into(registry::TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<registry::Type> result_type = std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> void { auto [variant_name, is_private] = varunit; [&]() { 
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, {});
  if (is_private){
registry.adt_index.private_constructors.set(variant_name, true);
}
 }(); },
  [&](const VarTuple& vartuple) -> void { auto [variant_name, field_types, is_private] = vartuple; [&]() { 
  mlc::Array<std::shared_ptr<registry::Type>> field_type_list = field_types.map([registry](std::shared_ptr<ast::TypeExpr> field_type_expression) mutable { return type_from_annotation_with_registry(field_type_expression, registry); });
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, field_type_list);
  if (is_private){
registry.adt_index.private_constructors.set(variant_name, true);
}
 }(); },
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs, is_private] = varrecord; [&]() { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>> defaults_for_variant = mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>();
  mlc::Array<mlc::String> ordered_names = field_defs.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->name; });
  int index = 0;
  while (index < field_defs.size()){
{
std::shared_ptr<ast::FieldDef> field_definition = field_defs[index];
field_map.set(field_definition->name, type_from_annotation_with_registry(field_definition->type_value, registry));
if (field_definition->has_default_expression){
{
defaults_for_variant.set(field_definition->name, field_definition->default_expression);
}
}
index = index + 1;
}
}
  registry.adt_index.constructor_types.set(variant_name, result_type);
  registry.adt_index.constructor_parameters.set(variant_name, {});
  registry.record_index.field_types.set(variant_name, field_map);
  registry.record_index.field_types.set(type_name, field_map);
  registry.record_index.record_field_names_ordered.set(variant_name, ordered_names);
  registry.record_index.record_field_names_ordered.set(type_name, ordered_names);
  registry.record_index.record_literal_field_defaults.set(variant_name, defaults_for_variant);
  registry.record_index.record_literal_field_defaults.set(type_name, defaults_for_variant);
  if (is_private){
registry.adt_index.private_constructors.set(variant_name, true);
}
 }(); }
}, (*variant));
}

std::shared_ptr<registry::Type> substitute_semantic_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return substitution.has(name) ? substitute_semantic_type(substitution.get(name), substitution) : type_value; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(substitute_semantic_type(inner, substitution))); } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return std::make_shared<registry::Type>(registry::TShared(substitute_semantic_type(inner, substitution))); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, type_arguments] = _v_tgeneric; return std::make_shared<registry::Type>(registry::TGeneric(name, type_arguments.map([substitution](std::shared_ptr<registry::Type> argument) mutable { return substitute_semantic_type(argument, substitution); }))); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [a, b] = _v_tpair; return std::make_shared<registry::Type>(registry::TPair(substitute_semantic_type(a, substitution), substitute_semantic_type(b, substitution))); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [parameter_types] = _v_ttuple; return std::make_shared<registry::Type>(registry::TTuple(parameter_types.map([substitution](std::shared_ptr<registry::Type> entry) mutable { return substitute_semantic_type(entry, substitution); }))); } if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [parameter_list, return_type] = _v_tfn; return std::make_shared<registry::Type>(registry::TFn(parameter_list.map([substitution](std::shared_ptr<registry::Type> parameter) mutable { return substitute_semantic_type(parameter, substitution); }), substitute_semantic_type(return_type, substitution))); } return type_value; }();}

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*object_type))) { auto _v_tshared = std::get<registry::TShared>((*object_type)); auto [inner] = _v_tshared; return inner; } return object_type; }();
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return resolve_nominal_type_name_for_fields(registry, name, {}); } if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [name, _w0] = _v_tgeneric; return resolve_nominal_type_name_for_fields(registry, name, {}); } return mlc::String(""); }();
mlc::Array<std::shared_ptr<registry::Type>> type_arguments = [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [_w0, type_argument_list] = _v_tgeneric; return type_argument_list; } return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty_type_arguments = {};
  return empty_type_arguments;
 }(); }();
return type_name != mlc::String("") && registry.record_index.field_types.has(type_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> raw_field_type = field_map.get(field_name);
  return type_arguments.size() > 0 ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<mlc::String> parameter_names = TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
  return substitute_semantic_type(raw_field_type, type_parameter_substitution_from_type_arguments(parameter_names, type_arguments));
 }() : raw_field_type;
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
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

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

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_variant_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept;

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

std::shared_ptr<ast::TypeExpr> type_expression_substitute_for_named(mlc::String name, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept;

std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept;

std::shared_ptr<ast::TypeExpr> substitute_type_expression_type_parameters(std::shared_ptr<ast::TypeExpr> type_expression, mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_expressions) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_parameter_substitution_from_type_arguments(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;

bool resolution_stack_contains_name(mlc::Array<mlc::String> resolution_stack, mlc::String candidate_name) noexcept;

mlc::String type_alias_named_target(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::String resolve_nominal_type_name_for_fields(registry::TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> resolve_named_type_with_registry(mlc::String name, registry::TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_generic_with_registry_and_stack(mlc::String generic_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments, registry::TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry) noexcept;

bool type_alias_has_cycle_from_name(registry::TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept;

bool type_alias_has_cycle(registry::TypeRegistry registry, mlc::String alias_name) noexcept;

std::shared_ptr<registry::Type> type_from_named_annotation(mlc::String name) noexcept;

std::shared_ptr<registry::Type> type_from_generic_annotation(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

registry::TypeRegistry build_registry(ast::Program program) noexcept;

mlc::Array<mlc::String> accumulate_trait_associated_type_name(mlc::Array<mlc::String> accumulated_names, std::shared_ptr<ast::Decl> method) noexcept;

mlc::Array<mlc::String> trait_associated_type_names_for(registry::TypeRegistry registry, mlc::String trait_name) noexcept;

bool trait_method_is_assoc_type_declaration(std::shared_ptr<ast::Decl> method) noexcept;

void register_trait_method_into_registry(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> method) noexcept;

void register_decl_trait_into_registry(registry::TypeRegistry& registry, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

mlc::String trait_base_name(mlc::String trait_name) noexcept;

void register_decl_extend_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

void register_decl_fn_into_registry(registry::TypeRegistry& registry, mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> trait_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type) noexcept;

mlc::String variant_name_of(std::shared_ptr<ast::TypeVariant> variant) noexcept;

void register_decl_type_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

void register_decl_type_alias_into_registry(registry::TypeRegistry& registry, mlc::String alias_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool register_extend_method_into_registry(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& assoc_type_bindings) noexcept;

bool register_decl_trait_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool register_decl_extend_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool register_decl_fn_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool register_decl_type_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool register_decl_type_alias_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool register_decl_exported_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

void registry_decl_noop() noexcept;

void register_decl_into(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;

bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

void register_variant_unit_into_registry(registry::TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, bool is_private) noexcept;

void register_variant_tuple_into_registry(registry::TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, bool is_private) noexcept;

void register_variant_record_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, bool is_private) noexcept;

void register_variant_into(registry::TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<registry::Type> substitute_semantic_type_for_named(mlc::String name, std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

std::shared_ptr<registry::Type> substitute_semantic_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

std::shared_ptr<registry::Type> unwrap_shared_semantic_type(std::shared_ptr<registry::Type> object_type) noexcept;

mlc::String field_lookup_type_name_from_semantic_type(std::shared_ptr<registry::Type> inner_type, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> generic_type_arguments_from_semantic_type(std::shared_ptr<registry::Type> inner_type) noexcept;

mlc::String method_owner_type_name_from_semantic_type(std::shared_ptr<registry::Type> inner_type) noexcept;

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

mlc::Array<mlc::String> TypeRegistry_algebraic_decl_variant_names_for(registry::TypeRegistry self, mlc::String algebraic_type_name) noexcept{return self.adt_index.algebraic_decl_variant_names.has(algebraic_type_name) ? self.adt_index.algebraic_decl_variant_names.get(algebraic_type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_variant_names = {};
  return empty_variant_names;
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
builtin_function_types.set(mlc::String("eprintln"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("exit"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}, std::make_shared<registry::Type>((registry::TUnit{})))));
builtin_function_types.set(mlc::String("args"), std::make_shared<registry::Type>(registry::TFn({}, std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TString{})))))));
builtin_function_types.set(mlc::String("read_line"), std::make_shared<registry::Type>(registry::TFn({}, std::make_shared<registry::Type>((registry::TString{})))));
mlc::HashMap<mlc::String, int> builtin_required_arity = mlc::HashMap<mlc::String, int>();
builtin_required_arity.set(mlc::String("print"), 1);
builtin_required_arity.set(mlc::String("println"), 1);
builtin_required_arity.set(mlc::String("eprintln"), 1);
builtin_required_arity.set(mlc::String("exit"), 1);
builtin_required_arity.set(mlc::String("args"), 0);
builtin_function_types.set(mlc::String("spawn_task"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUnknown{}))}, std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Task"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUnknown{}))})))));
builtin_required_arity.set(mlc::String("spawn_task"), 1);
builtin_function_types.set(mlc::String("make_channel"), std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUsize{}))}, std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Channel"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TUnknown{}))})))));
builtin_required_arity.set(mlc::String("make_channel"), 1);
builtin_required_arity.set(mlc::String("read_line"), 0);
mlc::HashMap<mlc::String, bool> empty_private_constructors = mlc::HashMap<mlc::String, bool>();
return registry::TypeRegistry{registry::FunctionIndex{builtin_function_types, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<int>>(), builtin_required_arity, mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>>()}, registry::AdtIndex{constructor_types_map, constructor_parameters_map, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), empty_private_constructors, mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>()}, registry::RecordIndex{mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>>()}, mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};
}

std::shared_ptr<registry::Type> primitive_type_from_name(mlc::String name) noexcept{return name == mlc::String("i64") ? std::make_shared<registry::Type>((registry::TI64{})) : name == mlc::String("f64") ? std::make_shared<registry::Type>((registry::TF64{})) : name == mlc::String("u8") ? std::make_shared<registry::Type>((registry::TU8{})) : name == mlc::String("usize") ? std::make_shared<registry::Type>((registry::TUsize{})) : name == mlc::String("char") ? std::make_shared<registry::Type>((registry::TChar{})) : name == mlc::String("i32") ? std::make_shared<registry::Type>((registry::TI32{})) : name == mlc::String("string") ? std::make_shared<registry::Type>((registry::TString{})) : name == mlc::String("bool") ? std::make_shared<registry::Type>((registry::TBool{})) : name == mlc::String("unit") ? std::make_shared<registry::Type>((registry::TUnit{})) : std::make_shared<registry::Type>(registry::TNamed(name));}

std::shared_ptr<ast::TypeExpr> type_expression_substitute_for_named(mlc::String name, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept{return name == parameter_name ? argument_expression : type_expression;}

std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept{return std::visit(overloaded{
  [&](const TyArray& tyarray) -> std::shared_ptr<ast::TypeExpr> { auto [inner] = tyarray; return std::make_shared<ast::TypeExpr>(ast::TyArray(type_expression_substitute_named_parameter(inner, parameter_name, argument_expression))); },
  [&](const TyShared& tyshared) -> std::shared_ptr<ast::TypeExpr> { auto [inner] = tyshared; return std::make_shared<ast::TypeExpr>(ast::TyShared(type_expression_substitute_named_parameter(inner, parameter_name, argument_expression))); },
  [&](const TyGeneric& tygeneric) -> std::shared_ptr<ast::TypeExpr> { auto [name, type_arguments] = tygeneric; return std::make_shared<ast::TypeExpr>(ast::TyGeneric(name, type_arguments.map([parameter_name, argument_expression](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_expression_substitute_named_parameter(type_argument, parameter_name, argument_expression); }))); },
  [&](const TyFn& tyfn) -> std::shared_ptr<ast::TypeExpr> { auto [parameters, return_type] = tyfn; return std::make_shared<ast::TypeExpr>(ast::TyFn(parameters.map([parameter_name, argument_expression](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_expression_substitute_named_parameter(parameter_type, parameter_name, argument_expression); }), type_expression_substitute_named_parameter(return_type, parameter_name, argument_expression))); },
  [&](const TyNamed& tynamed) -> std::shared_ptr<ast::TypeExpr> { auto [name] = tynamed; return type_expression_substitute_for_named(name, type_expression, parameter_name, argument_expression); },
  [&](const TyI32& tyi32) -> std::shared_ptr<ast::TypeExpr> { return type_expression; },
  [&](const TyString& tystring) -> std::shared_ptr<ast::TypeExpr> { return type_expression; },
  [&](const TyBool& tybool) -> std::shared_ptr<ast::TypeExpr> { return type_expression; },
  [&](const TyUnit& tyunit) -> std::shared_ptr<ast::TypeExpr> { return type_expression; },
  [&](const TyAssoc& tyassoc) -> std::shared_ptr<ast::TypeExpr> { auto [_w0, _w1] = tyassoc; return type_expression; }
}, (*type_expression));}

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

std::shared_ptr<registry::Type> type_from_annotation_generic_with_registry_and_stack(mlc::String generic_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments, registry::TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept{return generic_name == mlc::String("ref") && type_arguments.size() == 1 ? type_from_annotation_with_registry_and_stack(type_arguments[0], type_registry, resolution_stack) : type_registry.type_alias_annotations.has(generic_name) && type_registry.type_alias_type_parameter_names.has(generic_name) ? resolution_stack_contains_name(resolution_stack, generic_name) ? std::make_shared<registry::Type>((registry::TUnknown{})) : [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{generic_name});
  std::shared_ptr<ast::TypeExpr> substituted_expression = substitute_type_expression_type_parameters(type_registry.type_alias_annotations.get(generic_name), type_registry.type_alias_type_parameter_names.get(generic_name), type_arguments);
  return type_from_annotation_with_registry_and_stack(substituted_expression, type_registry, extended_stack);
 }() : std::make_shared<registry::Type>(registry::TGeneric(generic_name, type_arguments.map([type_registry, resolution_stack](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation_with_registry_and_stack(type_argument, type_registry, resolution_stack); })));}

std::shared_ptr<registry::Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TI32{})); },
  [&](const TyString& tystring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TString{})); },
  [&](const TyBool& tybool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TBool{})); },
  [&](const TyUnit& tyunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnit{})); },
  [&](const TyArray& tyarray) -> std::shared_ptr<registry::Type> { auto [inner] = tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation_with_registry_and_stack(inner, type_registry, resolution_stack))); },
  [&](const TyShared& tyshared) -> std::shared_ptr<registry::Type> { auto [inner] = tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation_with_registry_and_stack(inner, type_registry, resolution_stack))); },
  [&](const TyFn& tyfn) -> std::shared_ptr<registry::Type> { auto [parameters, return_type] = tyfn; return std::make_shared<registry::Type>(registry::TFn(parameters.map([type_registry, resolution_stack](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_from_annotation_with_registry_and_stack(parameter_type, type_registry, resolution_stack); }), type_from_annotation_with_registry_and_stack(return_type, type_registry, resolution_stack))); },
  [&](const TyAssoc& tyassoc) -> std::shared_ptr<registry::Type> { auto [parameter_name, associated_name] = tyassoc; return std::make_shared<registry::Type>(registry::TAssoc(parameter_name, associated_name)); },
  [&](const TyNamed& tynamed) -> std::shared_ptr<registry::Type> { auto [name] = tynamed; return resolve_named_type_with_registry(name, type_registry, resolution_stack); },
  [&](const TyGeneric& tygeneric) -> std::shared_ptr<registry::Type> { auto [generic_name, type_arguments] = tygeneric; return type_from_annotation_generic_with_registry_and_stack(generic_name, type_arguments, type_registry, resolution_stack); }
}, (*type_expression));}

std::shared_ptr<registry::Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry) noexcept{return type_from_annotation_with_registry_and_stack(type_expression, registry, {});}

bool type_alias_has_cycle_from_name(registry::TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept{return !registry.type_alias_annotations.has(current_name) ? false : resolution_stack_contains_name(resolution_stack, current_name) ? true : [&]() -> bool { 
  mlc::Array<mlc::String> extended_stack = resolution_stack.concat(mlc::Array<mlc::String>{current_name});
  mlc::String target_name = type_alias_named_target(registry.type_alias_annotations.get(current_name));
  return target_name == mlc::String("") ? false : type_alias_has_cycle_from_name(registry, target_name, extended_stack);
 }();}

bool type_alias_has_cycle(registry::TypeRegistry registry, mlc::String alias_name) noexcept{return type_alias_has_cycle_from_name(registry, alias_name, {});}

std::shared_ptr<registry::Type> type_from_named_annotation(mlc::String name) noexcept{return name == mlc::String("i64") ? std::make_shared<registry::Type>((registry::TI64{})) : name == mlc::String("f64") ? std::make_shared<registry::Type>((registry::TF64{})) : name == mlc::String("u8") ? std::make_shared<registry::Type>((registry::TU8{})) : name == mlc::String("usize") ? std::make_shared<registry::Type>((registry::TUsize{})) : name == mlc::String("char") ? std::make_shared<registry::Type>((registry::TChar{})) : std::make_shared<registry::Type>(registry::TNamed(name));}

std::shared_ptr<registry::Type> type_from_generic_annotation(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept{return name == mlc::String("ref") && type_arguments.size() == 1 ? type_from_annotation(type_arguments[0]) : std::make_shared<registry::Type>(registry::TGeneric(name, type_arguments.map([](std::shared_ptr<ast::TypeExpr> type_argument) mutable { return type_from_annotation(type_argument); })));}

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TI32{})); },
  [&](const TyString& tystring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TString{})); },
  [&](const TyBool& tybool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TBool{})); },
  [&](const TyUnit& tyunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnit{})); },
  [&](const TyArray& tyarray) -> std::shared_ptr<registry::Type> { auto [inner] = tyarray; return std::make_shared<registry::Type>(registry::TArray(type_from_annotation(inner))); },
  [&](const TyShared& tyshared) -> std::shared_ptr<registry::Type> { auto [inner] = tyshared; return std::make_shared<registry::Type>(registry::TShared(type_from_annotation(inner))); },
  [&](const TyFn& tyfn) -> std::shared_ptr<registry::Type> { auto [parameters, return_type] = tyfn; return std::make_shared<registry::Type>(registry::TFn(parameters.map([](std::shared_ptr<ast::TypeExpr> annotation) mutable { return type_from_annotation(annotation); }), type_from_annotation(return_type))); },
  [&](const TyAssoc& tyassoc) -> std::shared_ptr<registry::Type> { auto [param, assoc] = tyassoc; return std::make_shared<registry::Type>(registry::TAssoc(param, assoc)); },
  [&](const TyNamed& tynamed) -> std::shared_ptr<registry::Type> { auto [name] = tynamed; return type_from_named_annotation(name); },
  [&](const TyGeneric& tygeneric) -> std::shared_ptr<registry::Type> { auto [name, type_arguments] = tygeneric; return type_from_generic_annotation(name, type_arguments); }
}, (*type_expr));}

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

mlc::Array<mlc::String> trait_associated_type_names_for(registry::TypeRegistry registry, mlc::String trait_name) noexcept{return registry.adt_index.trait_assoc_types.has(trait_name) ? registry.adt_index.trait_assoc_types.get(trait_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();}

bool trait_method_is_assoc_type_declaration(std::shared_ptr<ast::Decl> method) noexcept{return [&]() { if (std::holds_alternative<ast::DeclAssocType>((*method))) { auto _v_declassoctype = std::get<ast::DeclAssocType>((*method)); auto [_w0, _w1] = _v_declassoctype; return true; } return false; }();}

void register_trait_method_into_registry(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> method) noexcept{
if (!trait_method_is_assoc_type_declaration(method)){
register_decl_into(registry, method);
}
}

void register_decl_trait_into_registry(registry::TypeRegistry& registry, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
int index = 0;
while (index < methods.size()){
{
register_trait_method_into_registry(registry, methods[index]);
index = index + 1;
}
}
mlc::Array<mlc::String> initial_trait_associated_type_names = trait_associated_type_names_for(registry, trait_name);
mlc::Array<mlc::String> trait_associated_type_names = methods.fold(initial_trait_associated_type_names, accumulate_trait_associated_type_name);
if (true){
registry.adt_index.trait_assoc_types.set(trait_name, trait_associated_type_names);
}
}

mlc::String trait_base_name(mlc::String trait_name) noexcept{
int index = 0;
while (index < trait_name.length()){
{
if (trait_name.char_at(index) == mlc::String("<")){
{
return trait_name.substring(0, index);
}
}
index = index + 1;
}
}
return trait_name;
}

void register_decl_extend_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
mlc::String bare_trait_name = trait_base_name(trait_name);
if (trait_name.length() > 0){
{
mlc::Array<mlc::String> trait_implementations = registry.adt_index.trait_impls.has(type_name) ? registry.adt_index.trait_impls.get(type_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }();
trait_implementations = trait_implementations.concat(mlc::Array<mlc::String>{trait_name});
if (bare_trait_name != trait_name){
{
trait_implementations = trait_implementations.concat(mlc::Array<mlc::String>{bare_trait_name});
}
}
registry.adt_index.trait_impls.set(type_name, trait_implementations);
}
}
mlc::String assoc_binding_key = type_name + mlc::String("::") + bare_trait_name;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> assoc_type_bindings = registry.adt_index.assoc_type_bindings.has(assoc_binding_key) ? registry.adt_index.assoc_type_bindings.get(assoc_binding_key) : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_bindings = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  return empty_bindings;
 }();
bool assoc_bindings_dirty = false;
int method_index = 0;
while (method_index < methods.size()){
{
if (register_extend_method_into_registry(registry, methods[method_index], assoc_type_bindings)){
{
assoc_bindings_dirty = true;
}
}
method_index = method_index + 1;
}
}
if (assoc_bindings_dirty){
registry.adt_index.assoc_type_bindings.set(assoc_binding_key, assoc_type_bindings);
}
}

void register_decl_fn_into_registry(registry::TypeRegistry& registry, mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> trait_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type) noexcept{
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
}

mlc::String variant_name_of(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [variant_name, _w0] = varunit; return variant_name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [variant_name, _w0, _w1] = vartuple; return variant_name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [variant_name, _w0, _w1] = varrecord; return variant_name; }
}, (*variant));}

void register_decl_type_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
registry.adt_index.algebraic_decl_type_parameter_names.set(type_name, type_parameters);
mlc::Array<mlc::String> variant_names = {};
int variant_index = 0;
while (variant_index < variants.size()){
{
register_variant_into(registry, type_name, variants[variant_index]);
variant_names = variant_names.concat(mlc::Array<mlc::String>{variant_name_of(variants[variant_index])});
variant_index = variant_index + 1;
}
}
registry.adt_index.algebraic_decl_variant_names.set(type_name, variant_names);
mlc::Array<mlc::String> phantom = compute_phantom_type_params(type_parameters, variants);
if (true){
registry.adt_index.algebraic_decl_phantom_type_params.set(type_name, phantom);
}
}

void register_decl_type_alias_into_registry(registry::TypeRegistry& registry, mlc::String alias_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
registry.type_alias_annotations.set(alias_name, type_expression);
if (type_parameters.size() > 0){
registry.type_alias_type_parameter_names.set(alias_name, type_parameters);
}
}

bool register_extend_method_into_registry(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& assoc_type_bindings) noexcept{return [&]() { if (std::holds_alternative<ast::DeclAssocBind>((*method))) { auto _v_declassocbind = std::get<ast::DeclAssocBind>((*method)); auto [assoc_name, type_expr, _w0] = _v_declassocbind; return [&]() -> bool { 
  assoc_type_bindings.set(assoc_name, type_from_annotation_with_registry(type_expr, registry));
  return true;
 }(); } return [&]() -> bool { 
  register_decl_into(registry, method);
  return false;
 }(); }();}

bool register_decl_trait_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclTrait>((*declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*declaration)); auto [trait_name, _w0, methods, _w1] = _v_decltrait; return [&]() -> bool { 
  register_decl_trait_into_registry(registry, trait_name, methods);
  return true;
 }(); } return false; }();}

bool register_decl_extend_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [type_name, trait_name, methods, _w0] = _v_declextend; return [&]() -> bool { 
  register_decl_extend_into_registry(registry, type_name, trait_name, methods);
  return true;
 }(); } return false; }();}

bool register_decl_fn_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [name, type_parameters, trait_bounds, parameters, return_type, _w0, _w1] = _v_declfn; return [&]() -> bool { 
  register_decl_fn_into_registry(registry, name, type_parameters, trait_bounds, parameters, return_type);
  return true;
 }(); } return false; }();}

bool register_decl_type_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclType>((*declaration))) { auto _v_decltype = std::get<ast::DeclType>((*declaration)); auto [type_name, type_parameters, variants, _w0, _w1] = _v_decltype; return [&]() -> bool { 
  register_decl_type_into_registry(registry, type_name, type_parameters, variants);
  return true;
 }(); } return false; }();}

bool register_decl_type_alias_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclTypeAlias>((*declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*declaration)); auto [alias_name, type_parameters, type_expression, _w0] = _v_decltypealias; return [&]() -> bool { 
  register_decl_type_alias_into_registry(registry, alias_name, type_parameters, type_expression);
  return true;
 }(); } return false; }();}

bool register_decl_exported_if(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [exported_declaration] = _v_declexported; return [&]() -> bool { 
  register_decl_into(registry, exported_declaration);
  return true;
 }(); } return false; }();}

void registry_decl_noop() noexcept{
}

void register_decl_into(registry::TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept{
bool handled = register_decl_trait_if(registry, declaration) || register_decl_extend_if(registry, declaration) || register_decl_fn_if(registry, declaration) || register_decl_type_if(registry, declaration) || register_decl_type_alias_if(registry, declaration) || register_decl_exported_if(registry, declaration);
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

void register_variant_unit_into_registry(registry::TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, bool is_private) noexcept{
registry.adt_index.constructor_types.set(variant_name, result_type);
registry.adt_index.constructor_parameters.set(variant_name, {});
if (is_private){
registry.adt_index.private_constructors.set(variant_name, true);
}
}

void register_variant_tuple_into_registry(registry::TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, bool is_private) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> field_type_list = field_types.map([registry](std::shared_ptr<ast::TypeExpr> field_type_expression) mutable { return type_from_annotation_with_registry(field_type_expression, registry); });
registry.adt_index.constructor_types.set(variant_name, result_type);
registry.adt_index.constructor_parameters.set(variant_name, field_type_list);
if (is_private){
registry.adt_index.private_constructors.set(variant_name, true);
}
}

void register_variant_record_into_registry(registry::TypeRegistry& registry, mlc::String type_name, mlc::String variant_name, std::shared_ptr<registry::Type> result_type, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, bool is_private) noexcept{
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
}

void register_variant_into(registry::TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
std::shared_ptr<registry::Type> result_type = std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> void { auto [variant_name, is_private] = varunit; register_variant_unit_into_registry(registry, variant_name, result_type, is_private); },
  [&](const VarTuple& vartuple) -> void { auto [variant_name, field_types, is_private] = vartuple; register_variant_tuple_into_registry(registry, variant_name, result_type, field_types, is_private); },
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs, is_private] = varrecord; register_variant_record_into_registry(registry, type_name, variant_name, result_type, field_defs, is_private); }
}, (*variant));
}

std::shared_ptr<registry::Type> substitute_semantic_type_for_named(mlc::String name, std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{return substitution.has(name) ? substitute_semantic_type(substitution.get(name), substitution) : type_value;}

std::shared_ptr<registry::Type> substitute_semantic_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{return std::visit(overloaded{
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [inner] = tarray; return std::make_shared<registry::Type>(registry::TArray(substitute_semantic_type(inner, substitution))); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [inner] = tshared; return std::make_shared<registry::Type>(registry::TShared(substitute_semantic_type(inner, substitution))); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [name, type_arguments] = tgeneric; return std::make_shared<registry::Type>(registry::TGeneric(name, type_arguments.map([substitution](std::shared_ptr<registry::Type> argument) mutable { return substitute_semantic_type(argument, substitution); }))); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [a, b] = tpair; return std::make_shared<registry::Type>(registry::TPair(substitute_semantic_type(a, substitution), substitute_semantic_type(b, substitution))); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [parameter_types] = ttuple; return std::make_shared<registry::Type>(registry::TTuple(parameter_types.map([substitution](std::shared_ptr<registry::Type> entry) mutable { return substitute_semantic_type(entry, substitution); }))); },
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [parameter_list, return_type] = tfn; return std::make_shared<registry::Type>(registry::TFn(parameter_list.map([substitution](std::shared_ptr<registry::Type> parameter) mutable { return substitute_semantic_type(parameter, substitution); }), substitute_semantic_type(return_type, substitution))); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [name] = tnamed; return substitute_semantic_type_for_named(name, type_value, substitution); },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return type_value; },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tassoc; return type_value; },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return type_value; }
}, (*type_value));}

std::shared_ptr<registry::Type> unwrap_shared_semantic_type(std::shared_ptr<registry::Type> object_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*object_type))) { auto _v_tshared = std::get<registry::TShared>((*object_type)); auto [inner] = _v_tshared; return inner; } return object_type; }();}

mlc::String field_lookup_type_name_from_semantic_type(std::shared_ptr<registry::Type> inner_type, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return resolve_nominal_type_name_for_fields(registry, name, {}); } if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [name, _w0] = _v_tgeneric; return resolve_nominal_type_name_for_fields(registry, name, {}); } return mlc::String(""); }();}

mlc::Array<std::shared_ptr<registry::Type>> generic_type_arguments_from_semantic_type(std::shared_ptr<registry::Type> inner_type) noexcept{return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [_w0, type_argument_list] = _v_tgeneric; return type_argument_list; } return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty_type_arguments = {};
  return empty_type_arguments;
 }(); }();}

mlc::String method_owner_type_name_from_semantic_type(std::shared_ptr<registry::Type> inner_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();}

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = unwrap_shared_semantic_type(object_type);
mlc::String type_name = field_lookup_type_name_from_semantic_type(inner_type, registry);
mlc::Array<std::shared_ptr<registry::Type>> type_arguments = generic_type_arguments_from_semantic_type(inner_type);
if (type_name == mlc::String("") || !registry.record_index.field_types.has(type_name)){
{
return std::make_shared<registry::Type>((registry::TUnknown{}));
}
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = TypeRegistry_fields_for(registry, type_name);
if (!field_map.has(field_name)){
{
return std::make_shared<registry::Type>((registry::TUnknown{}));
}
}
std::shared_ptr<registry::Type> raw_field_type = field_map.get(field_name);
if (type_arguments.size() > 0){
{
mlc::Array<mlc::String> parameter_names = TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
return substitute_semantic_type(raw_field_type, type_parameter_substitution_from_type_arguments(parameter_names, type_arguments));
}
}
return raw_field_type;
}

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = unwrap_shared_semantic_type(object_type);
mlc::String type_name = method_owner_type_name_from_semantic_type(inner_type);
if (type_name == mlc::String("")){
{
return std::make_shared<registry::Type>((registry::TUnknown{}));
}
}
mlc::String mangled = type_name + mlc::String("_") + method_name;
if (!TypeRegistry_has_fn(registry, mangled)){
{
return std::make_shared<registry::Type>((registry::TUnknown{}));
}
}
return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*TypeRegistry_fn_type(registry, mangled)))) { auto _v_tfn = std::get<registry::TFn>((*TypeRegistry_fn_type(registry, mangled))); auto [_w0, return_type] = _v_tfn; return return_type; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
}

} // namespace registry

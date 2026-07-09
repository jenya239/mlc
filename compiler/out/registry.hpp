#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "trait_param_expand.hpp"
namespace registry {

struct TI32;
struct TString;
struct TBool;
struct TUnit;
struct TI64;
struct TF64;
struct TU8;
struct TUsize;
struct TChar;
struct TArray;
struct TShared;
struct TNamed;
struct TGeneric;
struct TPair;
struct TTuple;
struct TFn;
struct TAssoc;
struct TUnknown;
using Type = std::variant<TI32, TString, TBool, TUnit, TI64, TF64, TU8, TUsize, TChar, TArray, TShared, TNamed, TGeneric, TPair, TTuple, TFn, TAssoc, TUnknown>;
struct TI32 {};
struct TString {};
struct TBool {};
struct TUnit {};
struct TI64 {};
struct TF64 {};
struct TU8 {};
struct TUsize {};
struct TChar {};
struct TArray {
  std::shared_ptr<Type> field0;
};
struct TShared {
  std::shared_ptr<Type> field0;
};
struct TNamed {
  mlc::String field0;
};
struct TGeneric {
  mlc::String field0;
  mlc::Array<std::shared_ptr<Type>> field1;
};
struct TPair {
  std::shared_ptr<Type> field0;
  std::shared_ptr<Type> field1;
};
struct TTuple {
  mlc::Array<std::shared_ptr<Type>> field0;
};
struct TFn {
  mlc::Array<std::shared_ptr<Type>> field0;
  std::shared_ptr<Type> field1;
};
struct TAssoc {
  mlc::String field0;
  mlc::String field1;
};
struct TUnknown {};
struct FunctionIndex {
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> function_types;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_type_parameter_names;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_parameter_names;
  mlc::HashMap<mlc::String, mlc::Array<int>> function_parameter_mutability_flags;
  mlc::HashMap<mlc::String, int> function_required_arity;
  mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>> function_trait_bounds;
};
struct AdtIndex {
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> constructor_types;
  mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>> constructor_parameters;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_type_parameter_names;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_phantom_type_params;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_variant_names;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_impls;
  mlc::HashMap<mlc::String, bool> private_constructors;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_assoc_types;
  mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>> assoc_type_bindings;
};
struct RecordIndex {
  mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>> field_types;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> record_field_names_ordered;
  mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>> record_literal_field_defaults;
};
struct TypeRegistry {
  FunctionIndex function_index;
  AdtIndex adt_index;
  RecordIndex record_index;
  mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> type_alias_type_parameter_names;
  mlc::HashMap<mlc::String, mlc::String> type_defining_path;
  mlc::HashMap<mlc::String, mlc::String> trait_defining_path;
};
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
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;
mlc::String TypeRegistry_defining_path_for_type(TypeRegistry self, mlc::String type_name) noexcept;
mlc::String TypeRegistry_defining_path_for_trait(TypeRegistry self, mlc::String trait_name) noexcept;
TypeRegistry empty_registry() noexcept;
void record_type_defining_path(TypeRegistry& registry, mlc::String type_name, mlc::String defining_path) noexcept;
void record_trait_defining_path(TypeRegistry& registry, mlc::String trait_name, mlc::String defining_path) noexcept;
std::shared_ptr<Type> primitive_type_from_name(mlc::String name) noexcept;
std::shared_ptr<ast::TypeExpr> type_expression_substitute_for_named(mlc::String name, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept;
std::shared_ptr<ast::TypeExpr> type_expression_substitute_named_parameter(std::shared_ptr<ast::TypeExpr> type_expression, mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> argument_expression) noexcept;
std::shared_ptr<ast::TypeExpr> substitute_type_expression_type_parameters(std::shared_ptr<ast::TypeExpr> type_expression, mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_expressions) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_parameter_substitution_from_type_arguments(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<Type>> type_arguments) noexcept;
bool resolution_stack_contains_name(mlc::Array<mlc::String> resolution_stack, mlc::String candidate_name) noexcept;
mlc::String type_alias_named_target(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::String resolve_nominal_type_name_for_fields(TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept;
std::shared_ptr<Type> resolve_named_type_with_registry(mlc::String name, TypeRegistry registry, mlc::Array<mlc::String> resolution_stack) noexcept;
std::shared_ptr<Type> type_from_annotation_generic_with_registry_and_stack(mlc::String generic_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments, TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept;
std::shared_ptr<Type> type_from_annotation_with_registry_and_stack(std::shared_ptr<ast::TypeExpr> type_expression, TypeRegistry type_registry, mlc::Array<mlc::String> resolution_stack) noexcept;
std::shared_ptr<Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, TypeRegistry registry) noexcept;
bool type_alias_has_cycle_from_name(TypeRegistry registry, mlc::String current_name, mlc::Array<mlc::String> resolution_stack) noexcept;
bool type_alias_has_cycle(TypeRegistry registry, mlc::String alias_name) noexcept;
std::shared_ptr<Type> type_from_named_annotation(mlc::String name) noexcept;
std::shared_ptr<Type> type_from_generic_annotation(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
TypeRegistry build_registry(ast::Program program) noexcept;
mlc::Array<mlc::String> accumulate_trait_associated_type_name(mlc::Array<mlc::String> accumulated_names, std::shared_ptr<ast::Decl> method) noexcept;
mlc::Array<mlc::String> trait_associated_type_names_for(TypeRegistry registry, mlc::String trait_name) noexcept;
bool trait_method_is_assoc_type_declaration(std::shared_ptr<ast::Decl> method) noexcept;
void register_trait_method_into_registry(TypeRegistry& registry, std::shared_ptr<ast::Decl> method) noexcept;
void register_decl_trait_into_registry(TypeRegistry& registry, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::String defining_path) noexcept;
mlc::String trait_base_name(mlc::String trait_name) noexcept;
void register_decl_extend_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;
void register_decl_fn_into_registry(TypeRegistry& registry, mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> trait_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type) noexcept;
mlc::String variant_name_of(std::shared_ptr<ast::TypeVariant> variant) noexcept;
void register_decl_type_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String defining_path) noexcept;
void register_decl_type_alias_into_registry(TypeRegistry& registry, mlc::String alias_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeExpr> type_expression, mlc::String defining_path) noexcept;
bool register_extend_method_into_registry(TypeRegistry& registry, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, std::shared_ptr<Type>>& assoc_type_bindings) noexcept;
bool register_decl_trait_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool register_decl_extend_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool register_decl_fn_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool register_decl_type_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool register_decl_type_alias_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool register_decl_exported_if(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
void registry_decl_noop() noexcept;
void register_decl_into(TypeRegistry& registry, std::shared_ptr<ast::Decl> declaration) noexcept;
bool type_param_in_annotation(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool type_param_in_annotation_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_annotation_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> compute_phantom_type_params(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
void register_variant_unit_into_registry(TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<Type> result_type, bool is_private) noexcept;
void register_variant_tuple_into_registry(TypeRegistry& registry, mlc::String variant_name, std::shared_ptr<Type> result_type, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, bool is_private) noexcept;
void register_variant_record_into_registry(TypeRegistry& registry, mlc::String type_name, mlc::String variant_name, std::shared_ptr<Type> result_type, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, bool is_private) noexcept;
void register_variant_into(TypeRegistry& registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
std::shared_ptr<Type> substitute_semantic_type_for_named(mlc::String name, std::shared_ptr<Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<Type>> substitution) noexcept;
std::shared_ptr<Type> substitute_semantic_type(std::shared_ptr<Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<Type>> substitution) noexcept;
std::shared_ptr<Type> unwrap_shared_semantic_type(std::shared_ptr<Type> object_type) noexcept;
mlc::String field_lookup_type_name_from_semantic_type(std::shared_ptr<Type> inner_type, TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<Type>> generic_type_arguments_from_semantic_type(std::shared_ptr<Type> inner_type) noexcept;
mlc::String method_owner_type_name_from_semantic_type(std::shared_ptr<Type> inner_type) noexcept;
std::shared_ptr<Type> field_type_from_object(std::shared_ptr<Type> object_type, mlc::String field_name, TypeRegistry registry) noexcept;
std::shared_ptr<Type> method_return_type_from_object(std::shared_ptr<Type> object_type, mlc::String method_name, TypeRegistry registry) noexcept;
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
bool TypeRegistry_is_private_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(TypeRegistry self, mlc::String trait_name) noexcept;
std::shared_ptr<Type> TypeRegistry_resolve_assoc(TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;
mlc::String TypeRegistry_defining_path_for_type(TypeRegistry self, mlc::String type_name) noexcept;
mlc::String TypeRegistry_defining_path_for_trait(TypeRegistry self, mlc::String trait_name) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

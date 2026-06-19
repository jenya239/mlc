#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "trait_param_expand.hpp"

namespace registry {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct TI32;struct TString;struct TBool;struct TUnit;struct TI64;struct TF64;struct TU8;struct TUsize;struct TChar;struct TArray;struct TShared;struct TNamed;struct TGeneric;struct TPair;struct TTuple;struct TFn;struct TAssoc;struct TUnknown;using Type = std::variant<TI32, TString, TBool, TUnit, TI64, TF64, TU8, TUsize, TChar, TArray, TShared, TNamed, TGeneric, TPair, TTuple, TFn, TAssoc, TUnknown>;struct TI32 {};struct TString {};struct TBool {};struct TUnit {};struct TI64 {};struct TF64 {};struct TU8 {};struct TUsize {};struct TChar {};struct TArray {std::shared_ptr<registry::Type> field0;};struct TShared {std::shared_ptr<registry::Type> field0;};struct TNamed {mlc::String field0;};struct TGeneric {mlc::String field0;mlc::Array<std::shared_ptr<registry::Type>> field1;};struct TPair {std::shared_ptr<registry::Type> field0;std::shared_ptr<registry::Type> field1;};struct TTuple {mlc::Array<std::shared_ptr<registry::Type>> field0;};struct TFn {mlc::Array<std::shared_ptr<registry::Type>> field0;std::shared_ptr<registry::Type> field1;};struct TAssoc {mlc::String field0;mlc::String field1;};struct TUnknown {};

struct FunctionIndex {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> function_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_type_parameter_names;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_parameter_names;mlc::HashMap<mlc::String, mlc::Array<int>> function_parameter_mutability_flags;mlc::HashMap<mlc::String, int> function_required_arity;mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>> function_trait_bounds;};

struct AdtIndex {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> constructor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> constructor_parameters;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_type_parameter_names;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_phantom_type_params;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_impls;mlc::HashMap<mlc::String, bool> private_constructors;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_assoc_types;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> assoc_type_bindings;};

struct RecordIndex {mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> field_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> record_field_names_ordered;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>> record_literal_field_defaults;};

struct TypeRegistry {registry::FunctionIndex function_index;registry::AdtIndex adt_index;registry::RecordIndex record_index;mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> type_alias_type_parameter_names;};

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

mlc::String resolve_nominal_type_name_for_fields(registry::TypeRegistry registry, mlc::String type_name, mlc::Array<mlc::String> resolution_stack) noexcept;

std::shared_ptr<registry::Type> type_from_annotation_with_registry(std::shared_ptr<ast::TypeExpr> type_expression, registry::TypeRegistry registry) noexcept;

bool type_alias_has_cycle(registry::TypeRegistry registry, mlc::String alias_name) noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program program) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

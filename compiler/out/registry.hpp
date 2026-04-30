#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "trait_param_expand.hpp"

namespace registry {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct TI32;struct TString;struct TBool;struct TUnit;struct TI64;struct TF64;struct TU8;struct TUsize;struct TChar;struct TArray;struct TShared;struct TNamed;struct TGeneric;struct TPair;struct TTuple;struct TFn;struct TAssoc;struct TUnknown;using Type = std::variant<TI32, TString, TBool, TUnit, TI64, TF64, TU8, TUsize, TChar, TArray, TShared, TNamed, TGeneric, TPair, TTuple, TFn, TAssoc, TUnknown>;struct TI32 {};struct TString {};struct TBool {};struct TUnit {};struct TI64 {};struct TF64 {};struct TU8 {};struct TUsize {};struct TChar {};struct TArray {std::shared_ptr<registry::Type> field0;};struct TShared {std::shared_ptr<registry::Type> field0;};struct TNamed {mlc::String field0;};struct TGeneric {mlc::String field0;mlc::Array<std::shared_ptr<registry::Type>> field1;};struct TPair {std::shared_ptr<registry::Type> field0;std::shared_ptr<registry::Type> field1;};struct TTuple {mlc::Array<std::shared_ptr<registry::Type>> field0;};struct TFn {mlc::Array<std::shared_ptr<registry::Type>> field0;std::shared_ptr<registry::Type> field1;};struct TAssoc {mlc::String field0;mlc::String field1;};struct TUnknown {};

struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> fn_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_type_parameter_names;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_parameter_names;mlc::HashMap<mlc::String, int> function_required_arity;mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>> function_trait_bounds;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> ctor_params;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_type_parameter_names;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_phantom_type_params;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> field_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_impls;mlc::Array<mlc::String> private_ctors;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_assoc_types;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> assoc_type_bindings;};

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

mlc::Array<mlc::String> TypeRegistry_trait_assoc_names(registry::TypeRegistry self, mlc::String trait_name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_resolve_assoc(registry::TypeRegistry self, mlc::String type_name, mlc::String trait_name, mlc::String assoc_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program program) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> method_return_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String method_name, registry::TypeRegistry registry) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

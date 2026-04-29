#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace registry {

struct TI32;
struct TString;
struct TBool;
struct TUnit;
struct TArray;
struct TShared;
struct TNamed;
struct TGeneric;
struct TPair;
struct TTuple;
struct TFn;
struct TUnknown;
using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TGeneric, TPair, TTuple, TFn, TUnknown>;
struct TI32 {};
struct TString {};
struct TBool {};
struct TUnit {};
struct TArray {std::shared_ptr<Type> field0;};
struct TShared {std::shared_ptr<Type> field0;};
struct TNamed {mlc::String field0;};
struct TGeneric {mlc::String field0;mlc::Array<std::shared_ptr<Type>> field1;};
struct TPair {std::shared_ptr<Type> field0;std::shared_ptr<Type> field1;};
struct TTuple {mlc::Array<std::shared_ptr<Type>> field0;};
struct TFn {mlc::Array<std::shared_ptr<Type>> field0;std::shared_ptr<Type> field1;};
struct TUnknown {};
struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<Type>> fn_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> function_type_parameter_names;mlc::HashMap<mlc::String, int> function_required_arity;mlc::HashMap<mlc::String, mlc::Array<mlc::Array<mlc::String>>> function_trait_bounds;mlc::HashMap<mlc::String, std::shared_ptr<Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>> ctor_params;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> algebraic_decl_type_parameter_names;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>> field_types;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> trait_impls;};
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<mlc::String> TypeRegistry_registered_function_type_parameter_names(TypeRegistry self, mlc::String function_name) noexcept;
mlc::Array<mlc::String> TypeRegistry_algebraic_decl_type_parameter_names_for(TypeRegistry self, mlc::String algebraic_type_name) noexcept;
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_type_implements_trait(TypeRegistry self, mlc::String type_name, mlc::String trait_name) noexcept;
mlc::Array<mlc::Array<mlc::String>> TypeRegistry_fn_trait_bounds(TypeRegistry self, mlc::String fn_name) noexcept;
int TypeRegistry_required_arity_for_fn(TypeRegistry self, mlc::String fn_name) noexcept;
TypeRegistry empty_registry() noexcept;
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
int required_arity_from_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
TypeRegistry build_registry(ast::Program prog) noexcept;
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<ast::Decl> decl) noexcept;
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
std::shared_ptr<Type> field_type_from_object(std::shared_ptr<Type> object_type, mlc::String field_name, TypeRegistry registry) noexcept;
std::shared_ptr<Type> method_return_type_from_object(std::shared_ptr<Type> object_type, mlc::String method_name, TypeRegistry registry) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

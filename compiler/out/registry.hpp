#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace registry {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct TI32;struct TString;struct TBool;struct TUnit;struct TArray;struct TShared;struct TNamed;struct TGeneric;struct TFn;struct TUnknown;using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TGeneric, TFn, TUnknown>;struct TI32 {};struct TString {};struct TBool {};struct TUnit {};struct TArray {std::shared_ptr<registry::Type> field0;};struct TShared {std::shared_ptr<registry::Type> field0;};struct TNamed {mlc::String field0;};struct TGeneric {mlc::String field0;mlc::Array<std::shared_ptr<registry::Type>> field1;};struct TFn {mlc::Array<std::shared_ptr<registry::Type>> field0;std::shared_ptr<registry::Type> field1;};struct TUnknown {};

struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> fn_types;mlc::HashMap<mlc::String, int> function_type_parameter_counts;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> ctor_params;mlc::HashMap<mlc::String, bool> polymorphic_constructors;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> field_types;};

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept;

int TypeRegistry_function_type_parameter_count(registry::TypeRegistry self, mlc::String name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_constructor_is_polymorphic_variant(registry::TypeRegistry self, mlc::String constructor_name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program prog) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

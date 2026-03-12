#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace registry {

struct Expr;
struct Stmt;

struct TI32;struct TString;struct TBool;struct TUnit;struct TArray;struct TShared;struct TNamed;struct TFn;struct TUnknown;using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TFn, TUnknown>;struct TI32 {};struct TString {};struct TBool {};struct TUnit {};struct TArray {std::shared_ptr<registry::Type> field0;};struct TShared {std::shared_ptr<registry::Type> field0;};struct TNamed {mlc::String field0;};struct TFn {mlc::Array<std::shared_ptr<registry::Type>> field0;std::shared_ptr<registry::Type> field1;};struct TUnknown {};

struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> fn_types;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<registry::Type>>> ctor_params;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>> field_types;};

std::shared_ptr<registry::Type> TypeRegistry_fn_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_fn(registry::TypeRegistry self, mlc::String name) noexcept;

std::shared_ptr<registry::Type> TypeRegistry_ctor_type(registry::TypeRegistry self, mlc::String name) noexcept;

bool TypeRegistry_has_ctor(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> TypeRegistry_ctor_params_for(registry::TypeRegistry self, mlc::String name) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> TypeRegistry_fields_for(registry::TypeRegistry self, mlc::String type_name) noexcept;

bool TypeRegistry_has_fields(registry::TypeRegistry self, mlc::String type_name) noexcept;

registry::TypeRegistry empty_registry() noexcept;

std::shared_ptr<registry::Type> type_from_annotation(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

registry::TypeRegistry build_registry(ast::Program prog) noexcept;

} // namespace registry

#endif // REGISTRY_HPP

#ifndef NATIVE_HPP
#define NATIVE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "value.hpp"

namespace native {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_call(mlc::String name, mlc::Array<value::VmValue> arguments) noexcept;

value::VmValue vm_variant_ctor(mlc::String tag, mlc::Array<value::VmValue> arguments) noexcept;

} // namespace native

#endif // NATIVE_HPP

#ifndef RUNTIME_HPP
#define RUNTIME_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "native.hpp"

namespace runtime {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct VmCallNative {mlc::String field0;};struct VmCallVariantCtor {mlc::String field0;};struct VmCallUser {mir_types::MirFunction field0;};using VmCallDispatch = std::variant<VmCallNative, VmCallVariantCtor, VmCallUser>;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept;

bool vm_is_native_callee(mlc::String name) noexcept;

bool vm_is_variant_ctor_name(mlc::String name) noexcept;

mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction _function, mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(runtime::VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept;

} // namespace runtime

#endif // RUNTIME_HPP

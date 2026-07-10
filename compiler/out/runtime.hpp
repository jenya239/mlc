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

struct VmCallNative;
struct VmCallUser;
using VmCallDispatch = std::variant<VmCallNative, VmCallUser>;
struct VmCallNative {
  mlc::String field0;
};
struct VmCallUser {
  mir_types::MirFunction field0;
};
ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept;
bool vm_is_native_callee(mlc::String name) noexcept;
ast::Result<VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept;
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_value_from_param_default(mir_types::MirParam parameter) noexcept;
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_bind_call_arguments(mir_types::MirFunction function, mlc::Array<value::VmValue> arguments) noexcept;
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept;

} // namespace runtime

#endif // RUNTIME_HPP

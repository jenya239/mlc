#include "runtime.hpp"

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "native.hpp"

namespace runtime {

using namespace ast;
using namespace mir_types;
using namespace value;
using namespace frame;
using namespace native;
using namespace ast_tokens;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept;

bool vm_is_native_callee(mlc::String name) noexcept;

bool vm_is_variant_ctor_name(mlc::String name) noexcept;

mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction _function, mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(runtime::VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept{
int module_index = 0;
while (module_index < program.modules.size()){
{
mir_types::MirModule mir_module = program.modules[module_index];
int function_index = 0;
while (function_index < mir_module.functions.size()){
{
mir_types::MirFunction function = mir_module.functions[function_index];
if (function.name == name){
{
return ast::Ok<mir_types::MirFunction>(function);
}
}
function_index = function_index + 1;
}
}
module_index = module_index + 1;
}
}
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: function ") + name + mlc::String(" not found")});
}

bool vm_is_native_callee(mlc::String name) noexcept{return name == mlc::String("println") || name == mlc::String("__mir_array_empty") || name == mlc::String("__mir_array_push") || name == mlc::String("__mir_array_length") || name == mlc::String("__mir_array_get") || name == mlc::String("__mir_map_empty") || name == mlc::String("__mir_map_set") || name == mlc::String("__mir_map_get") || name == mlc::String("__mir_map_length") || name == mlc::String("__mir_string_length") || name == mlc::String("__mir_length") || name == mlc::String("__mir_variant_is") || name == mlc::String("__mir_variant_field");}

bool vm_is_variant_ctor_name(mlc::String name) noexcept{return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}

mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction _function, mlc::Array<value::VmValue> arguments) noexcept{
mlc::Array<value::VmValue> locals = {};
int index = 0;
while (index < arguments.size()){
{
locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, arguments[index]);
index = index + 1;
}
}
return locals;
}

ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept{
return vm_is_native_callee(callee_name) ? ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>>(ast::Ok<runtime::VmCallDispatch>(runtime::VmCallNative(callee_name))) : ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>>(vm_is_variant_ctor_name(callee_name) ? ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>>(ast::Ok<runtime::VmCallDispatch>(runtime::VmCallVariantCtor(callee_name))) : ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<runtime::VmCallDispatch, mlc::Array<mlc::String>> { auto [function] = ok; return ast::Ok<runtime::VmCallDispatch>(runtime::VmCallUser(function)); }
}, vm_find_function(program, callee_name))));
}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(runtime::VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept{return std::visit(overloaded{
  [&](const VmCallNative& vmcallnative) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [name] = vmcallnative; return native::vm_native_call(name, argument_values); },
  [&](const VmCallVariantCtor& vmcallvariantctor) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [name] = vmcallvariantctor; return ast::Ok<value::VmValue>(native::vm_variant_ctor(name, argument_values)); },
  [&](const VmCallUser& vmcalluser) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmcalluser; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: user call ") + callee_name + mlc::String(" must enter frame stack")}); }
}, dispatch);}

} // namespace runtime

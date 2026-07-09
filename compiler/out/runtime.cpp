#define main mlc_user_main
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

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept{
  auto module_index = 0;
  while ((module_index < program.modules.length()))   {
    auto mir_module = program.modules[module_index];
    auto function_index = 0;
    while ((function_index < mir_module.functions.length()))     {
      auto function = mir_module.functions[function_index];
      if ((function.name == name))       {
        return ast::Ok<mir_types::MirFunction>{function};
      }
      (function_index = (function_index + 1));
    }
    (module_index = (module_index + 1));
  }
  return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: function ", 13) + mlc::to_string(name)) + mlc::String(" not found", 10))}};
}
bool vm_is_native_callee(mlc::String name) noexcept{
  return (((((((((((((name == mlc::String("println", 7)) || (name == mlc::String("__mir_array_empty", 17))) || (name == mlc::String("__mir_array_push", 16))) || (name == mlc::String("__mir_array_length", 18))) || (name == mlc::String("__mir_array_get", 15))) || (name == mlc::String("__mir_map_empty", 15))) || (name == mlc::String("__mir_map_set", 13))) || (name == mlc::String("__mir_map_get", 13))) || (name == mlc::String("__mir_map_length", 16))) || (name == mlc::String("__mir_string_length", 19))) || (name == mlc::String("__mir_length", 12))) || (name == mlc::String("__mir_variant_is", 16))) || (name == mlc::String("__mir_variant_field", 19)));
}
bool vm_is_variant_ctor_name(mlc::String name) noexcept{
  return (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
}
mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction _function, mlc::Array<value::VmValue> arguments) noexcept{
  auto locals = mlc::Array<value::VmValue>{};
  auto index = 0;
  while ((index < arguments.length()))   {
    (locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, arguments[index]));
    (index = (index + 1));
  }
  return locals;
}
ast::Result<VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept{
  if (vm_is_native_callee(callee_name))   {
    return ast::Ok<VmCallDispatch>{VmCallNative{callee_name}};
  } else if (vm_is_variant_ctor_name(callee_name))   {
    return ast::Ok<VmCallDispatch>{VmCallVariantCtor{callee_name}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<VmCallDispatch, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<VmCallDispatch, mlc::Array<mlc::String>> { auto [function] = ok; return ast::Ok<VmCallDispatch>{VmCallUser{function}}; }
}, vm_find_function(program, callee_name));
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept{
  return std::visit(overloaded{[&](const VmCallNative& vmCallNative) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [name] = vmCallNative; return native::vm_native_call(name, argument_values); },
[&](const VmCallVariantCtor& vmCallVariantCtor) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [name] = vmCallVariantCtor; return ast::Ok<value::VmValue>{native::vm_variant_ctor(name, argument_values)}; },
[&](const VmCallUser& vmCallUser) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [__0] = vmCallUser; return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: user call ", 14) + mlc::to_string(callee_name)) + mlc::String(" must enter frame stack", 23))}}; }
}, dispatch);
}

} // namespace runtime

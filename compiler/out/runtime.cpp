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
      (function_index = mlc::arith::checked_add(function_index, 1));
    }
    (module_index = mlc::arith::checked_add(module_index, 1));
  }
  return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: function ", 13) + mlc::to_string(name)) + mlc::String(" not found", 10))}};
}
bool vm_is_native_callee(mlc::String name) noexcept{
  return (((((((((((((((((((((name == mlc::String("println", 7)) || (name == mlc::String("__mir_array_empty", 17))) || (name == mlc::String("__mir_array_push", 16))) || (name == mlc::String("__mir_array_pop", 15))) || (name == mlc::String("__mir_array_length", 18))) || (name == mlc::String("__mir_array_get", 15))) || (name == mlc::String("__mir_map_empty", 15))) || (name == mlc::String("__mir_map_set", 13))) || (name == mlc::String("__mir_map_get", 13))) || (name == mlc::String("__mir_map_length", 16))) || (name == mlc::String("__mir_string_length", 19))) || (name == mlc::String("__mir_string_contains", 21))) || (name == mlc::String("__mir_length", 12))) || (name == mlc::String("__mir_variant_is", 16))) || (name == mlc::String("__mir_variant_field", 19))) || (name == mlc::String("__mir_variant_new", 17))) || (name == mlc::String("__mir_record_new", 16))) || (name == mlc::String("__mir_record_field", 18))) || (name == mlc::String("__mir_shared_new", 16))) || (name == mlc::String("__mir_result_is_err", 19))) || (name == mlc::String("__mir_result_ok_value", 21)));
}
ast::Result<VmCallDispatch, mlc::Array<mlc::String>> vm_resolve_call(mir_types::MirProgram program, mlc::String callee_name) noexcept{
  if (vm_is_native_callee(callee_name))   {
    return ast::Ok<VmCallDispatch>{VmCallNative{callee_name}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<VmCallDispatch, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<VmCallDispatch, mlc::Array<mlc::String>> { auto [function] = ok; return ast::Ok<VmCallDispatch>{VmCallUser{function}}; }
}, vm_find_function(program, callee_name));
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_value_from_param_default(mir_types::MirParam parameter) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirParamDefaultNone& mirParamDefaultNone) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: missing argument for ", 25) + mlc::to_string(parameter.name)) + mlc::String("", 0))}}; },
[&](const mir_types::MirParamDefaultInt& mirParamDefaultInt) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirParamDefaultInt; return ast::Ok<value::VmValue>{value::VmI32{value}}; },
[&](const mir_types::MirParamDefaultBool& mirParamDefaultBool) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirParamDefaultBool; return ast::Ok<value::VmValue>{value::VmBool{value}}; },
[&](const mir_types::MirParamDefaultStr& mirParamDefaultStr) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirParamDefaultStr; return ast::Ok<value::VmValue>{value::VmString{value}}; },
[&](const mir_types::MirParamDefaultUnit& mirParamDefaultUnit) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Ok<value::VmValue>{value::VmUnit{}}; }
}, parameter.default_value);
}
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_bind_call_arguments(mir_types::MirFunction function, mlc::Array<value::VmValue> arguments) noexcept{
  auto locals = mlc::Array<value::VmValue>{};
  auto index = 0;
  auto failed = false;
  auto errors = mlc::Array<mlc::String>{};
  while (((index < function.params.length()) && (!failed)))   {
    if ((index < arguments.length()))     {
      (locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, arguments[index]));
    } else     {
      std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> void { auto [default_errors] = err; [&]() {
(failed = true);
(errors = default_errors);
return std::make_tuple();
}(); },
[&](const ast::Ok<value::VmValue>& ok) -> void { auto [default_value] = ok; [&]() {
(locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, default_value));
return std::make_tuple();
}(); }
}, vm_value_from_param_default(function.params[index]));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  if (failed)   {
    return ast::Err<mlc::Array<mlc::String>>{errors};
  } else   {
    return ast::Ok<mlc::Array<value::VmValue>>{locals};
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_dispatch_call(VmCallDispatch dispatch, mlc::String callee_name, mlc::Array<value::VmValue> argument_values) noexcept{
  return std::visit(overloaded{[&](const VmCallNative& vmCallNative) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [name] = vmCallNative; return native::vm_native_call(name, argument_values); },
[&](const VmCallUser& vmCallUser) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [__0] = vmCallUser; return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: user call ", 14) + mlc::to_string(callee_name)) + mlc::String(" must enter frame stack", 23))}}; }
}, dispatch);
}

} // namespace runtime

#define main mlc_user_main
#include "interpreter.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "lower_program.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "execute.hpp"
#include "runtime.hpp"
#include "outcome.hpp"

namespace interpreter {

using namespace ast;
using namespace semantic_ir;
using namespace lower_program;
using namespace mir_types;
using namespace value;
using namespace execute;
using namespace runtime;
using namespace outcome;
using namespace ast_tokens;

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [function] = ok; return std::visit(overloaded{[&](const outcome::VmRunReturn& vmRunReturn) -> ast::Result<int, mlc::Array<mlc::String>> { auto [value] = vmRunReturn; return std::visit(overloaded{[&](const ast::Ok<int>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [code] = ok; return ast::Ok<int>{code}; },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [__0] = err; return ast::Ok<int>{0}; }
}, value::vm_value_as_i32(value)); },
[&](const outcome::VmRunFailed& vmRunFailed) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = vmRunFailed; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const outcome::VmRunContinue& vmRunContinue) -> ast::Result<int, mlc::Array<mlc::String>> { auto [__0] = vmRunContinue; return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: main returned without value", 31)}}; }
}, execute::vm_run_function(program, function, mlc::Array<value::VmValue>{}, trace_enabled)); }
}, runtime::vm_find_function(program, mlc::String("main", 4)));
}
ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::MirProgram>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [program] = ok; return interpret_mir_program(program, trace_enabled); }
}, lower_program::build_mir_program_from_semantic_items_checked(items));
}
ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept{
  auto program = mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mlc::Array<mir_types::MirFunction>{function}}}};
  return std::visit(overloaded{[&](const outcome::VmRunReturn& vmRunReturn) -> ast::Result<value::VmValue, mlc::String> { auto [value] = vmRunReturn; return ast::Ok<value::VmValue>{value}; },
[&](const outcome::VmRunFailed& vmRunFailed) -> ast::Result<value::VmValue, mlc::String> { auto [errors] = vmRunFailed; return [&]() -> ast::Result<value::VmValue, mlc::String> {
  if ((errors.length() > 0))   {
    return ast::Err<mlc::String>{errors[0]};
  } else   {
    return ast::Err<mlc::String>{mlc::String("vm: interpreter failed", 22)};
  }
}(); },
[&](const outcome::VmRunContinue& vmRunContinue) -> ast::Result<value::VmValue, mlc::String> { auto [__0] = vmRunContinue; return ast::Err<mlc::String>{mlc::String("vm: function did not return", 27)}; }
}, execute::vm_run_function(program, function, mlc::Array<value::VmValue>{}, false));
}

} // namespace interpreter

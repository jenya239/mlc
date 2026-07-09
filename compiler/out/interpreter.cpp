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

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept;

ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept;

ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept;

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [function] = ok; return std::visit(overloaded{
  [&](const VmRunReturn& vmrunreturn) -> ast::Result<int, mlc::Array<mlc::String>> { auto [value] = vmrunreturn; return std::visit(overloaded{
  [&](const ast::Ok<int>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [code] = ok; return ast::Ok<int>(code); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [_w0] = err; return ast::Ok<int>(0); }
}, value::vm_value_as_i32(value)); },
  [&](const VmRunFailed& vmrunfailed) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = vmrunfailed; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const VmRunContinue& vmruncontinue) -> ast::Result<int, mlc::Array<mlc::String>> { auto [_w0] = vmruncontinue; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: main returned without value")}); }
}, execute::vm_run_function(program, function, {}, trace_enabled)); }
}, runtime::vm_find_function(program, mlc::String("main")));}

ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::MirProgram>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [program] = ok; return interpret_mir_program(program, trace_enabled); }
}, lower_program::build_mir_program_from_semantic_items_checked(items));}

ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept{
mir_types::MirProgram program = mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mlc::Array<mir_types::MirFunction>{function}}}};
return std::visit(overloaded{
  [&](const VmRunReturn& vmrunreturn) -> ast::Result<value::VmValue, mlc::String> { auto [value] = vmrunreturn; return ast::Ok<value::VmValue>(value); },
  [&](const VmRunFailed& vmrunfailed) -> ast::Result<value::VmValue, mlc::String> { auto [errors] = vmrunfailed; return errors.size() > 0 ? ast::Result<value::VmValue, mlc::String>(ast::Err<mlc::String>(errors[0])) : ast::Result<value::VmValue, mlc::String>(ast::Err<mlc::String>(mlc::String("vm: interpreter failed"))); },
  [&](const VmRunContinue& vmruncontinue) -> ast::Result<value::VmValue, mlc::String> { auto [_w0] = vmruncontinue; return ast::Err<mlc::String>(mlc::String("vm: function did not return")); }
}, execute::vm_run_function(program, function, {}, false));
}

} // namespace interpreter

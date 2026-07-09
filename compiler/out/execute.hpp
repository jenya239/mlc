#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "mir_eval.hpp"
#include "runtime.hpp"
#include "outcome.hpp"

namespace execute {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

outcome::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept;

outcome::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept;

} // namespace execute

#endif // EXECUTE_HPP

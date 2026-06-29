#ifndef OUTCOME_HPP
#define OUTCOME_HPP

#include "mlc.hpp"
#include <variant>

#include "value.hpp"
#include "frame.hpp"

namespace outcome {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct VmRunContinue {mlc::Array<frame::VmFrame> field0;};struct VmRunReturn {value::VmValue field0;};struct VmRunFailed {mlc::Array<mlc::String> field0;};using VmRunOutcome = std::variant<VmRunContinue, VmRunReturn, VmRunFailed>;

} // namespace outcome

#endif // OUTCOME_HPP

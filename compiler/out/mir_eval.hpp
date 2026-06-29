#ifndef MIR_EVAL_HPP
#define MIR_EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"

namespace mir_eval {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept;

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept;

} // namespace mir_eval

#endif // MIR_EVAL_HPP

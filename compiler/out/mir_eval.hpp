#ifndef MIR_EVAL_HPP
#define MIR_EVAL_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
namespace mir_eval {

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept;
bool vm_values_equal(value::VmValue left, value::VmValue right) noexcept;
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary_i32(mlc::String operation, int left, int right) noexcept;
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept;
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept;
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept;
mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept;
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept;

} // namespace mir_eval

#endif // MIR_EVAL_HPP

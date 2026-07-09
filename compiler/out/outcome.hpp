#ifndef OUTCOME_HPP
#define OUTCOME_HPP

#include "mlc.hpp"
#include <variant>
#include "value.hpp"
#include "frame.hpp"
namespace outcome {

struct VmRunContinue;
struct VmRunReturn;
struct VmRunFailed;
using VmRunOutcome = std::variant<VmRunContinue, VmRunReturn, VmRunFailed>;
struct VmRunContinue {
  mlc::Array<frame::VmFrame> field0;
};
struct VmRunReturn {
  value::VmValue field0;
};
struct VmRunFailed {
  mlc::Array<mlc::String> field0;
};

} // namespace outcome

#endif // OUTCOME_HPP

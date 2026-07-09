#ifndef MIR_PASSES_HPP
#define MIR_PASSES_HPP

#include "mlc.hpp"
#include <variant>
#include "mir_types.hpp"
#include "const_fold.hpp"
#include "simplify_cfg.hpp"
namespace mir_passes {

mir_types::MirFunction run_mir_passes_on_function(mir_types::MirFunction function) noexcept;
mlc::Array<mir_types::MirFunction> run_mir_passes_on_functions(mlc::Array<mir_types::MirFunction> functions) noexcept;
mlc::Array<mir_types::MirModule> run_mir_passes_on_modules(mlc::Array<mir_types::MirModule> modules) noexcept;
mir_types::MirProgram run_mir_passes_on_program(mir_types::MirProgram program) noexcept;

} // namespace mir_passes

#endif // MIR_PASSES_HPP

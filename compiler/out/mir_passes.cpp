#define main mlc_user_main
#include "mir_passes.hpp"

#include "mir_types.hpp"
#include "const_fold.hpp"
#include "simplify_cfg.hpp"

namespace mir_passes {

using namespace mir_types;
using namespace const_fold;
using namespace simplify_cfg;

mir_types::MirFunction run_mir_passes_on_function(mir_types::MirFunction function) noexcept{
  return simplify_cfg::simplify_cfg_mir_function(const_fold::const_fold_mir_function(function));
}
mlc::Array<mir_types::MirFunction> run_mir_passes_on_functions(mlc::Array<mir_types::MirFunction> functions) noexcept{
  auto optimized = mlc::Array<mir_types::MirFunction>{};
  auto index = 0;
  while ((index < functions.length()))   {
    optimized.push_back(run_mir_passes_on_function(functions[index]));
    (index = (index + 1));
  }
  return optimized;
}
mlc::Array<mir_types::MirModule> run_mir_passes_on_modules(mlc::Array<mir_types::MirModule> modules) noexcept{
  auto optimized = mlc::Array<mir_types::MirModule>{};
  auto index = 0;
  while ((index < modules.length()))   {
    auto mir_module = modules[index];
    optimized.push_back(mir_types::MirModule{run_mir_passes_on_functions(mir_module.functions)});
    (index = (index + 1));
  }
  return optimized;
}
mir_types::MirProgram run_mir_passes_on_program(mir_types::MirProgram program) noexcept{
  return mir_types::MirProgram{run_mir_passes_on_modules(program.modules)};
}

} // namespace mir_passes

#ifndef MIR_PASSES_HPP
#define MIR_PASSES_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"
#include "const_fold.hpp"
#include "simplify_cfg.hpp"

namespace mir_passes {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mir_types::MirFunction run_mir_passes_on_function(mir_types::MirFunction function) noexcept;

mir_types::MirProgram run_mir_passes_on_program(mir_types::MirProgram program) noexcept;

} // namespace mir_passes

#endif // MIR_PASSES_HPP

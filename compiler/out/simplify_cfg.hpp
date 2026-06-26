#ifndef SIMPLIFY_CFG_HPP
#define SIMPLIFY_CFG_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"
#include "mir_ids.hpp"

namespace simplify_cfg {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mir_types::MirFunction simplify_cfg_mir_function(mir_types::MirFunction function) noexcept;

} // namespace simplify_cfg

#endif // SIMPLIFY_CFG_HPP

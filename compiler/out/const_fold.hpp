#ifndef CONST_FOLD_HPP
#define CONST_FOLD_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"

namespace const_fold {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mir_types::MirFunction const_fold_mir_function(mir_types::MirFunction function) noexcept;

} // namespace const_fold

#endif // CONST_FOLD_HPP

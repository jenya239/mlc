#ifndef LOWER_PROGRAM_HPP
#define LOWER_PROGRAM_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "mir_types.hpp"
#include "lower_fn.hpp"
#include "mir_passes.hpp"

namespace lower_program {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

} // namespace lower_program

#endif // LOWER_PROGRAM_HPP

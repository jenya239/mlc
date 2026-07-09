#ifndef MIR_DUMP_HPP
#define MIR_DUMP_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"

namespace mir_dump {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String print_mir_block(mir_types::MirBlock block) noexcept;

mlc::String print_mir_function(mir_types::MirFunction function) noexcept;

mlc::String print_mir_module(mir_types::MirModule mir_module) noexcept;

mlc::String print_mir_program(mir_types::MirProgram program) noexcept;

} // namespace mir_dump

#endif // MIR_DUMP_HPP

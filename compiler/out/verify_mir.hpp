#ifndef VERIFY_MIR_HPP
#define VERIFY_MIR_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"

namespace verify_mir {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<mlc::String> verify_mir_function(mir_types::MirFunction function) noexcept;

mlc::Array<mlc::String> verify_mir_module(mir_types::MirModule mir_module) noexcept;

mlc::Array<mlc::String> verify_mir_program(mir_types::MirProgram program) noexcept;

} // namespace verify_mir

#endif // VERIFY_MIR_HPP

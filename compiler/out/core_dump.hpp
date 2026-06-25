#ifndef CORE_DUMP_HPP
#define CORE_DUMP_HPP

#include "mlc.hpp"
#include <variant>

#include "core.hpp"

namespace core_dump {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String print_core_basic_block(core::CoreBasicBlock block) noexcept;

mlc::String print_core_function(core::CoreFunction function) noexcept;

mlc::String print_core_module(core::CoreModule core_module) noexcept;

} // namespace core_dump

#endif // CORE_DUMP_HPP

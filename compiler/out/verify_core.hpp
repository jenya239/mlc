#ifndef VERIFY_CORE_HPP
#define VERIFY_CORE_HPP

#include "mlc.hpp"
#include <variant>

#include "core.hpp"

namespace verify_core {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<mlc::String> verify_core_function(core::CoreFunction function) noexcept;

mlc::Array<mlc::String> verify_core_module(core::CoreModule core_module) noexcept;

} // namespace verify_core

#endif // VERIFY_CORE_HPP

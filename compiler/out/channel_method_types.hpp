#ifndef CHANNEL_METHOD_TYPES_HPP
#define CHANNEL_METHOD_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace channel_method_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool is_channel_method(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> channel_method_result_type(std::shared_ptr<registry::Type> channel_type, mlc::String method_name) noexcept;

} // namespace channel_method_types

#endif // CHANNEL_METHOD_TYPES_HPP

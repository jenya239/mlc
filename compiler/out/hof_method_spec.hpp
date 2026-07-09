#ifndef HOF_METHOD_SPEC_HPP
#define HOF_METHOD_SPEC_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "semantic_type_structure.hpp"

namespace hof_method_spec {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool is_result_option_hof_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

bool is_array_hof_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept;

} // namespace hof_method_spec

#endif // HOF_METHOD_SPEC_HPP

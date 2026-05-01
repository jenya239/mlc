#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "semantic_ir.hpp"

namespace transform {

std::shared_ptr<semantic_ir::SExpr> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SExpr> typed_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP

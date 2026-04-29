#ifndef INFER_EXPR_IDENT_HPP
#define INFER_EXPR_IDENT_HPP

#include "mlc.hpp"
#include <variant>

#include "check_context.hpp"
#include "infer_result.hpp"
#include "registry.hpp"

namespace infer_expr_ident {

infer_result::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept;

} // namespace infer_expr_ident

#endif // INFER_EXPR_IDENT_HPP

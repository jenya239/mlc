#ifndef INFER_LITERALS_HPP
#define INFER_LITERALS_HPP

#include "mlc.hpp"
#include <variant>

#include "infer_result.hpp"

namespace infer_literals {

infer_result::InferResult infer_expr_integer_literal() noexcept;
infer_result::InferResult infer_expr_string_literal() noexcept;
infer_result::InferResult infer_expr_boolean_literal() noexcept;
infer_result::InferResult infer_expr_unit_literal() noexcept;
infer_result::InferResult infer_expr_extern_placeholder() noexcept;

} // namespace infer_literals

#endif // INFER_LITERALS_HPP

#include "infer_literals.hpp"

#include "infer_result.hpp"

namespace infer_literals {

using namespace infer_result;

infer_result::InferResult infer_expr_integer_literal() noexcept;

infer_result::InferResult infer_expr_string_literal() noexcept;

infer_result::InferResult infer_expr_boolean_literal() noexcept;

infer_result::InferResult infer_expr_unit_literal() noexcept;

infer_result::InferResult infer_expr_f64_literal() noexcept;

infer_result::InferResult infer_expr_i64_literal() noexcept;

infer_result::InferResult infer_expr_u8_literal() noexcept;

infer_result::InferResult infer_expr_usize_literal() noexcept;

infer_result::InferResult infer_expr_char_literal() noexcept;

infer_result::InferResult infer_expr_extern_placeholder() noexcept;

infer_result::InferResult infer_expr_integer_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TI32{})));}

infer_result::InferResult infer_expr_string_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TString{})));}

infer_result::InferResult infer_expr_boolean_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TBool{})));}

infer_result::InferResult infer_expr_unit_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

infer_result::InferResult infer_expr_f64_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TF64{})));}

infer_result::InferResult infer_expr_i64_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TI64{})));}

infer_result::InferResult infer_expr_u8_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TU8{})));}

infer_result::InferResult infer_expr_usize_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUsize{})));}

infer_result::InferResult infer_expr_char_literal() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TChar{})));}

infer_result::InferResult infer_expr_extern_placeholder() noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

} // namespace infer_literals

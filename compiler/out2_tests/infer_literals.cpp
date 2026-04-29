#define main mlc_user_main
#include "infer_literals.hpp"

namespace infer_literals {

infer_result::InferResult infer_expr_integer_literal() noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TI32{}));
}
infer_result::InferResult infer_expr_string_literal() noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TString{}));
}
infer_result::InferResult infer_expr_boolean_literal() noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TBool{}));
}
infer_result::InferResult infer_expr_unit_literal() noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_extern_placeholder() noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnit{}));
}

} // namespace infer_literals

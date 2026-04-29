#ifndef INFER_FOR_SUPPORT_HPP
#define INFER_FOR_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "type_diagnostics.hpp"

namespace infer_for_support {

infer_result::InferResult infer_for_iterator_with_range_rules(infer_result::InferResult iterator_base, std::shared_ptr<ast::Expr> iterator_expression) noexcept;
std::shared_ptr<registry::Type> element_type_for_for_iterator(std::shared_ptr<registry::Type> iterator_type) noexcept;

} // namespace infer_for_support

#endif // INFER_FOR_SUPPORT_HPP

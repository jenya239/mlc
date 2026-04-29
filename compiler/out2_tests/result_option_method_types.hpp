#ifndef RESULT_OPTION_METHOD_TYPES_HPP
#define RESULT_OPTION_METHOD_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace result_option_method_types {

bool is_result_generic(std::shared_ptr<registry::Type> t) noexcept;
bool is_option_generic(std::shared_ptr<registry::Type> t) noexcept;
bool should_skip_array_hof_for_receiver(std::shared_ptr<registry::Type> t) noexcept;
bool is_result_combinator_method(mlc::String method_name) noexcept;
bool is_option_combinator_method(mlc::String method_name) noexcept;
bool should_infer_result_option_combinator(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
int result_option_hof_expected_argument_count(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
std::shared_ptr<registry::Type> result_ok_type(std::shared_ptr<registry::Type> result_type) noexcept;
std::shared_ptr<registry::Type> result_err_type(std::shared_ptr<registry::Type> result_type) noexcept;
std::shared_ptr<registry::Type> option_inner_type(std::shared_ptr<registry::Type> opt_type) noexcept;
std::shared_ptr<registry::Type> result_option_hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> arg_inferred_types) noexcept;
void __skip__() noexcept;

} // namespace result_option_method_types

#endif // RESULT_OPTION_METHOD_TYPES_HPP

#include "hof_method_spec.hpp"

#include "registry.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "semantic_type_structure.hpp"

namespace hof_method_spec {

using namespace registry;
using namespace array_method_types;
using namespace result_option_method_types;
using namespace semantic_type_structure;

bool is_result_option_hof_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

bool is_array_hof_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept;

bool is_result_option_hof_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return result_option_method_types::should_infer_result_option_combinator(receiver_type, method_name);}

bool is_array_hof_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return array_method_types::is_array_hof_method(method_name) && semantic_type_structure::type_is_array(receiver_type);}

std::shared_ptr<registry::Type> hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept{return is_result_option_hof_method(receiver_type, method_name) ? result_option_method_types::result_option_hof_call_result_type(receiver_type, method_name, argument_inferred_types) : is_array_hof_method_on_receiver(receiver_type, method_name) ? array_method_types::array_hof_call_result_type(semantic_type_structure::array_element_type_from_array_type(receiver_type), method_name, argument_inferred_types) : std::make_shared<registry::Type>((registry::TUnknown{}));}

} // namespace hof_method_spec

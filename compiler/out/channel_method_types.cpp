#include "channel_method_types.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace channel_method_types {

using namespace registry;
using namespace semantic_type_structure;

bool is_channel_method(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> channel_method_result_type(std::shared_ptr<registry::Type> channel_type, mlc::String method_name) noexcept;

bool is_channel_method(mlc::String method_name) noexcept{return method_name == mlc::String("send") || method_name == mlc::String("recv");}

std::shared_ptr<registry::Type> channel_method_result_type(std::shared_ptr<registry::Type> channel_type, mlc::String method_name) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::channel_element_type_from_channel_type(channel_type);
return method_name == mlc::String("send") ? std::make_shared<registry::Type>((registry::TBool{})) : method_name == mlc::String("recv") ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{element_type})) : std::make_shared<registry::Type>((registry::TUnit{}));
}

} // namespace channel_method_types

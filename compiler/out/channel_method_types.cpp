#define main mlc_user_main
#include "channel_method_types.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace channel_method_types {

using namespace registry;
using namespace semantic_type_structure;

bool is_channel_method(mlc::String method_name) noexcept{
  return ((method_name == mlc::String("send", 4)) || (method_name == mlc::String("recv", 4)));
}
std::shared_ptr<registry::Type> channel_method_result_type(std::shared_ptr<registry::Type> channel_type, mlc::String method_name) noexcept{
  auto element_type = semantic_type_structure::channel_element_type_from_channel_type(channel_type);
  if ((method_name == mlc::String("send", 4)))   {
    return std::make_shared<registry::Type>(registry::TBool{});
  } else if ((method_name == mlc::String("recv", 4)))   {
    return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{element_type}});
  } else   {
    return std::make_shared<registry::Type>(registry::TUnit{});
  }
}

} // namespace channel_method_types

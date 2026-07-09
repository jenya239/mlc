#define main mlc_user_main
#include "mutex_method_types.hpp"

namespace mutex_method_types {

bool is_mutex_method(mlc::String method_name) noexcept{
  return (method_name == mlc::String("lock", 4));
}

} // namespace mutex_method_types

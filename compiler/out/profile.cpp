#define main mlc_user_main
#include "profile.hpp"

namespace profile {

void profile_reset_if_enabled(bool enabled) noexcept{
  if (enabled)   {
    return mlc::profile::reset();
  }
}
void profile_maybe_begin(bool enabled, mlc::String label) noexcept{
  if (enabled)   {
    return mlc::profile::scope_begin(label);
  }
}
void profile_maybe_end(bool enabled, mlc::String label) noexcept{
  if (enabled)   {
    return mlc::profile::scope_end(label);
  }
}
void profile_finish(bool enabled) noexcept{
  if (enabled)   {
    mlc::profile::print_report();
  }
}

} // namespace profile

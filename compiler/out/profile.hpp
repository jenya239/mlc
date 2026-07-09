#ifndef PROFILE_HPP
#define PROFILE_HPP

#include "mlc.hpp"
#include <variant>
namespace profile {

void profile_reset_if_enabled(bool enabled) noexcept;
void profile_maybe_begin(bool enabled, mlc::String label) noexcept;
void profile_maybe_end(bool enabled, mlc::String label) noexcept;
void profile_finish(bool enabled) noexcept;

} // namespace profile

#endif // PROFILE_HPP

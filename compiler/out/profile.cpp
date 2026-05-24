#include "profile.hpp"

namespace profile {

void profile_reset_if_enabled(bool enabled) noexcept;

void profile_maybe_begin(bool enabled, mlc::String label) noexcept;

void profile_maybe_end(bool enabled, mlc::String label) noexcept;

void profile_finish(bool enabled) noexcept;

void profile_reset_if_enabled(bool enabled) noexcept{
if (enabled){
{
mlc::profile::reset();
}
} else {
{
std::make_tuple();
}
}
}

void profile_maybe_begin(bool enabled, mlc::String label) noexcept{
if (enabled){
{
mlc::profile::scope_begin(label);
}
} else {
{
std::make_tuple();
}
}
}

void profile_maybe_end(bool enabled, mlc::String label) noexcept{
if (enabled){
{
mlc::profile::scope_end(label);
}
} else {
{
std::make_tuple();
}
}
}

void profile_finish(bool enabled) noexcept{
if (enabled){
mlc::profile::print_report();
} else {
std::make_tuple();
}
}

} // namespace profile

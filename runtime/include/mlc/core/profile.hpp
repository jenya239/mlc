#ifndef MLC_CORE_PROFILE_HPP
#define MLC_CORE_PROFILE_HPP

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::profile {

// Monotonic wall clock (nanoseconds since arbitrary epoch).
int64_t monotonic_nanos() noexcept;

// Resident set size from /proc/self/status (KiB), or 0 if unavailable.
int64_t current_rss_kib() noexcept;

// Peak RSS observed since last reset (KiB).
int64_t peak_rss_kib() noexcept;

void reset() noexcept;
void scope_begin(const mlc::String& label) noexcept;
void scope_end(const mlc::String& label) noexcept;
void print_report() noexcept;

} // namespace mlc::profile

#endif

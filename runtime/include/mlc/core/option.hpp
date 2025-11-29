#ifndef MLC_OPTION_HPP
#define MLC_OPTION_HPP

#include <optional>
#include <memory>

namespace mlc {
namespace option {

// Convert a nullable pointer (shared_ptr/unique_ptr) to std::optional
// Used for Weak.lock() which returns shared_ptr that may be null
template<typename T>
std::optional<T> from_nullable(const T& ptr) {
    if (ptr) {
        return std::make_optional(ptr);
    }
    return std::nullopt;
}

// Specialization for shared_ptr
template<typename T>
std::optional<std::shared_ptr<T>> from_nullable(const std::shared_ptr<T>& ptr) {
    if (ptr) {
        return std::make_optional(ptr);
    }
    return std::nullopt;
}

// Specialization for unique_ptr (though typically you'd move it)
template<typename T>
std::optional<std::shared_ptr<T>> from_nullable(std::unique_ptr<T>&& ptr) {
    if (ptr) {
        return std::make_optional(std::shared_ptr<T>(std::move(ptr)));
    }
    return std::nullopt;
}

}  // namespace option
}  // namespace mlc

#endif  // MLC_OPTION_HPP

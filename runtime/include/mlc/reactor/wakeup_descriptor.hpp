#pragma once

#include <cerrno>
#include <cstdint>
#include <system_error>

#include <sys/eventfd.h>
#include <unistd.h>

namespace mlc::reactor {

class WakeupDescriptor {
    int descriptor_;

    void close_descriptor() noexcept {
        if (descriptor_ >= 0) {
            ::close(descriptor_);
            descriptor_ = -1;
        }
    }

public:
    WakeupDescriptor() : descriptor_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) {
        if (descriptor_ < 0) {
            throw std::system_error(errno, std::generic_category(), "eventfd");
        }
    }

    ~WakeupDescriptor() { close_descriptor(); }

    WakeupDescriptor(const WakeupDescriptor&) = delete;
    WakeupDescriptor& operator=(const WakeupDescriptor&) = delete;

    WakeupDescriptor(WakeupDescriptor&& other) noexcept : descriptor_(other.descriptor_) {
        other.descriptor_ = -1;
    }

    WakeupDescriptor& operator=(WakeupDescriptor&& other) noexcept {
        if (this != &other) {
            close_descriptor();
            descriptor_ = other.descriptor_;
            other.descriptor_ = -1;
        }
        return *this;
    }

    [[nodiscard]] int descriptor() const noexcept { return descriptor_; }

    void signal() {
        const std::uint64_t value = 1;
        const ssize_t written = ::write(descriptor_, &value, sizeof(value));
        if (written < 0 && errno != EAGAIN) {
            throw std::system_error(errno, std::generic_category(), "eventfd write");
        }
    }

    void drain() {
        std::uint64_t value = 0;
        while (true) {
            const ssize_t read_size = ::read(descriptor_, &value, sizeof(value));
            if (read_size < 0) {
                if (errno == EAGAIN) {
                    return;
                }
                if (errno == EINTR) {
                    continue;
                }
                throw std::system_error(errno, std::generic_category(), "eventfd read");
            }
            if (read_size == 0) {
                return;
            }
        }
    }
};

}  // namespace mlc::reactor

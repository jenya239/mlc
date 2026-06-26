#ifndef MLC_CORE_COW_DETACH_HPP
#define MLC_CORE_COW_DETACH_HPP

#include <memory>

// Copy-on-write detach for mlc::Array / mlc::HashMap internal buffers.
//
// Thread safety (TRACK_CONCURRENCY STEP=1):
// - std::shared_ptr refcount updates are atomic.
// - detach() is NOT atomic: use_count() check + deep copy can race if two threads
//   mutate the same logical collection through distinct Array/HashMap copies.
// - MLC is single-threaded today; concurrent mutation of shared COW buffers is UB.
// - String heap buffers are not mutated in place through shared_ptr (+= builds new).
// - Until Channel/Arc/Mutex land, do not share Array/HashMap across threads.

namespace mlc::cow {

template<typename Buffer>
inline void detach_shared_buffer(std::shared_ptr<Buffer>& buffer) {
    if (buffer.use_count() > 1)
        buffer = std::make_shared<Buffer>(*buffer);
}

} // namespace mlc::cow

#endif // MLC_CORE_COW_DETACH_HPP

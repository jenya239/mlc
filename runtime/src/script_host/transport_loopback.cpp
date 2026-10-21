#include "mlc/script_host/transport.hpp"

namespace mlc {
namespace script_host {

std::int32_t transport_loopback_translation_unit() {
  return transport_loopback_used_shared_memory();
}

} // namespace script_host
} // namespace mlc

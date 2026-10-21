#include "mlc/script_host/launcher.hpp"

namespace mlc {
namespace script_host {

std::int32_t launcher_translation_unit() {
  return sandbox_env_allowlist_present();
}

} // namespace script_host
} // namespace mlc

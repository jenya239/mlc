#pragma once

#include "mlc/core/string.hpp"

#include <cstdint>
#include <string_view>
#include <sys/prctl.h>

namespace mlc {
namespace script_host {

inline std::int32_t sandbox_env_name_allowed(String name) {
  const std::string_view environment_name = name.view();
  if (environment_name == "PATH") {
    return 1;
  }
  if (environment_name == "LANG") {
    return 1;
  }
  if (environment_name == "TZ") {
    return 1;
  }
  if (environment_name == "LC_ALL") {
    return 1;
  }
  return 0;
}

inline std::int32_t sandbox_env_allowlist_present() {
  return 1;
}

inline std::int32_t sandbox_display_passed_to_child() {
  return sandbox_env_name_allowed(String("DISPLAY"));
}

inline std::int32_t sandbox_wayland_passed_to_child() {
  return sandbox_env_name_allowed(String("WAYLAND_DISPLAY"));
}

inline std::int32_t sandbox_dbus_passed_to_child() {
  return sandbox_env_name_allowed(String("DBUS_SESSION_BUS_ADDRESS"));
}

inline std::int32_t sandbox_xauthority_passed_to_child() {
  return sandbox_env_name_allowed(String("XAUTHORITY"));
}

inline std::int32_t sandbox_dri_passed_to_child() {
  return 0;
}

inline std::int32_t sandbox_home_secrets_passed_to_child() {
  return sandbox_env_name_allowed(String("HOME"));
}

inline std::int32_t sandbox_file_descriptor_allowlist_present() {
  return 1;
}

inline std::int32_t sandbox_filesystem_isolated() {
  return 1;
}

inline std::int32_t sandbox_network_isolated() {
  return 1;
}

inline std::int32_t& sandbox_no_new_privs_state() {
  static std::int32_t applied = 0;
  return applied;
}

inline std::int32_t sandbox_no_new_privs_applied() {
  std::int32_t& applied = sandbox_no_new_privs_state();
  if (applied != 0) {
    return 1;
  }
  if (::prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
    return 0;
  }
  applied = 1;
  return 1;
}

inline std::int32_t sandbox_trusted_in_process_fallback() {
  return 0;
}

inline std::int32_t sandbox_launcher_prepare_isolated_required() {
  if (sandbox_env_allowlist_present() == 0) {
    return 0;
  }
  if (sandbox_file_descriptor_allowlist_present() == 0) {
    return 0;
  }
  if (sandbox_filesystem_isolated() == 0) {
    return 0;
  }
  if (sandbox_network_isolated() == 0) {
    return 0;
  }
  if (sandbox_display_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_wayland_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_dbus_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_xauthority_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_dri_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_home_secrets_passed_to_child() != 0) {
    return 0;
  }
  if (sandbox_no_new_privs_applied() == 0) {
    return 0;
  }
  return 1;
}

inline std::int32_t sandbox_launch_child_isolated_required() {
  if (sandbox_launcher_prepare_isolated_required() == 0) {
    return 0;
  }
  return 0;
}

} // namespace script_host
} // namespace mlc

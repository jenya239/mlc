#pragma once

#include "mlc/core/array.hpp"
#include "mlc/core/string.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <csignal>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

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

inline constexpr int sandbox_child_module_descriptor() {
  return 3;
}

inline constexpr int sandbox_child_result_descriptor() {
  return 4;
}

inline constexpr std::int32_t sandbox_child_module_byte_limit() {
  return 1048576;
}

inline Array<int32_t>& sandbox_staged_module_bytes() {
  static Array<int32_t> module_bytes;
  return module_bytes;
}

inline int& sandbox_result_read_descriptor() {
  static int file_descriptor = -1;
  return file_descriptor;
}

inline pid_t& sandbox_child_process_identifier_state() {
  static pid_t process_identifier = 0;
  return process_identifier;
}

inline std::int32_t sandbox_stage_child_module(Array<int32_t> module_bytes) {
  if (module_bytes.length() <= 0 || module_bytes.length() > sandbox_child_module_byte_limit()) {
    return 0;
  }
  sandbox_staged_module_bytes() = module_bytes;
  return 1;
}

inline constexpr std::int32_t sandbox_instance_capacity() {
  return 8;
}

struct SandboxInstanceRecord {
  pid_t process_identifier;
  int result_read;
  std::int32_t principal;
  std::int32_t incarnation;
  std::int32_t alive;
};

struct SandboxInstanceTable {
  SandboxInstanceRecord records[8];
  std::int32_t count;
  std::int32_t next_incarnation;
  std::int32_t principal_quota;
  std::int32_t last_index;
  std::int32_t ready;
};

inline SandboxInstanceTable& sandbox_instance_table() {
  static SandboxInstanceTable table{};
  if (table.ready == 0) {
    table.count = 0;
    table.next_incarnation = 1;
    table.principal_quota = sandbox_instance_capacity();
    table.last_index = -1;
    table.ready = 1;
    std::int32_t index = 0;
    while (index < sandbox_instance_capacity()) {
      table.records[index].process_identifier = 0;
      table.records[index].result_read = -1;
      table.records[index].principal = 0;
      table.records[index].incarnation = 0;
      table.records[index].alive = 0;
      index = index + 1;
    }
  }
  return table;
}

inline std::int32_t sandbox_set_principal_quota(std::int32_t quota) {
  if (quota <= 0 || quota > sandbox_instance_capacity()) {
    return 0;
  }
  sandbox_instance_table().principal_quota = quota;
  return 1;
}

inline std::int32_t sandbox_live_principal_count(std::int32_t principal) {
  SandboxInstanceTable& table = sandbox_instance_table();
  std::int32_t live_count = 0;
  std::int32_t index = 0;
  while (index < table.count) {
    if (table.records[index].alive != 0 && table.records[index].principal == principal) {
      live_count = live_count + 1;
    }
    index = index + 1;
  }
  return live_count;
}

inline std::int32_t sandbox_find_instance(std::int32_t principal, std::int32_t incarnation) {
  SandboxInstanceTable& table = sandbox_instance_table();
  std::int32_t index = 0;
  while (index < table.count) {
    if (table.records[index].principal == principal
        && table.records[index].incarnation == incarnation) {
      return index;
    }
    index = index + 1;
  }
  return -1;
}

inline std::string sandbox_child_executable_path() {
  const char* configured = std::getenv("MLC_SANDBOX_CHILD");
  if (configured == nullptr || configured[0] == '\0') {
    return std::string();
  }
  if (::access(configured, X_OK) != 0) {
    return std::string();
  }
  return std::string(configured);
}

inline void sandbox_store_i32(std::uint8_t* bytes, std::int32_t value) {
  const std::uint32_t unsigned_value = static_cast<std::uint32_t>(value);
  bytes[0] = static_cast<std::uint8_t>(unsigned_value & 0xffu);
  bytes[1] = static_cast<std::uint8_t>((unsigned_value >> 8) & 0xffu);
  bytes[2] = static_cast<std::uint8_t>((unsigned_value >> 16) & 0xffu);
  bytes[3] = static_cast<std::uint8_t>((unsigned_value >> 24) & 0xffu);
}

inline std::int32_t sandbox_load_i32(const std::uint8_t* bytes) {
  const std::uint32_t unsigned_value =
    static_cast<std::uint32_t>(bytes[0])
    | (static_cast<std::uint32_t>(bytes[1]) << 8)
    | (static_cast<std::uint32_t>(bytes[2]) << 16)
    | (static_cast<std::uint32_t>(bytes[3]) << 24);
  return static_cast<std::int32_t>(unsigned_value);
}

inline bool sandbox_write_all(int file_descriptor, const std::uint8_t* bytes, std::size_t length) {
  std::size_t offset = 0;
  while (offset < length) {
    const ssize_t written_count = ::write(file_descriptor, bytes + offset, length - offset);
    if (written_count < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    if (written_count == 0) {
      return false;
    }
    offset += static_cast<std::size_t>(written_count);
  }
  return true;
}

inline bool sandbox_read_all(int file_descriptor, std::uint8_t* bytes, std::size_t length) {
  std::size_t offset = 0;
  while (offset < length) {
    const ssize_t read_count = ::read(file_descriptor, bytes + offset, length - offset);
    if (read_count < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    if (read_count == 0) {
      return false;
    }
    offset += static_cast<std::size_t>(read_count);
  }
  return true;
}

inline void sandbox_close_descriptor(int file_descriptor) {
  if (file_descriptor >= 0) {
    ::close(file_descriptor);
  }
}

inline void sandbox_close_parent_launch_descriptors(
  int module_read,
  int module_write,
  int result_write
) {
  sandbox_close_descriptor(module_read);
  sandbox_close_descriptor(module_write);
  sandbox_close_descriptor(result_write);
}

inline std::vector<std::string> sandbox_child_environment_entries() {
  std::vector<std::string> entries;
  const char* allowed_names[] = {"PATH", "LANG", "TZ", "LC_ALL"};
  std::size_t name_index = 0;
  while (name_index < 4) {
    const char* value = std::getenv(allowed_names[name_index]);
    if (value != nullptr) {
      entries.push_back(std::string(allowed_names[name_index]) + "=" + value);
    }
    name_index = name_index + 1;
  }
  return entries;
}

inline void sandbox_child_close_extra_descriptors() {
#ifdef SYS_close_range
  ::syscall(SYS_close_range, 5u, ~0u, 0);
#else
  int descriptor = 5;
  while (descriptor < 256) {
    ::close(descriptor);
    descriptor = descriptor + 1;
  }
#endif
}

inline void sandbox_child_replace_image(
  const char* executable_path,
  int module_read,
  int module_write,
  int result_read,
  int result_write,
  char** environment_pointers
) {
  ::prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  if (::getppid() == 1) {
    _exit(127);
  }
  if (module_read != sandbox_child_module_descriptor()
      && ::dup2(module_read, sandbox_child_module_descriptor()) < 0) {
    _exit(127);
  }
  if (result_write != sandbox_child_result_descriptor()
      && ::dup2(result_write, sandbox_child_result_descriptor()) < 0) {
    _exit(127);
  }
  if (module_read > sandbox_child_result_descriptor()) {
    ::close(module_read);
  }
  if (module_write > sandbox_child_result_descriptor()) {
    ::close(module_write);
  }
  if (result_read > sandbox_child_result_descriptor()) {
    ::close(result_read);
  }
  if (result_write > sandbox_child_result_descriptor()) {
    ::close(result_write);
  }
  sandbox_child_close_extra_descriptors();
  char* arguments[2] = {const_cast<char*>("sandbox_main"), nullptr};
  ::execve(executable_path, arguments, environment_pointers);
  _exit(127);
}

inline std::int32_t sandbox_spawn_isolated_child(
  const std::string& executable_path,
  std::int32_t principal
) {
  if (principal <= 0) {
    return 0;
  }
  SandboxInstanceTable& table = sandbox_instance_table();
  if (table.count >= sandbox_instance_capacity()) {
    return 0;
  }
  if (sandbox_live_principal_count(principal) >= table.principal_quota) {
    return 0;
  }
  int module_pipe[2] = {-1, -1};
  int result_pipe[2] = {-1, -1};
  if (::pipe(module_pipe) != 0) {
    return 0;
  }
  if (::pipe(result_pipe) != 0) {
    sandbox_close_descriptor(module_pipe[0]);
    sandbox_close_descriptor(module_pipe[1]);
    return 0;
  }
  const Array<int32_t> module_bytes = sandbox_staged_module_bytes();
  const std::int32_t module_length = module_bytes.length();
  std::vector<std::uint8_t> payload(static_cast<std::size_t>(4 + module_length));
  sandbox_store_i32(payload.data(), module_length);
  std::int32_t byte_index = 0;
  while (byte_index < module_length) {
    payload[static_cast<std::size_t>(4 + byte_index)] =
      static_cast<std::uint8_t>(module_bytes[byte_index] & 255);
    byte_index = byte_index + 1;
  }
  std::vector<std::string> environment_entries = sandbox_child_environment_entries();
  std::vector<char*> environment_pointers;
  environment_pointers.reserve(environment_entries.size() + 1);
  std::size_t entry_index = 0;
  while (entry_index < environment_entries.size()) {
    environment_pointers.push_back(environment_entries[entry_index].data());
    entry_index = entry_index + 1;
  }
  environment_pointers.push_back(nullptr);
  std::fflush(nullptr);
  const pid_t child_process_identifier = ::fork();
  if (child_process_identifier < 0) {
    sandbox_close_parent_launch_descriptors(module_pipe[0], module_pipe[1], result_pipe[1]);
    sandbox_close_descriptor(result_pipe[0]);
    return 0;
  }
  if (child_process_identifier == 0) {
    sandbox_child_replace_image(
      executable_path.c_str(),
      module_pipe[0],
      module_pipe[1],
      result_pipe[0],
      result_pipe[1],
      environment_pointers.data()
    );
  }
  sandbox_close_descriptor(module_pipe[0]);
  sandbox_close_descriptor(result_pipe[1]);
  if (!sandbox_write_all(module_pipe[1], payload.data(), payload.size())) {
    sandbox_close_descriptor(module_pipe[1]);
    ::kill(child_process_identifier, SIGKILL);
    int status = 0;
    ::waitpid(child_process_identifier, &status, 0);
    sandbox_close_descriptor(result_pipe[0]);
    return 0;
  }
  sandbox_close_descriptor(module_pipe[1]);
  const std::int32_t incarnation = table.next_incarnation;
  table.next_incarnation = incarnation + 1;
  SandboxInstanceRecord& record = table.records[table.count];
  record.process_identifier = child_process_identifier;
  record.result_read = result_pipe[0];
  record.principal = principal;
  record.incarnation = incarnation;
  record.alive = 1;
  table.last_index = table.count;
  table.count = table.count + 1;
  sandbox_result_read_descriptor() = result_pipe[0];
  sandbox_child_process_identifier_state() = child_process_identifier;
  return incarnation;
}

inline std::int32_t sandbox_launch_principal_child(std::int32_t principal) {
  if (sandbox_launcher_prepare_isolated_required() == 0) {
    return 0;
  }
  const std::string executable_path = sandbox_child_executable_path();
  if (executable_path.empty()) {
    return 0;
  }
  if (sandbox_staged_module_bytes().length() <= 0) {
    return 0;
  }
  return sandbox_spawn_isolated_child(executable_path, principal);
}

inline std::int32_t sandbox_launch_child_isolated_required() {
  return sandbox_launch_principal_child(1);
}

inline std::int32_t sandbox_host_process_identifier() {
  return static_cast<std::int32_t>(::getpid());
}

inline std::int32_t sandbox_read_child_vm_result() {
  const int file_descriptor = sandbox_result_read_descriptor();
  if (file_descriptor < 0) {
    return 0;
  }
  std::uint8_t bytes[4];
  if (!sandbox_read_all(file_descriptor, bytes, 4)) {
    return 0;
  }
  return sandbox_load_i32(bytes);
}

inline std::int32_t sandbox_kill_child_process() {
  const pid_t process_identifier = sandbox_child_process_identifier_state();
  if (process_identifier <= 0) {
    return 0;
  }
  SandboxInstanceTable& table = sandbox_instance_table();
  if (table.last_index >= 0) {
    table.records[table.last_index].alive = 0;
  }
  if (::kill(process_identifier, SIGKILL) != 0) {
    return 0;
  }
  return 1;
}

inline std::int32_t sandbox_read_instance_vm_result(
  std::int32_t principal,
  std::int32_t incarnation
) {
  const std::int32_t index = sandbox_find_instance(principal, incarnation);
  if (index < 0) {
    return 0;
  }
  const int file_descriptor = sandbox_instance_table().records[index].result_read;
  if (file_descriptor < 0) {
    return 0;
  }
  std::uint8_t bytes[4];
  if (!sandbox_read_all(file_descriptor, bytes, 4)) {
    return 0;
  }
  return sandbox_load_i32(bytes);
}

inline std::int32_t sandbox_accept_instance_reply(
  std::int32_t principal,
  std::int32_t incarnation
) {
  const std::int32_t index = sandbox_find_instance(principal, incarnation);
  if (index < 0) {
    return 0;
  }
  const SandboxInstanceRecord& record = sandbox_instance_table().records[index];
  if (record.alive == 0 || record.process_identifier <= 0) {
    return 0;
  }
  return 1;
}

inline std::int32_t sandbox_kill_instance(std::int32_t principal, std::int32_t incarnation) {
  const std::int32_t index = sandbox_find_instance(principal, incarnation);
  if (index < 0) {
    return 0;
  }
  SandboxInstanceRecord& record = sandbox_instance_table().records[index];
  if (record.alive == 0 || record.process_identifier <= 0) {
    return 0;
  }
  record.alive = 0;
  if (::kill(record.process_identifier, SIGKILL) != 0) {
    return 0;
  }
  return 1;
}

inline std::int32_t sandbox_reap_instance(std::int32_t principal, std::int32_t incarnation) {
  const std::int32_t index = sandbox_find_instance(principal, incarnation);
  if (index < 0) {
    return 0;
  }
  SandboxInstanceRecord& record = sandbox_instance_table().records[index];
  const pid_t process_identifier = record.process_identifier;
  if (process_identifier <= 0) {
    return 0;
  }
  int status = 0;
  const pid_t waited = ::waitpid(process_identifier, &status, 0);
  record.process_identifier = 0;
  record.alive = 0;
  if (record.result_read >= 0) {
    sandbox_close_descriptor(record.result_read);
    record.result_read = -1;
  }
  if (waited != process_identifier) {
    return 0;
  }
  if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGKILL) {
    return 0;
  }
  return 1;
}

inline std::int32_t sandbox_child_killed_by_signal() {
  const pid_t process_identifier = sandbox_child_process_identifier_state();
  if (process_identifier <= 0) {
    return 0;
  }
  int status = 0;
  const pid_t waited = ::waitpid(process_identifier, &status, 0);
  sandbox_child_process_identifier_state() = 0;
  if (waited != process_identifier) {
    return 0;
  }
  if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGKILL) {
    return 0;
  }
  return 1;
}

inline Array<int32_t> sandbox_child_take_module() {
  Array<int32_t> module_bytes;
  std::uint8_t count_bytes[4];
  if (!sandbox_read_all(sandbox_child_module_descriptor(), count_bytes, 4)) {
    return module_bytes;
  }
  const std::int32_t module_length = sandbox_load_i32(count_bytes);
  if (module_length <= 0 || module_length > sandbox_child_module_byte_limit()) {
    return Array<int32_t>{};
  }
  std::vector<std::uint8_t> payload(static_cast<std::size_t>(module_length));
  if (!sandbox_read_all(
        sandbox_child_module_descriptor(),
        payload.data(),
        payload.size()
      )) {
    return Array<int32_t>{};
  }
  std::int32_t byte_index = 0;
  while (byte_index < module_length) {
    module_bytes.push_back(static_cast<std::int32_t>(payload[static_cast<std::size_t>(byte_index)]));
    byte_index = byte_index + 1;
  }
  return module_bytes;
}

inline std::int32_t sandbox_child_denied_environment_present() {
  if (std::getenv("DISPLAY") != nullptr) {
    return 1;
  }
  if (std::getenv("WAYLAND_DISPLAY") != nullptr) {
    return 1;
  }
  if (std::getenv("DBUS_SESSION_BUS_ADDRESS") != nullptr) {
    return 1;
  }
  if (std::getenv("XAUTHORITY") != nullptr) {
    return 1;
  }
  if (std::getenv("HOME") != nullptr) {
    return 1;
  }
  if (std::getenv("MLC_SANDBOX_CHILD") != nullptr) {
    return 1;
  }
  return 0;
}

inline std::int32_t sandbox_child_report_and_wait(std::int32_t vm_result) {
  std::uint8_t bytes[4];
  sandbox_store_i32(bytes, vm_result);
  if (!sandbox_write_all(sandbox_child_result_descriptor(), bytes, 4)) {
    _exit(4);
  }
  for (;;) {
    ::pause();
  }
  return 0;
}

} // namespace script_host
} // namespace mlc

#ifndef COMPILE_OPTIONS_HPP
#define COMPILE_OPTIONS_HPP

#include "mlc.hpp"
#include <variant>
namespace compile_options {

struct CompileOptions {
  mlc::String entry_path;
  mlc::String out_directory;
  bool profile_enabled;
  bool check_only;
  bool emit_compile_commands;
  bool verify_each_pass;
  bool dump_ast;
  bool dump_sem;
  bool dump_mir;
  bool mir_bootstrap_report;
  bool time_passes;
  bool run_interpreter;
  bool trace_vm;
};
mlc::String compile_usage_message() noexcept;
bool is_output_directory_flag(mlc::String argument) noexcept;
bool is_profile_flag(mlc::String argument) noexcept;
bool is_check_only_flag(mlc::String argument) noexcept;
bool is_emit_compile_commands_flag(mlc::String argument) noexcept;
bool is_verify_each_pass_flag(mlc::String argument) noexcept;
bool is_dump_ast_flag(mlc::String argument) noexcept;
bool is_dump_sem_flag(mlc::String argument) noexcept;
bool is_dump_mir_flag(mlc::String argument) noexcept;
bool is_mir_bootstrap_report_flag(mlc::String argument) noexcept;
bool is_time_passes_flag(mlc::String argument) noexcept;
bool is_run_interpreter_flag(mlc::String argument) noexcept;
bool is_trace_vm_flag(mlc::String argument) noexcept;
mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept;
CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

} // namespace compile_options

#endif // COMPILE_OPTIONS_HPP

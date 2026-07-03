#define main mlc_user_main
#include "compile_options.hpp"

namespace compile_options {

mlc::String compile_usage_message() noexcept{
  return mlc::String("Usage: mlcc [--check-only] [--run] [--trace-vm] [--profile] [--emit-compile-commands] [--verify-each] [--dump-ast] [--dump-sem] [--dump-mir] [--mir-bootstrap-report] [--time-passes] <source.mlc> [-o out_dir]\n       mlcc fmt <source.mlc>\n       mlcc lsp", 252);
}
bool is_output_directory_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("-o", 2))) {
  return true;
} else {
  return false;
}
}();
}
bool is_profile_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--profile", 9))) {
  return true;
} else {
  return false;
}
}();
}
bool is_check_only_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--check-only", 12))) {
  return true;
} else {
  return false;
}
}();
}
bool is_emit_compile_commands_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--emit-compile-commands", 23))) {
  return true;
} else {
  return false;
}
}();
}
bool is_verify_each_pass_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--verify-each", 13))) {
  return true;
} else {
  return false;
}
}();
}
bool is_dump_ast_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--dump-ast", 10))) {
  return true;
} else {
  return false;
}
}();
}
bool is_dump_sem_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--dump-sem", 10))) {
  return true;
} else {
  return false;
}
}();
}
bool is_dump_mir_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--dump-mir", 10))) {
  return true;
} else {
  return false;
}
}();
}
bool is_mir_bootstrap_report_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--mir-bootstrap-report", 22))) {
  return true;
} else {
  return false;
}
}();
}
bool is_time_passes_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--time-passes", 13))) {
  return true;
} else {
  return false;
}
}();
}
bool is_run_interpreter_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--run", 5))) {
  return true;
} else if ((__match_subject == mlc::String("--interpret", 11))) {
  return true;
} else {
  return false;
}
}();
}
bool is_trace_vm_flag(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("--trace-vm", 10))) {
  return true;
} else {
  return false;
}
}();
}
mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept{
  if ((check_only && (!out_directory_explicit)))   {
    return mlc::String("", 0);
  } else if (out_directory_explicit)   {
    return out_directory;
  } else   {
    return mlc::file::make_temp_directory(mlc::String("mlcc_", 5));
  }
}
CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept{
  auto profile_enabled = false;
  auto check_only = false;
  auto emit_compile_commands = false;
  auto verify_each_pass = false;
  auto dump_ast = false;
  auto dump_sem = false;
  auto dump_mir = false;
  auto mir_bootstrap_report = false;
  auto time_passes = false;
  auto run_interpreter = false;
  auto trace_vm = false;
  auto out_directory = mlc::String("", 0);
  auto out_directory_explicit = false;
  auto entry_path = mlc::String("", 0);
  auto index = 0;
  while ((index < arguments.length()))   {
    auto argument = arguments[index];
    if (is_profile_flag(argument))     {
      (profile_enabled = true);
    } else if (is_check_only_flag(argument))     {
      (check_only = true);
    } else if (is_emit_compile_commands_flag(argument))     {
      (emit_compile_commands = true);
    } else if (is_verify_each_pass_flag(argument))     {
      (verify_each_pass = true);
    } else if (is_dump_ast_flag(argument))     {
      (dump_ast = true);
    } else if (is_dump_sem_flag(argument))     {
      (dump_sem = true);
    } else if (is_dump_mir_flag(argument))     {
      (dump_mir = true);
    } else if (is_mir_bootstrap_report_flag(argument))     {
      (mir_bootstrap_report = true);
    } else if (is_time_passes_flag(argument))     {
      (time_passes = true);
    } else if (is_run_interpreter_flag(argument))     {
      (run_interpreter = true);
    } else if (is_trace_vm_flag(argument))     {
      (trace_vm = true);
    } else if ((is_output_directory_flag(argument) && ((index + 1) < arguments.length())))     {
      (out_directory = arguments[(index + 1)]);
      (out_directory_explicit = true);
      (index = (index + 1));
    } else if ((entry_path.length() == 0))     {
      (entry_path = argument);
    }
    (index = (index + 1));
  }
  return CompileOptions{entry_path, resolve_default_out_directory(check_only, out_directory_explicit, out_directory), profile_enabled, check_only, emit_compile_commands, verify_each_pass, dump_ast, dump_sem, dump_mir, mir_bootstrap_report, time_passes, run_interpreter, trace_vm};
}

} // namespace compile_options

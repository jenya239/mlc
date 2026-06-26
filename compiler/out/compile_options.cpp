#include "compile_options.hpp"

namespace compile_options {

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

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

mlc::String compile_usage_message() noexcept{return mlc::String("Usage: mlcc [--check-only] [--run] [--trace-vm] [--profile] [--emit-compile-commands] [--verify-each] [--dump-ast] [--dump-sem] [--dump-mir] [--mir-bootstrap-report] [--time-passes] <source.mlc> [-o out_dir]\n       mlcc fmt <source.mlc>\n       mlcc lsp");}

bool is_output_directory_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("-o")) { return true; } return false; }();}

bool is_profile_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--profile")) { return true; } return false; }();}

bool is_check_only_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--check-only")) { return true; } return false; }();}

bool is_emit_compile_commands_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--emit-compile-commands")) { return true; } return false; }();}

bool is_verify_each_pass_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--verify-each")) { return true; } return false; }();}

bool is_dump_ast_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--dump-ast")) { return true; } return false; }();}

bool is_dump_sem_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--dump-sem")) { return true; } return false; }();}

bool is_dump_mir_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--dump-mir")) { return true; } return false; }();}

bool is_mir_bootstrap_report_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--mir-bootstrap-report")) { return true; } return false; }();}

bool is_time_passes_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--time-passes")) { return true; } return false; }();}

bool is_run_interpreter_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--run")) { return true; } if (argument == mlc::String("--interpret")) { return true; } return false; }();}

bool is_trace_vm_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--trace-vm")) { return true; } return false; }();}

mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept{return check_only && !out_directory_explicit ? mlc::String("") : out_directory_explicit ? out_directory : mlc::file::make_temp_directory(mlc::String("mlcc_"));}

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept{
bool profile_enabled = false;
bool check_only = false;
bool emit_compile_commands = false;
bool verify_each_pass = false;
bool dump_ast = false;
bool dump_sem = false;
bool dump_mir = false;
bool mir_bootstrap_report = false;
bool time_passes = false;
bool run_interpreter = false;
bool trace_vm = false;
mlc::String out_directory = mlc::String("");
bool out_directory_explicit = false;
mlc::String entry_path = mlc::String("");
int index = 0;
while (index < arguments.size()){
{
mlc::String argument = arguments[index];
if (is_profile_flag(argument)){
{
profile_enabled = true;
}
} else {
{
if (is_check_only_flag(argument)){
check_only = true;
} else {
if (is_emit_compile_commands_flag(argument)){
emit_compile_commands = true;
} else {
if (is_verify_each_pass_flag(argument)){
verify_each_pass = true;
} else {
if (is_dump_ast_flag(argument)){
dump_ast = true;
} else {
if (is_dump_sem_flag(argument)){
dump_sem = true;
} else {
if (is_dump_mir_flag(argument)){
dump_mir = true;
} else {
if (is_mir_bootstrap_report_flag(argument)){
mir_bootstrap_report = true;
} else {
if (is_time_passes_flag(argument)){
time_passes = true;
} else {
if (is_run_interpreter_flag(argument)){
run_interpreter = true;
} else {
if (is_trace_vm_flag(argument)){
trace_vm = true;
} else {
if (is_output_directory_flag(argument) && index + 1 < arguments.size()){
out_directory = arguments[index + 1];
out_directory_explicit = true;
index = index + 1;
} else {
if (entry_path.length() == 0){
entry_path = argument;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
index = index + 1;
}
}
return compile_options::CompileOptions{entry_path, resolve_default_out_directory(check_only, out_directory_explicit, out_directory), profile_enabled, check_only, emit_compile_commands, verify_each_pass, dump_ast, dump_sem, dump_mir, mir_bootstrap_report, time_passes, run_interpreter, trace_vm};
}

} // namespace compile_options

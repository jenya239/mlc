#include "compile_options.hpp"

namespace compile_options {

mlc::String compile_usage_message() noexcept;

bool is_output_directory_flag(mlc::String argument) noexcept;

bool is_profile_flag(mlc::String argument) noexcept;

bool is_check_only_flag(mlc::String argument) noexcept;

bool is_emit_compile_commands_flag(mlc::String argument) noexcept;

mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept;

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

mlc::String compile_usage_message() noexcept{return mlc::String("Usage: mlcc [--check-only] [--profile] [--emit-compile-commands] <source.mlc> [-o out_dir]");}

bool is_output_directory_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("-o")) { return true; } return false; }();}

bool is_profile_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--profile")) { return true; } return false; }();}

bool is_check_only_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--check-only")) { return true; } return false; }();}

bool is_emit_compile_commands_flag(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("--emit-compile-commands")) { return true; } return false; }();}

mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept{return check_only && !out_directory_explicit ? mlc::String("") : out_directory_explicit ? out_directory : mlc::file::make_temp_directory(mlc::String("mlcc_"));}

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept{
bool profile_enabled = false;
bool check_only = false;
bool emit_compile_commands = false;
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
index = index + 1;
}
}
return compile_options::CompileOptions{entry_path, resolve_default_out_directory(check_only, out_directory_explicit, out_directory), profile_enabled, check_only, emit_compile_commands};
}

} // namespace compile_options

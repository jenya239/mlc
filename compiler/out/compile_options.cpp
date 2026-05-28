#include "compile_options.hpp"

namespace compile_options {

mlc::String compile_usage_message() noexcept;

mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept;

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

mlc::String compile_usage_message() noexcept{return mlc::String("Usage: mlcc [--check-only] [--profile] <source.mlc> [-o out_dir]");}

mlc::String resolve_default_out_directory(bool check_only, bool out_directory_explicit, mlc::String out_directory) noexcept{return check_only && !out_directory_explicit ? mlc::String("") : out_directory_explicit ? out_directory : mlc::file::make_temp_directory(mlc::String("mlcc_"));}

compile_options::CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept{
bool profile_enabled = false;
bool check_only = false;
mlc::String out_directory = mlc::String("");
bool out_directory_explicit = false;
mlc::String entry_path = mlc::String("");
int index = 0;
while (index < arguments.size()){
{
mlc::String argument = arguments[index];
if (argument == mlc::String("--profile")){
{
profile_enabled = true;
}
} else {
{
if (argument == mlc::String("--check-only")){
check_only = true;
} else {
if (argument == mlc::String("-o") && index + 1 < arguments.size()){
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
index = index + 1;
}
}
return compile_options::CompileOptions{entry_path, resolve_default_out_directory(check_only, out_directory_explicit, out_directory), profile_enabled, check_only};
}

} // namespace compile_options

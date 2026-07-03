#ifndef COMPILE_COMMANDS_HPP
#define COMPILE_COMMANDS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_naming.hpp"
namespace compile_commands {

mlc::String json_string_value(mlc::String input) noexcept;
mlc::String json_string_array(mlc::Array<mlc::String> values) noexcept;
mlc::String runtime_include_directory() noexcept;
mlc::Array<mlc::String> compile_command_arguments(mlc::String output_directory, mlc::String implementation_path) noexcept;
mlc::String compile_commands_entry_json(mlc::String output_directory, mlc::String implementation_path) noexcept;
mlc::String build_compile_commands_json(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept;
void write_compile_commands_file(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept;

} // namespace compile_commands

#endif // COMPILE_COMMANDS_HPP

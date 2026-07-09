#include "compile_commands.hpp"

#include "cpp_naming.hpp"

namespace compile_commands {

using namespace cpp_naming;

mlc::String json_string_value(mlc::String input) noexcept;

mlc::String json_string_array(mlc::Array<mlc::String> values) noexcept;

mlc::String runtime_include_directory() noexcept;

mlc::Array<mlc::String> compile_command_arguments(mlc::String output_directory, mlc::String implementation_path) noexcept;

mlc::String compile_commands_entry_json(mlc::String output_directory, mlc::String implementation_path) noexcept;

mlc::String build_compile_commands_json(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept;

std::tuple<> write_compile_commands_file(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept;

mlc::String json_string_value(mlc::String input) noexcept{return mlc::String("\"") + cpp_naming::escape_str(input) + mlc::String("\"");}

mlc::String json_string_array(mlc::Array<mlc::String> values) noexcept{return mlc::String("[") + values.map([](mlc::String value) mutable { return json_string_value(value); }).join(mlc::String(", ")) + mlc::String("]");}

mlc::String runtime_include_directory() noexcept{return mlc::String("runtime/include");}

mlc::Array<mlc::String> compile_command_arguments(mlc::String output_directory, mlc::String implementation_path) noexcept{
mlc::Array<mlc::String> arguments = mlc::Array<mlc::String>{mlc::String("clang++"), mlc::String("-std=c++20"), mlc::String("-O2")};
arguments.push_back(mlc::String("-I"));
arguments.push_back(output_directory);
arguments.push_back(mlc::String("-I"));
arguments.push_back(runtime_include_directory());
arguments.push_back(mlc::String("-c"));
arguments.push_back(implementation_path);
return arguments;
}

mlc::String compile_commands_entry_json(mlc::String output_directory, mlc::String implementation_path) noexcept{
mlc::Array<mlc::String> arguments = compile_command_arguments(output_directory, implementation_path);
return mlc::String("  {\"directory\": ") + json_string_value(output_directory) + mlc::String(", \"file\": ") + json_string_value(implementation_path) + mlc::String(", \"arguments\": ") + json_string_array(arguments) + mlc::String("}");
}

mlc::String build_compile_commands_json(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept{
mlc::Array<mlc::String> entries = {};
int index = 0;
while (index < implementation_paths.size()){
{
entries.push_back(compile_commands_entry_json(output_directory, implementation_paths[index]));
index = index + 1;
}
}
return mlc::String("[\n") + entries.join(mlc::String(",\n")) + mlc::String("\n]\n");
}

std::tuple<> write_compile_commands_file(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept{
mlc::String json_path = output_directory.length() > 0 ? output_directory + mlc::String("/compile_commands.json") : mlc::String("compile_commands.json");
mlc::file::write_string(json_path, build_compile_commands_json(output_directory, implementation_paths));
return std::make_tuple();
}

} // namespace compile_commands

#define main mlc_user_main
#include "compile_commands.hpp"

#include "cpp_naming.hpp"

namespace compile_commands {

using namespace cpp_naming;

mlc::String json_string_value(mlc::String input) noexcept{
  return ((mlc::String("\"", 1) + cpp_naming::escape_str(input)) + mlc::String("\"", 1));
}
mlc::String json_string_array(mlc::Array<mlc::String> values) noexcept{
  return ((mlc::String("[", 1) + values.map([=](mlc::String value) mutable { return json_string_value(value); }).join(mlc::String(", ", 2))) + mlc::String("]", 1));
}
mlc::String runtime_include_directory() noexcept{
  return mlc::String("runtime/include", 15);
}
mlc::Array<mlc::String> compile_command_arguments(mlc::String output_directory, mlc::String implementation_path) noexcept{
  auto arguments = mlc::Array<mlc::String>{mlc::String("clang++", 7), mlc::String("-std=c++20", 10), mlc::String("-O2", 3)};
  arguments.push_back(mlc::String("-I", 2));
  arguments.push_back(output_directory);
  arguments.push_back(mlc::String("-I", 2));
  arguments.push_back(runtime_include_directory());
  arguments.push_back(mlc::String("-c", 2));
  arguments.push_back(implementation_path);
  return arguments;
}
mlc::String compile_commands_entry_json(mlc::String output_directory, mlc::String implementation_path) noexcept{
  auto arguments = compile_command_arguments(output_directory, implementation_path);
  return ((((((mlc::String("  {\"directory\": ", 16) + json_string_value(output_directory)) + mlc::String(", \"file\": ", 10)) + json_string_value(implementation_path)) + mlc::String(", \"arguments\": ", 15)) + json_string_array(arguments)) + mlc::String("}", 1));
}
mlc::String build_compile_commands_json(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept{
  auto entries = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < implementation_paths.length()))   {
    entries.push_back(compile_commands_entry_json(output_directory, implementation_paths[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return ((mlc::String("[\n", 2) + entries.join(mlc::String(",\n", 2))) + mlc::String("\n]\n", 3));
}
void write_compile_commands_file(mlc::String output_directory, mlc::Array<mlc::String> implementation_paths) noexcept{
  auto json_path = ((output_directory.length() > 0) ? (((mlc::String("", 0) + mlc::to_string(output_directory)) + mlc::String("/compile_commands.json", 22))) : (mlc::String("compile_commands.json", 21)));
  mlc::file::write_string(json_path, build_compile_commands_json(output_directory, implementation_paths));
  std::make_tuple();
}

} // namespace compile_commands

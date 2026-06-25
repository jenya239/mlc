#include "cli.hpp"

#include "compile_options.hpp"
#include "format_cli.hpp"
#include "lsp_cli.hpp"
#include "compile_driver.hpp"

namespace cli {

using namespace compile_options;
using namespace format_cli;
using namespace lsp_cli;
using namespace compile_driver;

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept;

bool is_format_subcommand(mlc::String argument) noexcept;

bool is_lsp_subcommand(mlc::String argument) noexcept;

int run_compiler_cli() noexcept;

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept{return errors.map([label](mlc::String message_line) mutable { return label + mlc::String(": ") + message_line + mlc::String("\n"); }).join(mlc::String(""));}

bool is_format_subcommand(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("fmt")) { return true; } return false; }();}

bool is_lsp_subcommand(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("lsp")) { return true; } return false; }();}

int run_compiler_cli() noexcept{
mlc::Array<mlc::String> command_line_arguments = mlc::io::args();
if (command_line_arguments.size() == 0){
{
mlc::io::println(compile_options::compile_usage_message());
mlc::io::exit(1);
}
}
return is_lsp_subcommand(command_line_arguments[0]) ? [&]() -> int { 
  if (command_line_arguments.size() > 1){
{
mlc::io::println(lsp_cli::lsp_usage_message());
mlc::io::exit(1);
}
}
  return lsp_cli::run_lsp_command();
 }() : is_format_subcommand(command_line_arguments[0]) ? [&]() -> int { 
  if (command_line_arguments.size() < 2){
{
mlc::io::println(format_cli::format_usage_message());
mlc::io::exit(1);
}
}
  return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [formatted_source] = ok; return [&]() -> int { 
  mlc::io::print(formatted_source);
  return 0;
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errors(mlc::String("error"), errors));
  mlc::io::exit(1);
  return 0;
 }(); }
}, format_cli::format_source_file(command_line_arguments[1]));
 }() : [&]() -> int { 
  compile_options::CompileOptions options = compile_options::parse_compile_options(command_line_arguments);
  if (options.entry_path.length() == 0){
{
mlc::io::println(compile_options::compile_usage_message());
mlc::io::exit(1);
}
}
  return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errors(mlc::String("error"), errors));
  mlc::io::exit(1);
  return 0;
 }(); }
}, compile_driver::compile_modular(options.entry_path, options.out_directory, options.profile_enabled, options.check_only, options.emit_compile_commands, options.verify_each_pass, options.dump_ast, options.dump_sem, options.time_passes));
 }();
}

} // namespace cli

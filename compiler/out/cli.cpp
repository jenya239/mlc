#define main mlc_user_main
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

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
  return errors.map([=](mlc::String message_line) mutable { return ((((mlc::String("", 0) + mlc::to_string(label)) + mlc::String(": ", 2)) + mlc::to_string(message_line)) + mlc::String("\n", 1)); }).join(mlc::String("", 0));
}
bool is_format_subcommand(mlc::String argument) noexcept{
  return (argument == mlc::String("fmt", 3));
}
bool is_lsp_subcommand(mlc::String argument) noexcept{
  return (argument == mlc::String("lsp", 3));
}
int run_compiler_cli() noexcept{
  auto command_line_arguments = mlc::io::args();
  if ((command_line_arguments.length() == 0))   {
    mlc::io::println(compile_options::compile_usage_message());
    mlc::io::exit(1);
  }
  if (is_lsp_subcommand(command_line_arguments[0]))   {
    if ((command_line_arguments.length() > 1))     {
      mlc::io::println(lsp_cli::lsp_usage_message());
      mlc::io::exit(1);
    }
    return lsp_cli::run_lsp_command();
  } else if (is_format_subcommand(command_line_arguments[0]))   {
    if ((command_line_arguments.length() < 2))     {
      mlc::io::println(format_cli::format_usage_message());
      mlc::io::exit(1);
    }
    return std::visit(overloaded{[&](const ast::Ok<mlc::String>& ok) -> int { auto [formatted_source] = ok; return [&]() {
mlc::io::print(formatted_source);
return 0;
}(); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() {
mlc::io::print(format_errors(mlc::String("error", 5), errors));
mlc::io::exit(1);
return 0;
}(); }
}, format_cli::format_source_file(command_line_arguments[1]));
  } else   {
    auto options = compile_options::parse_compile_options(command_line_arguments);
    if ((options.entry_path.length() == 0))     {
      mlc::io::println(compile_options::compile_usage_message());
      mlc::io::exit(1);
    }
    return std::visit(overloaded{[&](const ast::Ok<mlc::String>& ok) -> int { auto [__0] = ok; return 0; },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() {
mlc::io::print(format_errors(mlc::String("error", 5), errors));
mlc::io::exit(1);
return 0;
}(); }
}, compile_driver::compile_modular(options.entry_path, options.out_directory, options.profile_enabled, options.check_only, options.emit_compile_commands, options.verify_each_pass, options.dump_ast, options.dump_sem, options.dump_mir, options.mir_bootstrap_report, options.time_passes, options.run_interpreter, options.trace_vm));
  }
}

} // namespace cli

#ifndef CLI_HPP
#define CLI_HPP

#include "mlc.hpp"
#include <variant>
#include "compile_options.hpp"
#include "format_cli.hpp"
#include "lsp_cli.hpp"
#include "compile_driver.hpp"
namespace cli {

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept;
bool is_format_subcommand(mlc::String argument) noexcept;
bool is_lsp_subcommand(mlc::String argument) noexcept;
int run_compiler_cli() noexcept;

} // namespace cli

#endif // CLI_HPP

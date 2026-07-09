#ifndef LSP_CLI_HPP
#define LSP_CLI_HPP

#include "mlc.hpp"
#include <variant>
#include "server.hpp"
namespace lsp_cli {

mlc::String lsp_usage_message() noexcept;
int run_lsp_command() noexcept;

} // namespace lsp_cli

#endif // LSP_CLI_HPP

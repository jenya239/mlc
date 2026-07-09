#include "lsp_cli.hpp"

#include "server.hpp"

namespace lsp_cli {

using namespace server;

mlc::String lsp_usage_message() noexcept;

int run_lsp_command() noexcept;

mlc::String lsp_usage_message() noexcept{return mlc::String("Usage: mlcc lsp\n       Language Server (stdio JSON-RPC)");}

int run_lsp_command() noexcept{return server::run_lsp_server();}

} // namespace lsp_cli

#include "mlc.hpp"

#define main mlc_user_main
#include "main.hpp"

#include "cli.hpp"

namespace mlc_main {

using namespace cli;

int main(int argc, char** argv) noexcept;

int main(int argc, char** argv) noexcept{return cli::run_compiler_cli();}

} // namespace mlc_main

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) {
    arguments.emplace_back(argv[i]);
  }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::mlc_main::mlc_user_main(argc, argv);
}

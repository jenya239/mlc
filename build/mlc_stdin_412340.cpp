#include "mlc.hpp"

#define main mlc_user_main
#include "mlc_stdin_412340.hpp"

namespace mlc_stdin_412340 {

int main(int argc, char** argv) noexcept{return mlc::io::println(mlc::String("from-stdin"));}

} // namespace mlc_stdin_412340

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
  return ::mlc_stdin_412340::mlc_user_main(argc, argv);
}

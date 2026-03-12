#include "mlc.hpp"

#define main mlc_user_main
#include "array.hpp"

namespace array {

constexpr int count(mlc::Array<mlc::String> items) noexcept{return items.size();}

int main(int argc, char** argv) noexcept{return array::count(mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b"), mlc::String("c")});}

} // namespace array

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
  return ::array::mlc_user_main(argc, argv);
}

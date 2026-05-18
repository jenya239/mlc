#define main mlc_user_main
#include "fibonacci.hpp"

namespace fibonacci {

int fib(int n) noexcept{
if ((n <= 1)) {
return n;
} else {
return (fib((n - 1)) + fib((n - 2)));
}
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::io::println(mlc::to_string(fib(10)));
return 0;
}

} // namespace fibonacci

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::fibonacci::mlc_user_main(argc, argv);
}

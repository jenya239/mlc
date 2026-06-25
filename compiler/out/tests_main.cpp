#include "mlc.hpp"

#define main mlc_user_main
#include "tests_main.hpp"

#include "test_runner.hpp"
#include "suite_registry.hpp"

namespace tests_main {

using namespace test_runner;
using namespace suite_registry;

mlc::Array<test_runner::TestResult> all_tests() noexcept;

int main(int argc, char** argv) noexcept;

mlc::Array<test_runner::TestResult> all_tests() noexcept{return suite_registry::all_test_suites();}

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::io::print(test_runner::run_all(all_tests()));
return 0;
}

} // namespace tests_main

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
  return ::tests_main::mlc_user_main(argc, argv);
}

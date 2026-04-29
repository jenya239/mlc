#include "mlc.hpp"
#define main mlc_user_main
#define main mlc_user_main
#include "tests_main.hpp"

namespace tests_main {

mlc::Array<test_runner::TestResult> all_tests() noexcept{
auto all = mlc::Array<test_runner::TestResult>{};
auto index = 0;
auto suite_lex = test_lexer::lexer_tests();
index = 0;
while ((index < suite_lex.length())) {
all.push_back(suite_lex[index]);
index = (index + 1);
}
auto suite_parse = test_parser::parser_tests();
index = 0;
while ((index < suite_parse.length())) {
all.push_back(suite_parse[index]);
index = (index + 1);
}
auto suite_check = test_checker::checker_tests();
index = 0;
while ((index < suite_check.length())) {
all.push_back(suite_check[index]);
index = (index + 1);
}
auto suite_gen = test_codegen::codegen_tests();
index = 0;
while ((index < suite_gen.length())) {
all.push_back(suite_gen[index]);
index = (index + 1);
}
auto suite_pipe = test_pipe_and_record_update::pipe_and_record_update_tests();
index = 0;
while ((index < suite_pipe.length())) {
all.push_back(suite_pipe[index]);
index = (index + 1);
}
auto suite_decl = test_decl_gen::decl_gen_tests();
index = 0;
while ((index < suite_decl.length())) {
all.push_back(suite_decl[index]);
index = (index + 1);
}
return all;
}
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
  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::tests_main::mlc_user_main(argc, argv);
}
#undef main
static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) arguments.emplace_back(argv[i]);
  mlc::io::set_args(std::move(arguments));
}
int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::tests_main::mlc_user_main(argc, argv);
}

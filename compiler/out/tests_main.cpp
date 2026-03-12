#include "mlc.hpp"

#define main mlc_user_main
#include "tests_main.hpp"

#include "test_lexer.hpp"
#include "test_parser.hpp"
#include "test_checker.hpp"
#include "test_codegen.hpp"
#include "test_pipe_and_record_update.hpp"
#include "test_runner.hpp"

namespace tests_main {

using namespace test_lexer;
using namespace test_parser;
using namespace test_checker;
using namespace test_codegen;
using namespace test_pipe_and_record_update;
using namespace test_runner;

mlc::Array<test_runner::TestResult> all_tests() noexcept;

int main() noexcept;

mlc::Array<test_runner::TestResult> all_tests() noexcept{
mlc::Array<test_runner::TestResult> all = {};
int i = 0;
mlc::Array<test_runner::TestResult> lex_results = test_lexer::lexer_tests();
while (i < lex_results.size()){
{
all.push_back(lex_results[i]);
i = i + 1;
}
}
mlc::Array<test_runner::TestResult> parse_results = test_parser::parser_tests();
i = 0;
while (i < parse_results.size()){
{
all.push_back(parse_results[i]);
i = i + 1;
}
}
mlc::Array<test_runner::TestResult> check_results = test_checker::checker_tests();
i = 0;
while (i < check_results.size()){
{
all.push_back(check_results[i]);
i = i + 1;
}
}
mlc::Array<test_runner::TestResult> gen_results = test_codegen::codegen_tests();
i = 0;
while (i < gen_results.size()){
{
all.push_back(gen_results[i]);
i = i + 1;
}
}
mlc::Array<test_runner::TestResult> pipe_record_results = test_pipe_and_record_update::pipe_and_record_update_tests();
i = 0;
while (i < pipe_record_results.size()){
{
all.push_back(pipe_record_results[i]);
i = i + 1;
}
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
  for (int i = 1; i < argc; ++i) {
    arguments.emplace_back(argv[i]);
  }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::tests_main::mlc_user_main(argc, argv);
}

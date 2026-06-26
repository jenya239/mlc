#include "mlc.hpp"

#define main mlc_user_main
#include "cpp_parse_count.hpp"

#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"

namespace cpp_parse_count {

using namespace cpp_lexer;
using namespace cpp_parser;

int main(int argc, char** argv) noexcept;

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::Array<mlc::String> command_line_arguments = mlc::io::args();
return command_line_arguments.size() < 1 ? 1 : [&]() -> int { 
  mlc::String source_text = mlc::file::read_to_string(command_line_arguments[0]);
  int declaration_count = cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program.declarations.size();
  mlc::io::print(mlc::to_string(declaration_count));
  return 0;
 }();
}

} // namespace cpp_parse_count

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
  return ::cpp_parse_count::mlc_user_main(argc, argv);
}

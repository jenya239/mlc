#define main mlc_user_main
#include "format_cli.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast_printer.hpp"

namespace format_cli {

using namespace ast;
using namespace lexer;
using namespace decls;
using namespace ast_printer;
using namespace ast_tokens;

mlc::String format_usage_message() noexcept{
  return mlc::String("Usage: mlcc fmt <source.mlc>", 28);
}
ast::Result<mlc::String, mlc::Array<mlc::String>> format_mlc_source_text(mlc::String source, mlc::String source_path) noexcept{
  auto lexer_output = lexer::tokenize(source);
  if (ast_tokens::LexOut_has_errors(lexer_output))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("lex: ", 5) + mlc::to_string(lexer_output.errors[0])) + mlc::String("", 0))}};
  } else   {
    auto parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
    auto parse_error_count = parse_parsed.errors.length();
    if ((parse_error_count > 0))     {
      return ast::Err<mlc::Array<mlc::String>>{parse_parsed.errors};
    } else     {
      return ast::Ok<mlc::String>{ast_printer::print_mlc_program(parse_parsed.program)};
    }
  }
}
ast::Result<mlc::String, mlc::Array<mlc::String>> file_not_found_error(mlc::String source_path) noexcept{
  return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("file not found: ", 16) + mlc::to_string(source_path)) + mlc::String("", 0))}};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> format_source_file(mlc::String source_path) noexcept{
  auto source = mlc::file::read_to_string(source_path);
  if (((source.length() == 0) && (!mlc::file::exists(source_path))))   {
    return file_not_found_error(source_path);
  } else   {
    return format_mlc_source_text(source, source_path);
  }
}

} // namespace format_cli

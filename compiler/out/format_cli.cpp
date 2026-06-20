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

mlc::String format_usage_message() noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> format_mlc_source_text(mlc::String source, mlc::String source_path) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> file_not_found_error(mlc::String source_path) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> format_source_file(mlc::String source_path) noexcept;

mlc::String format_usage_message() noexcept{return mlc::String("Usage: mlcc fmt <source.mlc>");}

ast::Result<mlc::String, mlc::Array<mlc::String>> format_mlc_source_text(mlc::String source, mlc::String source_path) noexcept{
ast_tokens::LexOut lexer_output = lexer::tokenize(source);
return ast_tokens::LexOut_has_errors(lexer_output) ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("lex: ") + lexer_output.errors[0]})) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  predicates::ProgramParseValue parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
  int parse_error_count = parse_parsed.errors.size();
  return parse_error_count > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(parse_parsed.errors)) : ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(ast_printer::print_mlc_program(parse_parsed.program)));
 }());
}

ast::Result<mlc::String, mlc::Array<mlc::String>> file_not_found_error(mlc::String source_path) noexcept{return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("file not found: ") + source_path});}

ast::Result<mlc::String, mlc::Array<mlc::String>> format_source_file(mlc::String source_path) noexcept{
mlc::String source = mlc::file::read_to_string(source_path);
return source.length() == 0 && !mlc::file::exists(source_path) ? ast::Result<mlc::String, mlc::Array<mlc::String>>(file_not_found_error(source_path)) : ast::Result<mlc::String, mlc::Array<mlc::String>>(format_mlc_source_text(source, source_path));
}

} // namespace format_cli

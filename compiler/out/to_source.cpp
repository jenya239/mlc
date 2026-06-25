#include "to_source.hpp"

#include "cpp_ast.hpp"
#include "print.hpp"
#include "cpp_tokens.hpp"
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_decls.hpp"
#include "cpp_predicates.hpp"

namespace to_source {

using namespace cpp_ast;
using namespace print;
using namespace cpp_tokens;
using namespace cpp_lexer;
using namespace cpp_parser;
using namespace cpp_decls;
using namespace cpp_predicates;

mlc::String cpp_tokens_span_to_source(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;

bool cpp_to_source_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String cpp_declaration_to_source(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String cpp_program_to_source(cpp_ast::CppProgram program) noexcept;

cpp_ast::CppProgram cpp_program_from_tokens_preserving_trivia(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_source_roundtrip_preserving_trivia(mlc::String source_text) noexcept;

mlc::String cpp_parse_to_source_canonical(mlc::String source_text) noexcept;

mlc::String cpp_tokens_span_to_source(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept{
mlc::Array<mlc::String> parts = {};
int index = start;
while (index < end_position && index < tokens.size()){
{
parts.push_back(cpp_tokens::cpp_token_to_source(tokens[index]));
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

bool cpp_to_source_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*declaration))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*declaration)); auto [is_angle, path] = _v_cppinclude; return is_angle && path == mlc::String("__skip__"); } return false; }();}

mlc::String cpp_declaration_to_source(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppDeclarationFragment>((*declaration))) { auto _v_cppdeclarationfragment = std::get<cpp_ast::CppDeclarationFragment>((*declaration)); auto [code] = _v_cppdeclarationfragment; return code; } return print::print_decl(declaration); }();}

mlc::String cpp_program_to_source(cpp_ast::CppProgram program) noexcept{return program.declarations.map([](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return cpp_declaration_to_source(declaration); }).join(mlc::String(""));}

cpp_ast::CppProgram cpp_program_from_tokens_preserving_trivia(mlc::Array<cpp_tokens::CppToken> tokens) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = {};
int position = 0;
while (!cpp_predicates::cpp_at_end(tokens, position)){
{
int span_start = position;
cpp_decls::CppDeclarationParseResult parse_result = cpp_decls::parse_cpp_declaration(tokens, position);
if (!cpp_to_source_is_skip(parse_result.declaration)){
{
mlc::String fragment = cpp_tokens_span_to_source(tokens, span_start, parse_result.position);
if (fragment != mlc::String("")){
declarations.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationFragment(fragment)));
}
}
}
if (cpp_to_source_is_skip(parse_result.declaration) && parse_result.position == position){
{
position = position + 1;
}
} else {
{
position = parse_result.position;
}
}
}
}
return cpp_ast::CppProgram{declarations};
}

bool cpp_source_roundtrip_preserving_trivia(mlc::String source_text) noexcept{
mlc::Array<cpp_tokens::CppToken> tokens = cpp_lexer::cpp_tokenize(source_text).tokens;
cpp_ast::CppProgram program = cpp_program_from_tokens_preserving_trivia(tokens);
return cpp_program_to_source(program) == source_text;
}

mlc::String cpp_parse_to_source_canonical(mlc::String source_text) noexcept{return cpp_program_to_source(cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program);}

} // namespace to_source

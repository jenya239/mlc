#include "cpp_parser.hpp"

#include "cpp_decls.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"

namespace cpp_parser {

using namespace cpp_decls;
using namespace cpp_ast;
using namespace cpp_tokens;
using namespace cpp_predicates;

bool cpp_is_skip_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

cpp_parser::CppParseResult cpp_parse(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_is_skip_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*declaration))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*declaration)); auto [is_angle, path] = _v_cppinclude; return is_angle && path == mlc::String("__skip__"); } return false; }();}

cpp_parser::CppParseResult cpp_parse(mlc::Array<cpp_tokens::CppToken> tokens) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = {};
mlc::Array<mlc::String> errors = {};
int position = 0;
while (!cpp_predicates::cpp_at_end(tokens, position)){
{
cpp_decls::CppDeclarationParseResult parse_result = cpp_decls::parse_cpp_declaration(tokens, position);
if (parse_result.error_message != mlc::String("")){
{
errors.push_back(parse_result.error_message);
}
}
if (cpp_is_skip_declaration(parse_result.declaration) && parse_result.position == position){
{
position = position + 1;
}
} else {
{
if (!cpp_is_skip_declaration(parse_result.declaration)){
{
declarations.push_back(parse_result.declaration);
}
}
position = parse_result.position;
}
}
}
}
return cpp_parser::CppParseResult{cpp_ast::CppProgram{declarations}, errors};
}

} // namespace cpp_parser

#define main mlc_user_main
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

bool cpp_is_skip_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<cpp_ast::CppInclude>((*__match_subject))) {
const cpp_ast::CppInclude& cppInclude = std::get<cpp_ast::CppInclude>((*__match_subject));
auto [is_angle, path] = cppInclude; return (is_angle && (path == mlc::String("__skip__", 8)));
}
return false;
std::abort();
}();
}
CppParseResult cpp_parse(mlc::Array<cpp_tokens::CppToken> tokens) noexcept{
  auto declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto errors = mlc::Array<mlc::String>{};
  auto position = 0;
  while ((!cpp_predicates::cpp_at_end(tokens, position)))   {
    auto parse_result = cpp_decls::parse_cpp_declaration(tokens, position);
    if ((parse_result.error_message != mlc::String("", 0)))     {
      errors.push_back(parse_result.error_message);
    }
    if ((cpp_is_skip_declaration(parse_result.declaration) && (parse_result.position == position)))     {
      (position = mlc::arith::checked_add(position, 1));
    } else if ((!cpp_is_skip_declaration(parse_result.declaration)))     {
      declarations.push_back(parse_result.declaration);
      (position = parse_result.position);
    } else     {
      (position = parse_result.position);
    }
  }
  return CppParseResult{cpp_ast::CppProgram{declarations}, errors};
}

} // namespace cpp_parser

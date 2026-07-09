#define main mlc_user_main
#include "module_tu_helpers.hpp"

#include "cpp_naming.hpp"
#include "cpp_ast.hpp"

namespace module_tu_helpers {

using namespace cpp_naming;
using namespace cpp_ast;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> push_parsed_include_line(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String line) noexcept{
  if ((((line.length() >= 10) && (line.substring(0, 10) == mlc::String("#include \"", 10))) && (line.char_at((line.length() - 1)) == mlc::String("\"", 1))))   {
    declarations.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{false, line.substring(10, (line.length() - 11))}));
  } else if ((((line.length() >= 10) && (line.substring(0, 10) == mlc::String("#include <", 10))) && (line.char_at((line.length() - 1)) == mlc::String(">", 1))))   {
    declarations.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{true, line.substring(10, (line.length() - 11))}));
  }
  return declarations;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations_from_include_text(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String include_text) noexcept{
  auto result = declarations;
  auto start = 0;
  auto index = 0;
  while ((index < include_text.length()))   {
    if ((include_text.char_at(index) == mlc::String("\n", 1)))     {
      if ((index > start))       {
        (result = push_parsed_include_line(result, include_text.substring(start, (index - start))));
      }
      (start = (index + 1));
    }
    (index = (index + 1));
  }
  if ((start < include_text.length()))   {
    (result = push_parsed_include_line(result, include_text.substring(start, (include_text.length() - start))));
  }
  return result;
}
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_ifndef_cpp_declaration(mlc::String guard_macro) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppIfndef{guard_macro});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_define_cpp_declaration(mlc::String guard_macro) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDefineMacro{guard_macro, mlc::String("", 0)});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_include_guard_endif_cpp_declaration(mlc::String guard_macro) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppEndif{guard_macro});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_namespace_open_cpp_declaration(mlc::String namespace_identifier) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppNamespaceBegin{namespace_identifier});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_namespace_close_cpp_declaration(mlc::String namespace_identifier) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppNamespaceEnd{namespace_identifier});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_define_main_as_user_main_cpp_declaration() noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDefineMacro{mlc::String("main", 4), mlc::String("mlc_user_main", 13)});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_quoted_include_cpp_declaration(mlc::String header_filename) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{false, header_filename});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_blank_line_cpp_declaration() noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppBlankLine{});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_using_namespace_cpp_declaration(mlc::String namespace_identifier) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppUsingNamespace{namespace_identifier});
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_using_namespace_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::Array<mlc::String> import_paths) noexcept{
  auto result = declarations;
  auto namespace_names = cpp_naming::using_namespace_names(import_paths);
  auto index = 0;
  while ((index < namespace_names.length()))   {
    result.push_back(make_using_namespace_cpp_declaration(namespace_names[index]));
    (index = (index + 1));
  }
  return result;
}
std::shared_ptr<cpp_ast::CppDeclaration> make_entry_host_main_cpp_declaration(mlc::String qualified_namespace) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppHostEntryMain{qualified_namespace});
}

} // namespace module_tu_helpers

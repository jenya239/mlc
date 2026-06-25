#include "verify_cpp_ast.hpp"

#include "cpp_ast.hpp"

namespace verify_cpp_ast {

using namespace cpp_ast;

bool cpp_identifier_is_safe(mlc::String name) noexcept;

bool cpp_include_path_is_safe(mlc::String path) noexcept;

mlc::Array<mlc::String> verify_cpp_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;

mlc::Array<mlc::String> verify_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::Array<mlc::String> verify_cpp_program(cpp_ast::CppProgram program) noexcept;

bool cpp_identifier_is_safe(mlc::String name) noexcept{
return name == mlc::String("") || name.length() > 256 ? false : [&]() -> bool { 
  int index = 0;
  while (index < name.length()){
{
mlc::String character = name.char_at(index);
if (character == mlc::String(";") || character == mlc::String("\n") || character == mlc::String("\r") || character == mlc::String("\"") || character == mlc::String("'")){
{
return false;
}
}
index = index + 1;
}
}
  return true;
 }();
}

bool cpp_include_path_is_safe(mlc::String path) noexcept{
return path == mlc::String("") || path.length() > 512 ? false : path.contains(mlc::String("..")) ? false : [&]() -> bool { 
  int index = 0;
  while (index < path.length()){
{
mlc::String character = path.char_at(index);
if (character == mlc::String(";") || character == mlc::String("\n") || character == mlc::String("\r") || character == mlc::String("\"")){
{
return false;
}
}
index = index + 1;
}
}
  return true;
 }();
}

mlc::Array<mlc::String> verify_cpp_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
mlc::Array<mlc::String> combined = accumulator;
int index = 0;
while (index < more.size()){
{
combined.push_back(more[index]);
index = index + 1;
}
}
return combined;
}

mlc::Array<mlc::String> verify_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<mlc::String> errors = {};
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<cpp_ast::CppInclude>((*declaration))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*declaration)); auto [_w0, path] = _v_cppinclude; return !cpp_include_path_is_safe(path) ? mlc::Array<mlc::String>{mlc::String("cpp include: unsafe path ") + path} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppStruct>((*declaration))) { auto _v_cppstruct = std::get<cpp_ast::CppStruct>((*declaration)); auto [_w0, name, _w1, _w2] = _v_cppstruct; return !cpp_identifier_is_safe(name) ? mlc::Array<mlc::String>{mlc::String("cpp struct: unsafe name ") + name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppForwardDecl>((*declaration))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*declaration)); auto [_w0, name] = _v_cppforwarddecl; return !cpp_identifier_is_safe(name) ? mlc::Array<mlc::String>{mlc::String("cpp forward decl: unsafe name ") + name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppUsing>((*declaration))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*declaration)); auto [alias, _w0] = _v_cppusing; return !cpp_identifier_is_safe(alias) ? mlc::Array<mlc::String>{mlc::String("cpp using: unsafe alias ") + alias} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppNamespace>((*declaration))) { auto _v_cppnamespace = std::get<cpp_ast::CppNamespace>((*declaration)); auto [name, inner] = _v_cppnamespace; return [&]() -> mlc::Array<mlc::String> { 
  if (!cpp_identifier_is_safe(name)){
{
errors.push_back(mlc::String("cpp namespace: unsafe name ") + name);
}
}
  int index = 0;
  while (index < inner.size()){
{
errors = verify_cpp_append_errors(errors, verify_cpp_declaration(inner[index]));
index = index + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<cpp_ast::CppNamespaceBegin>((*declaration))) { auto _v_cppnamespacebegin = std::get<cpp_ast::CppNamespaceBegin>((*declaration)); auto [name] = _v_cppnamespacebegin; return !cpp_identifier_is_safe(name) ? mlc::Array<mlc::String>{mlc::String("cpp namespace begin: unsafe name ") + name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppFnProto>((*declaration))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*declaration)); auto [_w0, name, _w1, _w2] = _v_cppfnproto; return !cpp_identifier_is_safe(name) ? mlc::Array<mlc::String>{mlc::String("cpp fn proto: unsafe name ") + name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppFnDef>((*declaration))) { auto _v_cppfndef = std::get<cpp_ast::CppFnDef>((*declaration)); auto [_w0, name, _w1, _w2, _w3, _w4] = _v_cppfndef; return !cpp_identifier_is_safe(name) ? mlc::Array<mlc::String>{mlc::String("cpp fn def: unsafe name ") + name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*declaration))) { auto _v_cppclassdeclaration = std::get<cpp_ast::CppClassDeclaration>((*declaration)); auto [definition] = _v_cppclassdeclaration; return !cpp_identifier_is_safe(definition.name) ? mlc::Array<mlc::String>{mlc::String("cpp class: unsafe name ") + definition.name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppTypedefDeclaration>((*declaration))) { auto _v_cpptypedefdeclaration = std::get<cpp_ast::CppTypedefDeclaration>((*declaration)); auto [alias, _w0] = _v_cpptypedefdeclaration; return !cpp_identifier_is_safe(alias) ? mlc::Array<mlc::String>{mlc::String("cpp typedef: unsafe alias ") + alias} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppFunctionPrototypeDecl>((*declaration))) { auto _v_cppfunctionprototypedecl = std::get<cpp_ast::CppFunctionPrototypeDecl>((*declaration)); auto [prototype] = _v_cppfunctionprototypedecl; return !cpp_identifier_is_safe(prototype.name) ? mlc::Array<mlc::String>{mlc::String("cpp fn prototype: unsafe name ") + prototype.name} : mlc::Array<mlc::String>{}; } if (std::holds_alternative<cpp_ast::CppExternBlock>((*declaration))) { auto _v_cppexternblock = std::get<cpp_ast::CppExternBlock>((*declaration)); auto [_w0, inner] = _v_cppexternblock; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < inner.size()){
{
errors = verify_cpp_append_errors(errors, verify_cpp_declaration(inner[index]));
index = index + 1;
}
}
  return errors;
 }(); } return errors; }();
}

mlc::Array<mlc::String> verify_cpp_program(cpp_ast::CppProgram program) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < program.declarations.size()){
{
errors = verify_cpp_append_errors(errors, verify_cpp_declaration(program.declarations[index]));
index = index + 1;
}
}
return errors;
}

} // namespace verify_cpp_ast

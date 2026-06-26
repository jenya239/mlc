#include "cpp_naming.hpp"

namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String map_builtin_identifier_reference(mlc::String name) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

bool is_stdlib_import_path(mlc::String path) noexcept;

mlc::Array<mlc::String> using_namespace_names(mlc::Array<mlc::String> import_paths) noexcept;

mlc::String using_namespace_lines(mlc::Array<mlc::String> import_paths) noexcept;

mlc::String path_to_module_base(mlc::String path) noexcept{
int last_slash = -1;
int last_dot = path.length();
int i = 0;
while (i < path.length()){
{
if (path.char_at(i) == mlc::String("/")){
{
last_slash = i;
}
} else {
{
if (path.char_at(i) == mlc::String(".") && i > last_slash){
last_dot = i;
}
}
}
i = i + 1;
}
}
return last_dot > last_slash ? path.substring(last_slash + 1, last_dot - last_slash - 1) : path.substring(last_slash + 1, path.length() - last_slash - 1);
}

bool cpp_keyword(mlc::String name) noexcept{return [&]() { if (name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class")) { return true; } if (name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return")) { return true; } if (name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do")) { return true; } if (name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue")) { return true; } if (name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete")) { return true; } if (name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private")) { return true; } if (name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline")) { return true; } if (name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union")) { return true; } if (name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool")) { return true; } if (name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double")) { return true; } if (name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof")) { return true; } if (name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq")) { return true; } if (name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl")) { return true; } if (name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval")) { return true; } if (name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype")) { return true; } if (name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept")) { return true; } if (name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast")) { return true; } if (name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t")) { return true; } return false; }();}

mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String map_method(mlc::String method_name) noexcept{return [&]() -> mlc::String { if (method_name == mlc::String("length") || method_name == mlc::String("len")) { return mlc::String("length"); } if (method_name == mlc::String("push")) { return mlc::String("push_back"); } if (method_name == mlc::String("recv")) { return mlc::String("receive"); } if (method_name == mlc::String("to_string")) { return mlc::String("to_string"); } if (method_name == mlc::String("to_int") || method_name == mlc::String("to_i")) { return mlc::String("to_i"); } if (method_name == mlc::String("upper") || method_name == mlc::String("to_upper")) { return mlc::String("upper"); } if (method_name == mlc::String("lower") || method_name == mlc::String("to_lower")) { return mlc::String("lower"); } return method_name; }();}

mlc::String map_builtin(mlc::String name) noexcept{return [&]() -> mlc::String { if (name == mlc::String("print")) { return mlc::String("mlc::io::print"); } if (name == mlc::String("println")) { return mlc::String("mlc::io::println"); } if (name == mlc::String("exit")) { return mlc::String("mlc::io::exit"); } if (name == mlc::String("args")) { return mlc::String("mlc::io::args"); } if (name == mlc::String("read_line")) { return mlc::String("mlc::io::read_line"); } if (name == mlc::String("spawn_task")) { return mlc::String("mlc::concurrency::spawn_task"); } if (name == mlc::String("make_channel")) { return mlc::String("mlc::concurrency::make_channel"); } if (name == mlc::String("is_ready")) { return mlc::String("mlc::is_ready"); } if (name == mlc::String("block_on")) { return mlc::String("mlc::block_on"); } return name; }();}

mlc::String map_builtin_identifier_reference(mlc::String name) noexcept{return name == mlc::String("args") ? name : map_builtin(name);}

mlc::String escape_str(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < input.length()){
{
mlc::String ch = input.char_at(i);
if (ch == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (ch == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (ch == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (ch == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (ch == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
if (ch == mlc::String("\0", 1)){
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(ch);
}
}
}
}
}
}
}
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{return type_params.size() > 0 ? mlc::String("template<typename ") + type_params.join(mlc::String(", typename ")) + mlc::String(">\n") : mlc::String("");}

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{return import_paths.map([](mlc::String path) mutable { return mlc::String("#include ") + mlc::String("\"") + path_to_module_base(path) + mlc::String(".hpp") + mlc::String("\"") + mlc::String("\n"); }).join(mlc::String(""));}

bool is_stdlib_import_path(mlc::String path) noexcept{
return path.length() >= 4 && path.substring(0, 4) == mlc::String("std/") ? true : [&]() -> bool { 
  mlc::String base = path_to_module_base(path);
  return base == mlc::String("Option") || base == mlc::String("Result") || base == mlc::String("Array") || base == mlc::String("String") || base == mlc::String("Conv") || base == mlc::String("IO") || base == mlc::String("File") || base == mlc::String("Json") || base == mlc::String("Math") || base == mlc::String("Graphics");
 }();
}

mlc::Array<mlc::String> using_namespace_names(mlc::Array<mlc::String> import_paths) noexcept{
mlc::Array<mlc::String> names = {};
bool has_ast = false;
int index = 0;
while (index < import_paths.size()){
{
mlc::String path = import_paths[index];
mlc::String base = path_to_module_base(path);
if (!is_stdlib_import_path(path) && base.length() > 0){
{
names.push_back(base);
if (base == mlc::String("ast")){
has_ast = true;
}
}
}
index = index + 1;
}
}
if (has_ast){
{
names.push_back(mlc::String("ast_tokens"));
}
}
return names;
}

mlc::String using_namespace_lines(mlc::Array<mlc::String> import_paths) noexcept{
mlc::String lines = mlc::String("");
int index = 0;
mlc::Array<mlc::String> names = using_namespace_names(import_paths);
while (index < names.size()){
{
lines = lines + mlc::String("using namespace ") + names[index] + mlc::String(";\n");
index = index + 1;
}
}
return lines;
}

} // namespace cpp_naming

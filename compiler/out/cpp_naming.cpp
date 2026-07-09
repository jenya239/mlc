#include "cpp_naming.hpp"

namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept;

mlc::Array<mlc::String> cpp_reserved_keywords() noexcept;

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

mlc::Array<mlc::String> cpp_reserved_keywords() noexcept{
mlc::Array<mlc::String> keywords = mlc::Array<mlc::String>{mlc::String("export"), mlc::String("import"), mlc::String("module"), mlc::String("struct"), mlc::String("class"), mlc::String("template"), mlc::String("typename"), mlc::String("const"), mlc::String("return"), mlc::String("if"), mlc::String("else"), mlc::String("for"), mlc::String("while"), mlc::String("do"), mlc::String("switch"), mlc::String("case"), mlc::String("default"), mlc::String("break"), mlc::String("continue"), mlc::String("try"), mlc::String("catch"), mlc::String("throw"), mlc::String("new"), mlc::String("delete"), mlc::String("virtual"), mlc::String("override"), mlc::String("final"), mlc::String("public"), mlc::String("private"), mlc::String("protected"), mlc::String("friend"), mlc::String("static"), mlc::String("extern"), mlc::String("inline"), mlc::String("using"), mlc::String("namespace"), mlc::String("typedef"), mlc::String("enum"), mlc::String("union"), mlc::String("true"), mlc::String("false"), mlc::String("nullptr"), mlc::String("void"), mlc::String("bool"), mlc::String("int"), mlc::String("long"), mlc::String("short"), mlc::String("char"), mlc::String("float"), mlc::String("double"), mlc::String("signed"), mlc::String("unsigned"), mlc::String("auto"), mlc::String("typeid"), mlc::String("sizeof"), mlc::String("and"), mlc::String("or"), mlc::String("not"), mlc::String("xor"), mlc::String("and_eq"), mlc::String("or_eq"), mlc::String("not_eq"), mlc::String("xor_eq"), mlc::String("bitand"), mlc::String("bitor"), mlc::String("compl"), mlc::String("alignas"), mlc::String("alignof"), mlc::String("asm"), mlc::String("concept"), mlc::String("consteval"), mlc::String("constexpr"), mlc::String("constinit"), mlc::String("const_cast"), mlc::String("decltype"), mlc::String("dynamic_cast"), mlc::String("explicit"), mlc::String("mutable"), mlc::String("noexcept"), mlc::String("operator"), mlc::String("reinterpret_cast"), mlc::String("requires"), mlc::String("static_cast"), mlc::String("this"), mlc::String("thread_local"), mlc::String("volatile"), mlc::String("wchar_t")};
return keywords;
}

bool cpp_keyword(mlc::String name) noexcept{return cpp_reserved_keywords().any([name](mlc::String keyword) mutable { return name == keyword; });}

mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") ? mlc::String("length") : method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("recv") ? mlc::String("receive") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") ? mlc::String("to_i") : method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") ? mlc::String("upper") : method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") ? mlc::String("lower") : method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}

mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("eprintln") ? mlc::String("mlc::io::eprintln") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name == mlc::String("read_line") ? mlc::String("mlc::io::read_line") : name == mlc::String("spawn_task") ? mlc::String("mlc::concurrency::spawn_task") : name == mlc::String("make_channel") ? mlc::String("mlc::concurrency::make_channel") : name == mlc::String("is_ready") ? mlc::String("mlc::is_ready") : name == mlc::String("block_on") ? mlc::String("mlc::block_on") : name;}

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

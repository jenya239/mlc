#include "cpp_naming.hpp"

namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

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

bool cpp_keyword(mlc::String name) noexcept{return name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class") || name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return") || name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do") || name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue") || name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete") || name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private") || name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline") || name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union") || name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool") || name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double") || name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof") || name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq") || name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl") || name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval") || name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype") || name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept") || name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast") || name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t");}

mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") || method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") || method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") || method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}

mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}

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

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{
mlc::Array<mlc::String> lines = {};
int i = 0;
while (i < import_paths.size()){
{
lines.push_back(mlc::String("#include \"") + path_to_module_base(import_paths[i]) + mlc::String(".hpp\"\n"));
i = i + 1;
}
}
return lines.join(mlc::String(""));
}

} // namespace cpp_naming

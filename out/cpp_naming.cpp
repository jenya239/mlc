#define main mlc_user_main
#include "cpp_naming.hpp"

namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept{
auto last_slash = (-1);
auto last_dot = path.length();
auto i = 0;
while ((i < path.length())) {
if ((path.char_at(i) == mlc::String("/", 1))) {
last_slash = i;
} else if (((path.char_at(i) == mlc::String(".", 1)) && (i > last_slash))) {
last_dot = i;
}

i = (i + 1);
}
if ((last_dot > last_slash)) {
return path.substring((last_slash + 1), ((last_dot - last_slash) - 1));
} else {
return path.substring((last_slash + 1), ((path.length() - last_slash) - 1));
}
}
bool cpp_keyword(mlc::String name) noexcept{
return (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((name == mlc::String("export", 6)) || (name == mlc::String("import", 6))) || (name == mlc::String("module", 6))) || (name == mlc::String("struct", 6))) || (name == mlc::String("class", 5))) || (name == mlc::String("template", 8))) || (name == mlc::String("typename", 8))) || (name == mlc::String("const", 5))) || (name == mlc::String("return", 6))) || (name == mlc::String("if", 2))) || (name == mlc::String("else", 4))) || (name == mlc::String("for", 3))) || (name == mlc::String("while", 5))) || (name == mlc::String("do", 2))) || (name == mlc::String("switch", 6))) || (name == mlc::String("case", 4))) || (name == mlc::String("default", 7))) || (name == mlc::String("break", 5))) || (name == mlc::String("continue", 8))) || (name == mlc::String("try", 3))) || (name == mlc::String("catch", 5))) || (name == mlc::String("throw", 5))) || (name == mlc::String("new", 3))) || (name == mlc::String("delete", 6))) || (name == mlc::String("virtual", 7))) || (name == mlc::String("override", 8))) || (name == mlc::String("final", 5))) || (name == mlc::String("public", 6))) || (name == mlc::String("private", 7))) || (name == mlc::String("protected", 9))) || (name == mlc::String("friend", 6))) || (name == mlc::String("static", 6))) || (name == mlc::String("extern", 6))) || (name == mlc::String("inline", 6))) || (name == mlc::String("using", 5))) || (name == mlc::String("namespace", 9))) || (name == mlc::String("typedef", 7))) || (name == mlc::String("enum", 4))) || (name == mlc::String("union", 5))) || (name == mlc::String("true", 4))) || (name == mlc::String("false", 5))) || (name == mlc::String("nullptr", 7))) || (name == mlc::String("void", 4))) || (name == mlc::String("bool", 4))) || (name == mlc::String("int", 3))) || (name == mlc::String("long", 4))) || (name == mlc::String("short", 5))) || (name == mlc::String("char", 4))) || (name == mlc::String("float", 5))) || (name == mlc::String("double", 6))) || (name == mlc::String("signed", 6))) || (name == mlc::String("unsigned", 8))) || (name == mlc::String("auto", 4))) || (name == mlc::String("typeid", 6))) || (name == mlc::String("sizeof", 6))) || (name == mlc::String("and", 3))) || (name == mlc::String("or", 2))) || (name == mlc::String("not", 3))) || (name == mlc::String("xor", 3))) || (name == mlc::String("and_eq", 6))) || (name == mlc::String("or_eq", 5))) || (name == mlc::String("not_eq", 6))) || (name == mlc::String("xor_eq", 6))) || (name == mlc::String("bitand", 6))) || (name == mlc::String("bitor", 5))) || (name == mlc::String("compl", 5))) || (name == mlc::String("alignas", 7))) || (name == mlc::String("alignof", 7))) || (name == mlc::String("asm", 3))) || (name == mlc::String("concept", 7))) || (name == mlc::String("consteval", 9))) || (name == mlc::String("constexpr", 9))) || (name == mlc::String("constinit", 9))) || (name == mlc::String("const_cast", 10))) || (name == mlc::String("decltype", 8))) || (name == mlc::String("dynamic_cast", 12))) || (name == mlc::String("explicit", 8))) || (name == mlc::String("mutable", 7))) || (name == mlc::String("noexcept", 8))) || (name == mlc::String("operator", 8))) || (name == mlc::String("reinterpret_cast", 16))) || (name == mlc::String("requires", 8))) || (name == mlc::String("static_cast", 11))) || (name == mlc::String("this", 4))) || (name == mlc::String("thread_local", 12))) || (name == mlc::String("volatile", 8))) || (name == mlc::String("wchar_t", 7)));
}
mlc::String cpp_safe(mlc::String name) noexcept{
if (cpp_keyword(name)) {
return (name + mlc::String("_", 1));
} else {
return name;
}
}
mlc::String lower_first(mlc::String name) noexcept{
if ((name.length() == 0)) {
return name;
} else {
return (name.char_at(0).lower() + name.substring(1, name.length()));
}
}
mlc::String map_method(mlc::String method_name) noexcept{
if (((method_name == mlc::String("length", 6)) || (method_name == mlc::String("len", 3)))) {
return mlc::String("length", 6);
} else if ((method_name == mlc::String("push", 4))) {
return mlc::String("push_back", 9);
} else if ((method_name == mlc::String("to_string", 9))) {
return mlc::String("to_string", 9);
} else if (((method_name == mlc::String("to_int", 6)) || (method_name == mlc::String("to_i", 4)))) {
return mlc::String("to_i", 4);
} else if (((method_name == mlc::String("upper", 5)) || (method_name == mlc::String("to_upper", 8)))) {
return mlc::String("upper", 5);
} else if (((method_name == mlc::String("lower", 5)) || (method_name == mlc::String("to_lower", 8)))) {
return mlc::String("lower", 5);
} else {
return method_name;
}





}
mlc::String map_builtin(mlc::String name) noexcept{
if ((name == mlc::String("print", 5))) {
return mlc::String("mlc::io::print", 14);
} else if ((name == mlc::String("println", 7))) {
return mlc::String("mlc::io::println", 16);
} else if ((name == mlc::String("exit", 4))) {
return mlc::String("mlc::io::exit", 13);
} else if ((name == mlc::String("args", 4))) {
return mlc::String("mlc::io::args", 13);
} else {
return name;
}



}
mlc::String escape_str(mlc::String input) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < input.length())) {
auto ch = input.char_at(i);
if ((ch == mlc::String("\\", 1))) {
parts.push_back(mlc::String("\\\\", 2));
} else if ((ch == mlc::String("\"", 1))) {
parts.push_back(mlc::String("\\\"", 2));
} else if ((ch == mlc::String("\n", 1))) {
parts.push_back(mlc::String("\\n", 2));
} else if ((ch == mlc::String("\r", 1))) {
parts.push_back(mlc::String("\\r", 2));
} else if ((ch == mlc::String("\t", 1))) {
parts.push_back(mlc::String("\\t", 2));
} else if ((ch == mlc::String("\0", 1))) {
parts.push_back(mlc::String("\\0", 2));
} else {
parts.push_back(ch);
}





i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{
if ((type_params.length() > 0)) {
return ((mlc::String("template<typename ", 18) + type_params.join(mlc::String(", typename ", 11))) + mlc::String(">\n", 2));
} else {
return mlc::String("", 0);
}
}
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{
auto lines = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < import_paths.length())) {
lines.push_back(((mlc::String("#include \"", 10) + path_to_module_base(import_paths[i])) + mlc::String(".hpp\"\n", 6)));
i = (i + 1);
}
return lines.join(mlc::String("", 0));
}

} // namespace cpp_naming

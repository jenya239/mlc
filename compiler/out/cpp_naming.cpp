#define main mlc_user_main
#include "cpp_naming.hpp"

namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept{
  auto last_slash = (-1);
  auto last_dot = path.length();
  auto i = 0;
  while ((i < path.length()))   {
    if ((path.char_at(i) == mlc::String("/", 1)))     {
      (last_slash = i);
    } else if (((path.char_at(i) == mlc::String(".", 1)) && (i > last_slash)))     {
      (last_dot = i);
    }
    (i = (i + 1));
  }
  if ((last_dot > last_slash))   {
    return path.substring((last_slash + 1), ((last_dot - last_slash) - 1));
  } else   {
    return path.substring((last_slash + 1), ((path.length() - last_slash) - 1));
  }
}
bool cpp_keyword(mlc::String name) noexcept{
  return [&]() -> bool {
auto __match_subject = name;
if ((__match_subject == mlc::String("export", 6))) {
  return true;
} else if ((__match_subject == mlc::String("import", 6))) {
  return true;
} else if ((__match_subject == mlc::String("module", 6))) {
  return true;
} else if ((__match_subject == mlc::String("struct", 6))) {
  return true;
} else if ((__match_subject == mlc::String("class", 5))) {
  return true;
} else if ((__match_subject == mlc::String("template", 8))) {
  return true;
} else if ((__match_subject == mlc::String("typename", 8))) {
  return true;
} else if ((__match_subject == mlc::String("const", 5))) {
  return true;
} else if ((__match_subject == mlc::String("return", 6))) {
  return true;
} else if ((__match_subject == mlc::String("if", 2))) {
  return true;
} else if ((__match_subject == mlc::String("else", 4))) {
  return true;
} else if ((__match_subject == mlc::String("for", 3))) {
  return true;
} else if ((__match_subject == mlc::String("while", 5))) {
  return true;
} else if ((__match_subject == mlc::String("do", 2))) {
  return true;
} else if ((__match_subject == mlc::String("switch", 6))) {
  return true;
} else if ((__match_subject == mlc::String("case", 4))) {
  return true;
} else if ((__match_subject == mlc::String("default", 7))) {
  return true;
} else if ((__match_subject == mlc::String("break", 5))) {
  return true;
} else if ((__match_subject == mlc::String("continue", 8))) {
  return true;
} else if ((__match_subject == mlc::String("try", 3))) {
  return true;
} else if ((__match_subject == mlc::String("catch", 5))) {
  return true;
} else if ((__match_subject == mlc::String("throw", 5))) {
  return true;
} else if ((__match_subject == mlc::String("new", 3))) {
  return true;
} else if ((__match_subject == mlc::String("delete", 6))) {
  return true;
} else if ((__match_subject == mlc::String("virtual", 7))) {
  return true;
} else if ((__match_subject == mlc::String("override", 8))) {
  return true;
} else if ((__match_subject == mlc::String("final", 5))) {
  return true;
} else if ((__match_subject == mlc::String("public", 6))) {
  return true;
} else if ((__match_subject == mlc::String("private", 7))) {
  return true;
} else if ((__match_subject == mlc::String("protected", 9))) {
  return true;
} else if ((__match_subject == mlc::String("friend", 6))) {
  return true;
} else if ((__match_subject == mlc::String("static", 6))) {
  return true;
} else if ((__match_subject == mlc::String("extern", 6))) {
  return true;
} else if ((__match_subject == mlc::String("inline", 6))) {
  return true;
} else if ((__match_subject == mlc::String("using", 5))) {
  return true;
} else if ((__match_subject == mlc::String("namespace", 9))) {
  return true;
} else if ((__match_subject == mlc::String("typedef", 7))) {
  return true;
} else if ((__match_subject == mlc::String("enum", 4))) {
  return true;
} else if ((__match_subject == mlc::String("union", 5))) {
  return true;
} else if ((__match_subject == mlc::String("true", 4))) {
  return true;
} else if ((__match_subject == mlc::String("false", 5))) {
  return true;
} else if ((__match_subject == mlc::String("nullptr", 7))) {
  return true;
} else if ((__match_subject == mlc::String("void", 4))) {
  return true;
} else if ((__match_subject == mlc::String("bool", 4))) {
  return true;
} else if ((__match_subject == mlc::String("int", 3))) {
  return true;
} else if ((__match_subject == mlc::String("long", 4))) {
  return true;
} else if ((__match_subject == mlc::String("short", 5))) {
  return true;
} else if ((__match_subject == mlc::String("char", 4))) {
  return true;
} else if ((__match_subject == mlc::String("float", 5))) {
  return true;
} else if ((__match_subject == mlc::String("double", 6))) {
  return true;
} else if ((__match_subject == mlc::String("signed", 6))) {
  return true;
} else if ((__match_subject == mlc::String("unsigned", 8))) {
  return true;
} else if ((__match_subject == mlc::String("auto", 4))) {
  return true;
} else if ((__match_subject == mlc::String("typeid", 6))) {
  return true;
} else if ((__match_subject == mlc::String("sizeof", 6))) {
  return true;
} else if ((__match_subject == mlc::String("and", 3))) {
  return true;
} else if ((__match_subject == mlc::String("or", 2))) {
  return true;
} else if ((__match_subject == mlc::String("not", 3))) {
  return true;
} else if ((__match_subject == mlc::String("xor", 3))) {
  return true;
} else if ((__match_subject == mlc::String("and_eq", 6))) {
  return true;
} else if ((__match_subject == mlc::String("or_eq", 5))) {
  return true;
} else if ((__match_subject == mlc::String("not_eq", 6))) {
  return true;
} else if ((__match_subject == mlc::String("xor_eq", 6))) {
  return true;
} else if ((__match_subject == mlc::String("bitand", 6))) {
  return true;
} else if ((__match_subject == mlc::String("bitor", 5))) {
  return true;
} else if ((__match_subject == mlc::String("compl", 5))) {
  return true;
} else if ((__match_subject == mlc::String("alignas", 7))) {
  return true;
} else if ((__match_subject == mlc::String("alignof", 7))) {
  return true;
} else if ((__match_subject == mlc::String("asm", 3))) {
  return true;
} else if ((__match_subject == mlc::String("concept", 7))) {
  return true;
} else if ((__match_subject == mlc::String("consteval", 9))) {
  return true;
} else if ((__match_subject == mlc::String("constexpr", 9))) {
  return true;
} else if ((__match_subject == mlc::String("constinit", 9))) {
  return true;
} else if ((__match_subject == mlc::String("const_cast", 10))) {
  return true;
} else if ((__match_subject == mlc::String("decltype", 8))) {
  return true;
} else if ((__match_subject == mlc::String("dynamic_cast", 12))) {
  return true;
} else if ((__match_subject == mlc::String("explicit", 8))) {
  return true;
} else if ((__match_subject == mlc::String("mutable", 7))) {
  return true;
} else if ((__match_subject == mlc::String("noexcept", 8))) {
  return true;
} else if ((__match_subject == mlc::String("operator", 8))) {
  return true;
} else if ((__match_subject == mlc::String("reinterpret_cast", 16))) {
  return true;
} else if ((__match_subject == mlc::String("requires", 8))) {
  return true;
} else if ((__match_subject == mlc::String("static_cast", 11))) {
  return true;
} else if ((__match_subject == mlc::String("this", 4))) {
  return true;
} else if ((__match_subject == mlc::String("thread_local", 12))) {
  return true;
} else if ((__match_subject == mlc::String("volatile", 8))) {
  return true;
} else if ((__match_subject == mlc::String("wchar_t", 7))) {
  return true;
} else {
  return false;
}
}();
}
mlc::String cpp_safe(mlc::String name) noexcept{
  if (cpp_keyword(name))   {
    return ((mlc::String("", 0) + mlc::to_string(name)) + mlc::String("_", 1));
  } else   {
    return name;
  }
}
mlc::String lower_first(mlc::String name) noexcept{
  if ((name.length() == 0))   {
    return name;
  } else   {
    return ((((mlc::String("", 0) + mlc::to_string(name.char_at(0).lower())) + mlc::String("", 0)) + mlc::to_string(name.substring(1, name.length()))) + mlc::String("", 0));
  }
}
mlc::String map_method(mlc::String method_name) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method_name;
if ((__match_subject == mlc::String("length", 6))) {
  return mlc::String("length", 6);
} else if ((__match_subject == mlc::String("len", 3))) {
  return mlc::String("length", 6);
} else if ((__match_subject == mlc::String("push", 4))) {
  return mlc::String("push_back", 9);
} else if ((__match_subject == mlc::String("recv", 4))) {
  return mlc::String("receive", 7);
} else if ((__match_subject == mlc::String("to_string", 9))) {
  return mlc::String("to_string", 9);
} else if ((__match_subject == mlc::String("to_int", 6))) {
  return mlc::String("to_i", 4);
} else if ((__match_subject == mlc::String("to_i", 4))) {
  return mlc::String("to_i", 4);
} else if ((__match_subject == mlc::String("upper", 5))) {
  return mlc::String("upper", 5);
} else if ((__match_subject == mlc::String("to_upper", 8))) {
  return mlc::String("upper", 5);
} else if ((__match_subject == mlc::String("lower", 5))) {
  return mlc::String("lower", 5);
} else if ((__match_subject == mlc::String("to_lower", 8))) {
  return mlc::String("lower", 5);
} else {
  return method_name;
}
}();
}
mlc::String map_builtin(mlc::String name) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = name;
if ((__match_subject == mlc::String("print", 5))) {
  return mlc::String("mlc::io::print", 14);
} else if ((__match_subject == mlc::String("println", 7))) {
  return mlc::String("mlc::io::println", 16);
} else if ((__match_subject == mlc::String("exit", 4))) {
  return mlc::String("mlc::io::exit", 13);
} else if ((__match_subject == mlc::String("args", 4))) {
  return mlc::String("mlc::io::args", 13);
} else if ((__match_subject == mlc::String("read_line", 9))) {
  return mlc::String("mlc::io::read_line", 18);
} else if ((__match_subject == mlc::String("spawn_task", 10))) {
  return mlc::String("mlc::concurrency::spawn_task", 28);
} else if ((__match_subject == mlc::String("make_channel", 12))) {
  return mlc::String("mlc::concurrency::make_channel", 30);
} else if ((__match_subject == mlc::String("is_ready", 8))) {
  return mlc::String("mlc::is_ready", 13);
} else if ((__match_subject == mlc::String("block_on", 8))) {
  return mlc::String("mlc::block_on", 13);
} else {
  return name;
}
}();
}
mlc::String map_builtin_identifier_reference(mlc::String name) noexcept{
  if ((name == mlc::String("args", 4)))   {
    return name;
  } else   {
    return map_builtin(name);
  }
}
mlc::String escape_str(mlc::String input) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto i = 0;
  while ((i < input.length()))   {
    auto ch = input.char_at(i);
    if ((ch == mlc::String("\\", 1)))     {
      parts.push_back(mlc::String("\\\\", 2));
    } else if ((ch == mlc::String("\"", 1)))     {
      parts.push_back(mlc::String("\\\"", 2));
    } else if ((ch == mlc::String("\n", 1)))     {
      parts.push_back(mlc::String("\\n", 2));
    } else if ((ch == mlc::String("\r", 1)))     {
      parts.push_back(mlc::String("\\r", 2));
    } else if ((ch == mlc::String("\t", 1)))     {
      parts.push_back(mlc::String("\\t", 2));
    } else if ((ch == mlc::String("\0", 1)))     {
      parts.push_back(mlc::String("\\0", 2));
    } else     {
      parts.push_back(ch);
    }
    (i = (i + 1));
  }
  return parts.join(mlc::String("", 0));
}
mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{
  if ((type_params.length() > 0))   {
    return ((mlc::String("template<typename ", 18) + mlc::to_string(type_params.join(mlc::String(", typename ", 11)))) + mlc::String(">\n", 2));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{
  return import_paths.map([=](mlc::String path) mutable { return (((((mlc::String("#include ", 9) + mlc::String("\"", 1)) + path_to_module_base(path)) + mlc::String(".hpp", 4)) + mlc::String("\"", 1)) + mlc::String("\n", 1)); }).join(mlc::String("", 0));
}
bool is_stdlib_import_path(mlc::String path) noexcept{
  if (((path.length() >= 4) && (path.substring(0, 4) == mlc::String("std/", 4))))   {
    return true;
  } else   {
    auto base = path_to_module_base(path);
    return ((((((((((base == mlc::String("Option", 6)) || (base == mlc::String("Result", 6))) || (base == mlc::String("Array", 5))) || (base == mlc::String("String", 6))) || (base == mlc::String("Conv", 4))) || (base == mlc::String("IO", 2))) || (base == mlc::String("File", 4))) || (base == mlc::String("Json", 4))) || (base == mlc::String("Math", 4))) || (base == mlc::String("Graphics", 8)));
  }
}
mlc::Array<mlc::String> using_namespace_names(mlc::Array<mlc::String> import_paths) noexcept{
  auto names = mlc::Array<mlc::String>{};
  auto has_ast = false;
  auto index = 0;
  while ((index < import_paths.length()))   {
    auto path = import_paths[index];
    auto base = path_to_module_base(path);
    if (((!is_stdlib_import_path(path)) && (base.length() > 0)))     {
      names.push_back(base);
      if ((base == mlc::String("ast", 3)))       {
        (has_ast = true);
      }
    }
    (index = (index + 1));
  }
  if (has_ast)   {
    names.push_back(mlc::String("ast_tokens", 10));
  }
  return names;
}
mlc::String using_namespace_lines(mlc::Array<mlc::String> import_paths) noexcept{
  auto lines = mlc::String("", 0);
  auto index = 0;
  auto names = using_namespace_names(import_paths);
  while ((index < names.length()))   {
    (lines = (((lines + mlc::String("using namespace ", 16)) + names[index]) + mlc::String(";\n", 2)));
    (index = (index + 1));
  }
  return lines;
}

} // namespace cpp_naming

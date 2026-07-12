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
    (i = mlc::arith::checked_add(i, 1));
  }
  if ((last_dot > last_slash))   {
    return path.substring(mlc::arith::checked_add(last_slash, 1), mlc::arith::checked_sub(mlc::arith::checked_sub(last_dot, last_slash), 1));
  } else   {
    return path.substring(mlc::arith::checked_add(last_slash, 1), mlc::arith::checked_sub(mlc::arith::checked_sub(path.length(), last_slash), 1));
  }
}
mlc::Array<mlc::String> cpp_reserved_keywords() noexcept{
  auto keywords = mlc::Array<mlc::String>{mlc::String("export", 6), mlc::String("import", 6), mlc::String("module", 6), mlc::String("struct", 6), mlc::String("class", 5), mlc::String("template", 8), mlc::String("typename", 8), mlc::String("const", 5), mlc::String("return", 6), mlc::String("if", 2), mlc::String("else", 4), mlc::String("for", 3), mlc::String("while", 5), mlc::String("do", 2), mlc::String("switch", 6), mlc::String("case", 4), mlc::String("default", 7), mlc::String("break", 5), mlc::String("continue", 8), mlc::String("try", 3), mlc::String("catch", 5), mlc::String("throw", 5), mlc::String("new", 3), mlc::String("delete", 6), mlc::String("virtual", 7), mlc::String("override", 8), mlc::String("final", 5), mlc::String("public", 6), mlc::String("private", 7), mlc::String("protected", 9), mlc::String("friend", 6), mlc::String("static", 6), mlc::String("extern", 6), mlc::String("inline", 6), mlc::String("using", 5), mlc::String("namespace", 9), mlc::String("typedef", 7), mlc::String("enum", 4), mlc::String("union", 5), mlc::String("true", 4), mlc::String("false", 5), mlc::String("nullptr", 7), mlc::String("void", 4), mlc::String("bool", 4), mlc::String("int", 3), mlc::String("long", 4), mlc::String("short", 5), mlc::String("char", 4), mlc::String("float", 5), mlc::String("double", 6), mlc::String("signed", 6), mlc::String("unsigned", 8), mlc::String("auto", 4), mlc::String("typeid", 6), mlc::String("sizeof", 6), mlc::String("and", 3), mlc::String("or", 2), mlc::String("not", 3), mlc::String("xor", 3), mlc::String("and_eq", 6), mlc::String("or_eq", 5), mlc::String("not_eq", 6), mlc::String("xor_eq", 6), mlc::String("bitand", 6), mlc::String("bitor", 5), mlc::String("compl", 5), mlc::String("alignas", 7), mlc::String("alignof", 7), mlc::String("asm", 3), mlc::String("concept", 7), mlc::String("consteval", 9), mlc::String("constexpr", 9), mlc::String("constinit", 9), mlc::String("const_cast", 10), mlc::String("decltype", 8), mlc::String("dynamic_cast", 12), mlc::String("explicit", 8), mlc::String("mutable", 7), mlc::String("noexcept", 8), mlc::String("operator", 8), mlc::String("reinterpret_cast", 16), mlc::String("requires", 8), mlc::String("static_cast", 11), mlc::String("this", 4), mlc::String("thread_local", 12), mlc::String("volatile", 8), mlc::String("wchar_t", 7)};
  return keywords;
}
bool cpp_keyword(mlc::String name) noexcept{
  return cpp_reserved_keywords().any([=](mlc::String keyword) mutable { return (name == keyword); });
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
  if ((method_name == mlc::String("length", 6)))   {
    return mlc::String("length", 6);
  } else if ((method_name == mlc::String("len", 3)))   {
    return mlc::String("length", 6);
  } else if ((method_name == mlc::String("push", 4)))   {
    return mlc::String("push_back", 9);
  } else if ((method_name == mlc::String("recv", 4)))   {
    return mlc::String("receive", 7);
  } else if ((method_name == mlc::String("to_string", 9)))   {
    return mlc::String("to_string", 9);
  } else if ((method_name == mlc::String("to_int", 6)))   {
    return mlc::String("to_i", 4);
  } else if ((method_name == mlc::String("to_i", 4)))   {
    return mlc::String("to_i", 4);
  } else if ((method_name == mlc::String("upper", 5)))   {
    return mlc::String("upper", 5);
  } else if ((method_name == mlc::String("to_upper", 8)))   {
    return mlc::String("upper", 5);
  } else if ((method_name == mlc::String("lower", 5)))   {
    return mlc::String("lower", 5);
  } else if ((method_name == mlc::String("to_lower", 8)))   {
    return mlc::String("lower", 5);
  } else   {
    return method_name;
  }
}
mlc::String map_builtin(mlc::String name) noexcept{
  if ((name == mlc::String("print", 5)))   {
    return mlc::String("mlc::io::print", 14);
  } else if ((name == mlc::String("println", 7)))   {
    return mlc::String("mlc::io::println", 16);
  } else if ((name == mlc::String("eprintln", 8)))   {
    return mlc::String("mlc::io::eprintln", 17);
  } else if ((name == mlc::String("exit", 4)))   {
    return mlc::String("mlc::io::exit", 13);
  } else if ((name == mlc::String("args", 4)))   {
    return mlc::String("mlc::io::args", 13);
  } else if ((name == mlc::String("read_line", 9)))   {
    return mlc::String("mlc::io::read_line", 18);
  } else if ((name == mlc::String("read_all", 8)))   {
    return mlc::String("mlc::io::read_all", 17);
  } else if ((name == mlc::String("spawn_task", 10)))   {
    return mlc::String("mlc::concurrency::spawn_task", 28);
  } else if ((name == mlc::String("__task_scope_new", 16)))   {
    return mlc::String("mlc::concurrency::TaskScope", 27);
  } else if ((name == mlc::String("make_channel", 12)))   {
    return mlc::String("mlc::concurrency::make_channel", 30);
  } else if ((name == mlc::String("is_ready", 8)))   {
    return mlc::String("mlc::is_ready", 13);
  } else if ((name == mlc::String("block_on", 8)))   {
    return mlc::String("mlc::block_on", 13);
  } else   {
    return name;
  }
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
    (i = mlc::arith::checked_add(i, 1));
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
    (index = mlc::arith::checked_add(index, 1));
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
    (index = mlc::arith::checked_add(index, 1));
  }
  return lines;
}

} // namespace cpp_naming

#define main mlc_user_main
#include "path_normalize.hpp"

#include "header_import.hpp"

namespace path_normalize {

using namespace header_import;

bool path_character_is_safe(mlc::String character) noexcept{
  return ((((((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("A", 1)) && (character <= mlc::String("Z", 1)))) || ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)))) || (character == mlc::String("_", 1))) || (character == mlc::String("/", 1))) || (character == mlc::String(".", 1))) || (character == mlc::String("-", 1)));
}
bool path_contains_parent_segment(mlc::String path) noexcept{
  auto index = 0;
  while ((mlc::arith::checked_add(index, 1) < path.length()))   {
    if (((path.char_at(index) == mlc::String(".", 1)) && (path.char_at(mlc::arith::checked_add(index, 1)) == mlc::String(".", 1))))     {
      return true;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return false;
}
bool driver_source_path_is_safe(mlc::String path) noexcept{
  if (((path == mlc::String("", 0)) || (path.length() > 512)))   {
    return false;
  } else if (path_contains_parent_segment(path))   {
    return false;
  } else   {
    auto index = 0;
    while ((index < path.length()))     {
      if ((!path_character_is_safe(path.char_at(index))))       {
        return false;
      }
      (index = mlc::arith::checked_add(index, 1));
    }
    return true;
  }
}
mlc::String dirname(mlc::String path) noexcept{
  auto last_slash_index = (-1);
  auto path_character_index = 0;
  while ((path_character_index < path.length()))   {
    if ((path.char_at(path_character_index) == mlc::String("/", 1)))     {
      (last_slash_index = path_character_index);
    }
    (path_character_index = mlc::arith::checked_add(path_character_index, 1));
  }
  if ((last_slash_index <= 0))   {
    return mlc::String("", 0);
  } else   {
    return path.substring(0, last_slash_index);
  }
}
mlc::String resolve_dotdot(mlc::String path) noexcept{
  auto remaining_path = path;
  auto scan_index = 0;
  while ((mlc::arith::checked_add(scan_index, 4) <= remaining_path.length()))   {
    if ((remaining_path.substring(scan_index, 4) == mlc::String("/../", 4)))     {
      auto reverse_segment_index = mlc::arith::checked_sub(scan_index, 1);
      while (((reverse_segment_index >= 0) && (remaining_path.char_at(reverse_segment_index) != mlc::String("/", 1))))       {
        (reverse_segment_index = mlc::arith::checked_sub(reverse_segment_index, 1));
      }
      auto prefix_part = ((reverse_segment_index <= 0) ? (mlc::String("", 0)) : (remaining_path.substring(0, reverse_segment_index)));
      auto suffix_part = remaining_path.substring(mlc::arith::checked_add(scan_index, 4), mlc::arith::checked_sub(mlc::arith::checked_sub(remaining_path.length(), scan_index), 4));
      (remaining_path = ((prefix_part == mlc::String("", 0)) ? (suffix_part) : (((((mlc::String("", 0) + mlc::to_string(prefix_part)) + mlc::String("/", 1)) + mlc::to_string(suffix_part)) + mlc::String("", 0)))));
      (scan_index = 0);
    }
    (scan_index = mlc::arith::checked_add(scan_index, 1));
  }
  if (((remaining_path.length() >= 3) && (remaining_path.substring(0, 3) == mlc::String("../", 3))))   {
    (remaining_path = remaining_path.substring(3, mlc::arith::checked_sub(remaining_path.length(), 3)));
  }
  return remaining_path;
}
bool import_path_contains_slash(mlc::String import_path) noexcept{
  auto index = 0;
  while ((index < import_path.length()))   {
    if ((import_path.char_at(index) == mlc::String("/", 1)))     {
      return true;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return false;
}
bool is_bare_module_import_name(mlc::String import_path) noexcept{
  return (((import_path.length() > 0) && (import_path.char_at(0) != mlc::String(".", 1))) && (!import_path_contains_slash(import_path)));
}
mlc::String stdlib_module_relative_path(mlc::String module_name) noexcept{
  if ((module_name == mlc::String("Tcp", 3)))   {
    return mlc::String("net/tcp.mlc", 11);
  } else if ((module_name == mlc::String("HttpServer", 10)))   {
    return mlc::String("net/http_server.mlc", 19);
  } else if ((module_name == mlc::String("Env", 3)))   {
    return mlc::String("env/env.mlc", 11);
  } else if ((module_name == mlc::String("Log", 3)))   {
    return mlc::String("log/logger.mlc", 14);
  } else if ((module_name == mlc::String("Validate", 8)))   {
    return mlc::String("validate/validate.mlc", 21);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String parent_directory(mlc::String path) noexcept{
  if (((path == mlc::String("", 0)) || (path == mlc::String(".", 1))))   {
    return mlc::String("", 0);
  } else   {
    auto directory = dirname(path);
    if ((directory != mlc::String("", 0)))     {
      return directory;
    } else     {
      return mlc::String(".", 1);
    }
  }
}
mlc::String find_stdlib_root(mlc::String from_path) noexcept{
  auto directory = dirname(from_path);
  if ((directory == mlc::String("", 0)))   {
    (directory = mlc::String(".", 1));
  }
  auto attempts = 0;
  while (((attempts < 48) && (directory != mlc::String("", 0))))   {
    auto candidate_root = ((directory == mlc::String(".", 1)) ? (mlc::String("lib/mlc/common/stdlib", 21)) : (((mlc::String("", 0) + mlc::to_string(directory)) + mlc::String("/lib/mlc/common/stdlib", 22))));
    if (mlc::file::exists(((mlc::String("", 0) + mlc::to_string(candidate_root)) + mlc::String("/net/tcp.mlc", 12))))     {
      return candidate_root;
    }
    (directory = parent_directory(directory));
    (attempts = mlc::arith::checked_add(attempts, 1));
  }
  return mlc::String("", 0);
}
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
  if (is_bare_module_import_name(import_path))   {
    auto relative_path = stdlib_module_relative_path(import_path);
    if ((relative_path != mlc::String("", 0)))     {
      auto stdlib_root = find_stdlib_root(base_path);
      if ((stdlib_root != mlc::String("", 0)))       {
        return resolve_dotdot(((((mlc::String("", 0) + mlc::to_string(stdlib_root)) + mlc::String("/", 1)) + mlc::to_string(relative_path)) + mlc::String("", 0)));
      }
    }
  }
  auto base_dir = dirname(base_path);
  auto rest = (((import_path.length() >= 2) && (import_path.substring(0, 2) == mlc::String("./", 2))) ? (import_path.substring(2, mlc::arith::checked_sub(import_path.length(), 2))) : (import_path));
  auto with_extension = (header_import::is_cpp_header_path(rest) ? (rest) : ((((rest.length() >= 4) && (rest.substring(mlc::arith::checked_sub(rest.length(), 4), 4) == mlc::String(".mlc", 4))) ? (rest) : (((mlc::String("", 0) + mlc::to_string(rest)) + mlc::String(".mlc", 4))))));
  auto combined_before_normalization = ((base_dir == mlc::String("", 0)) ? (with_extension) : (((((mlc::String("", 0) + mlc::to_string(base_dir)) + mlc::String("/", 1)) + mlc::to_string(with_extension)) + mlc::String("", 0))));
  return resolve_dotdot(combined_before_normalization);
}

} // namespace path_normalize

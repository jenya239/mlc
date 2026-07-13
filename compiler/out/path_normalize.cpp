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
mlc::String find_project_root(mlc::String from_path) noexcept{
  auto directory = dirname(from_path);
  if ((directory == mlc::String("", 0)))   {
    (directory = mlc::String(".", 1));
  }
  auto attempts = 0;
  while (((attempts < 48) && (directory != mlc::String("", 0))))   {
    auto manifest_path = ((directory == mlc::String(".", 1)) ? (mlc::String("mlc.json", 8)) : (((mlc::String("", 0) + mlc::to_string(directory)) + mlc::String("/mlc.json", 9))));
    if (mlc::file::exists(manifest_path))     {
      return directory;
    }
    (directory = parent_directory(directory));
    (attempts = mlc::arith::checked_add(attempts, 1));
  }
  return mlc::String("", 0);
}
int string_index_of(mlc::String haystack, mlc::String needle) noexcept{
  if ((needle.length() == 0))   {
    return 0;
  }
  if ((needle.length() > haystack.length()))   {
    return (-1);
  }
  auto index = 0;
  while ((mlc::arith::checked_add(index, needle.length()) <= haystack.length()))   {
    if ((haystack.substring(index, needle.length()) == needle))     {
      return index;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return (-1);
}
bool path_has_prefix(mlc::String path, mlc::String prefix) noexcept{
  return ((path.length() >= prefix.length()) && (path.substring(0, prefix.length()) == prefix));
}
bool is_package_name_charset(mlc::String package_name) noexcept{
  if ((package_name.length() == 0))   {
    return false;
  }
  if ((package_name == mlc::String("mlc_packages", 12)))   {
    return false;
  }
  auto first_character = package_name.char_at(0);
  if ((!((first_character >= mlc::String("a", 1)) && (first_character <= mlc::String("z", 1)))))   {
    return false;
  }
  auto index = 1;
  while ((index < package_name.length()))   {
    auto character = package_name.char_at(index);
    if ((!((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)))) || (character == mlc::String("_", 1)))))     {
      return false;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return true;
}
mlc::String first_path_segment(mlc::String import_path) noexcept{
  auto index = 0;
  while ((index < import_path.length()))   {
    if ((import_path.char_at(index) == mlc::String("/", 1)))     {
      return import_path.substring(0, index);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return import_path;
}
mlc::String path_after_first_segment(mlc::String import_path) noexcept{
  auto index = 0;
  while ((index < import_path.length()))   {
    if ((import_path.char_at(index) == mlc::String("/", 1)))     {
      return import_path.substring(mlc::arith::checked_add(index, 1), mlc::arith::checked_sub(mlc::arith::checked_sub(import_path.length(), index), 1));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return mlc::String("", 0);
}
bool manifest_lists_dependency(mlc::String manifest_text, mlc::String package_name) noexcept{
  auto dependencies_index = string_index_of(manifest_text, mlc::String("\"dependencies\"", 14));
  if ((dependencies_index < 0))   {
    return false;
  }
  auto key_token = ((mlc::String("\"", 1) + mlc::to_string(package_name)) + mlc::String("\"", 1));
  auto index = dependencies_index;
  auto found = false;
  while (((mlc::arith::checked_add(index, key_token.length()) <= manifest_text.length()) && (!found)))   {
    if ((manifest_text.substring(index, key_token.length()) == key_token))     {
      auto after = mlc::arith::checked_add(index, key_token.length());
      auto scan_done = false;
      [&]() {
while (((after < manifest_text.length()) && (!scan_done))) {
auto character = manifest_text.char_at(after);
if (((((character == mlc::String(" ", 1)) || (character == mlc::String("\n", 1))) || (character == mlc::String("\r", 1))) || (character == mlc::String("\t", 1)))) {
  (after = mlc::arith::checked_add(after, 1));
} else if ((character == mlc::String(":", 1))) {
  (found = true);
  (scan_done = true);
} else {
  (scan_done = true);
}
}
}();
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return found;
}
mlc::String append_mlc_extension_if_needed(mlc::String module_path) noexcept{
  if (header_import::is_cpp_header_path(module_path))   {
    return module_path;
  } else if (((module_path.length() >= 4) && (module_path.substring(mlc::arith::checked_sub(module_path.length(), 4), 4) == mlc::String(".mlc", 4))))   {
    return module_path;
  } else   {
    return ((mlc::String("", 0) + mlc::to_string(module_path)) + mlc::String(".mlc", 4));
  }
}
mlc::String try_resolve_package_import(mlc::String base_path, mlc::String import_path) noexcept{
  if ((import_path.length() == 0))   {
    return mlc::String("", 0);
  }
  if ((import_path.char_at(0) == mlc::String(".", 1)))   {
    return mlc::String("", 0);
  }
  if ((!import_path_contains_slash(import_path)))   {
    return mlc::String("", 0);
  }
  auto package_name = first_path_segment(import_path);
  if ((!is_package_name_charset(package_name)))   {
    return mlc::String("", 0);
  }
  auto project_root = find_project_root(base_path);
  if ((project_root == mlc::String("", 0)))   {
    return mlc::String("", 0);
  }
  auto manifest_path = ((project_root == mlc::String(".", 1)) ? (mlc::String("mlc.json", 8)) : (((mlc::String("", 0) + mlc::to_string(project_root)) + mlc::String("/mlc.json", 9))));
  if ((!mlc::file::exists(manifest_path)))   {
    return mlc::String("", 0);
  }
  auto manifest_text = mlc::file::read_to_string(manifest_path);
  if ((!manifest_lists_dependency(manifest_text, package_name)))   {
    return mlc::String("", 0);
  }
  auto module_rest = path_after_first_segment(import_path);
  if ((module_rest.length() == 0))   {
    return mlc::String("", 0);
  }
  auto with_extension = append_mlc_extension_if_needed(module_rest);
  auto package_root = ((project_root == mlc::String(".", 1)) ? (((mlc::String(".mlc_packages/", 14) + mlc::to_string(package_name)) + mlc::String("", 0))) : (((((mlc::String("", 0) + mlc::to_string(project_root)) + mlc::String("/.mlc_packages/", 15)) + mlc::to_string(package_name)) + mlc::String("", 0))));
  auto normalized = resolve_dotdot(((((mlc::String("", 0) + mlc::to_string(package_root)) + mlc::String("/", 1)) + mlc::to_string(with_extension)) + mlc::String("", 0)));
  auto required_prefix = ((mlc::String("", 0) + mlc::to_string(package_root)) + mlc::String("/", 1));
  if (path_has_prefix(normalized, required_prefix))   {
    return normalized;
  } else   {
    return mlc::String("", 0);
  }
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
  auto package_resolved = try_resolve_package_import(base_path, import_path);
  if ((package_resolved != mlc::String("", 0)))   {
    return package_resolved;
  }
  auto base_dir = dirname(base_path);
  auto rest = (((import_path.length() >= 2) && (import_path.substring(0, 2) == mlc::String("./", 2))) ? (import_path.substring(2, mlc::arith::checked_sub(import_path.length(), 2))) : (import_path));
  auto with_extension = append_mlc_extension_if_needed(rest);
  auto combined_before_normalization = ((base_dir == mlc::String("", 0)) ? (with_extension) : (((((mlc::String("", 0) + mlc::to_string(base_dir)) + mlc::String("/", 1)) + mlc::to_string(with_extension)) + mlc::String("", 0))));
  return resolve_dotdot(combined_before_normalization);
}

} // namespace path_normalize

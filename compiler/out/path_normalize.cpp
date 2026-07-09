#include "path_normalize.hpp"

#include "header_import.hpp"

namespace path_normalize {

using namespace header_import;

bool path_character_is_safe(mlc::String character) noexcept;

bool path_contains_parent_segment(mlc::String path) noexcept;

bool driver_source_path_is_safe(mlc::String path) noexcept;

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

bool path_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_") || character == mlc::String("/") || character == mlc::String(".") || character == mlc::String("-");}

bool path_contains_parent_segment(mlc::String path) noexcept{
int index = 0;
while (index + 1 < path.length()){
{
if (path.char_at(index) == mlc::String(".") && path.char_at(index + 1) == mlc::String(".")){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

bool driver_source_path_is_safe(mlc::String path) noexcept{
return path == mlc::String("") || path.length() > 512 ? false : path_contains_parent_segment(path) ? false : [&]() -> bool { 
  int index = 0;
  while (index < path.length()){
{
if (!path_character_is_safe(path.char_at(index))){
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

mlc::String dirname(mlc::String path) noexcept{
int last_slash_index = -1;
int path_character_index = 0;
while (path_character_index < path.length()){
{
if (path.char_at(path_character_index) == mlc::String("/")){
{
last_slash_index = path_character_index;
}
}
path_character_index = path_character_index + 1;
}
}
return last_slash_index <= 0 ? mlc::String("") : path.substring(0, last_slash_index);
}

mlc::String resolve_dotdot(mlc::String path) noexcept{
mlc::String remaining_path = path;
int scan_index = 0;
while (scan_index + 4 <= remaining_path.length()){
{
if (remaining_path.substring(scan_index, 4) == mlc::String("/../")){
{
int reverse_segment_index = scan_index - 1;
while (reverse_segment_index >= 0 && remaining_path.char_at(reverse_segment_index) != mlc::String("/")){
{
reverse_segment_index = reverse_segment_index - 1;
}
}
mlc::String prefix_part = reverse_segment_index <= 0 ? mlc::String("") : remaining_path.substring(0, reverse_segment_index);
mlc::String suffix_part = remaining_path.substring(scan_index + 4, remaining_path.length() - scan_index - 4);
remaining_path = prefix_part == mlc::String("") ? suffix_part : prefix_part + mlc::String("/") + suffix_part;
scan_index = 0;
}
}
scan_index = scan_index + 1;
}
}
if (remaining_path.length() >= 3 && remaining_path.substring(0, 3) == mlc::String("../")){
{
remaining_path = remaining_path.substring(3, remaining_path.length() - 3);
}
}
return remaining_path;
}

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
mlc::String base_dir = dirname(base_path);
mlc::String rest = import_path.length() >= 2 && import_path.substring(0, 2) == mlc::String("./") ? import_path.substring(2, import_path.length() - 2) : import_path;
mlc::String with_extension = header_import::is_cpp_header_path(rest) ? rest : rest.length() >= 4 && rest.substring(rest.length() - 4, 4) == mlc::String(".mlc") ? rest : rest + mlc::String(".mlc");
mlc::String combined_before_normalization = base_dir == mlc::String("") ? with_extension : base_dir + mlc::String("/") + with_extension;
return resolve_dotdot(combined_before_normalization);
}

} // namespace path_normalize

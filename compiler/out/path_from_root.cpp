#include "path_from_root.hpp"

namespace path_from_root {

bool test_relative_path_character_is_safe(mlc::String character) noexcept;

bool test_relative_path_contains_parent_segment(mlc::String relative_path) noexcept;

bool test_relative_path_is_safe(mlc::String relative_path) noexcept;

mlc::String resolve_test_fixture_path(mlc::String relative_path) noexcept;

bool test_relative_path_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_") || character == mlc::String("/") || character == mlc::String(".") || character == mlc::String("-");}

bool test_relative_path_contains_parent_segment(mlc::String relative_path) noexcept{
int index = 0;
while (index + 1 < relative_path.length()){
{
if (relative_path.char_at(index) == mlc::String(".") && relative_path.char_at(index + 1) == mlc::String(".")){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

bool test_relative_path_is_safe(mlc::String relative_path) noexcept{
return relative_path == mlc::String("") || relative_path.length() > 512 ? false : test_relative_path_contains_parent_segment(relative_path) ? false : relative_path.char_at(0) == mlc::String("/") ? false : [&]() -> bool { 
  int index = 0;
  while (index < relative_path.length()){
{
if (!test_relative_path_character_is_safe(relative_path.char_at(index))){
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

mlc::String resolve_test_fixture_path(mlc::String relative_path) noexcept{return mlc::String("compiler/tests/") + relative_path;}

} // namespace path_from_root

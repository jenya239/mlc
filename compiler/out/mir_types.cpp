#include "mir_types.hpp"

#include "registry.hpp"

namespace mir_types {

using namespace registry;

mir_types::LocalId mir_local_id(int index) noexcept;

mir_types::BlockId mir_block_id(int index) noexcept;

bool mir_id_is_valid(int index) noexcept;

mir_types::MirProgram mir_program_empty() noexcept;

bool mir_name_character_is_safe(mlc::String character) noexcept;

bool mir_function_name_is_safe(mlc::String name) noexcept;

bool mir_block_label_is_safe(mlc::String label) noexcept;

mir_types::LocalId mir_local_id(int index) noexcept{return mir_types::LocalId{index};}

mir_types::BlockId mir_block_id(int index) noexcept{return mir_types::BlockId{index};}

bool mir_id_is_valid(int index) noexcept{return index >= 0 && index < 1048576;}

mir_types::MirProgram mir_program_empty() noexcept{return mir_types::MirProgram{{}};}

bool mir_name_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_");}

bool mir_function_name_is_safe(mlc::String name) noexcept{
return name == mlc::String("") || name.length() > 128 ? false : [&]() -> bool { 
  int index = 0;
  while (index < name.length()){
{
if (!mir_name_character_is_safe(name.char_at(index))){
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

bool mir_block_label_is_safe(mlc::String label) noexcept{
return label == mlc::String("") || label.length() > 64 ? false : [&]() -> bool { 
  int index = 0;
  while (index < label.length()){
{
if (!mir_name_character_is_safe(label.char_at(index))){
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

} // namespace mir_types
